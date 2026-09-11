#include <algorithm>
#include <windows.h>
#include <commctrl.h>
#include <Uxtheme.h>
#include <dwmapi.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include <thread>
#include <mutex>
#include <iomanip>
#include <chrono>

// Forward declarations
void AppendToTerminal(const std::string& text);
void PrintSystemStats();

void PrintSystemStats() {
    AppendToTerminal("NatLP Console - build 0.9.4");
    
    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    char timeStr[100];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now_c));
    AppendToTerminal("Session opened " + std::string(timeStr));
    AppendToTerminal("");
    AppendToTerminal("Good evening. I'm here when you need me.");
    AppendToTerminal("");
    
    // Get Memory
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    double totalPhysMem = (double)memInfo.ullTotalPhys / (1024 * 1024 * 1024);
    double physMemUsed = totalPhysMem - ((double)memInfo.ullAvailPhys / (1024 * 1024 * 1024));
    
    // Get CPU Cores
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    
    // Get Uptime
    ULONGLONG uptimeMs = GetTickCount64();
    int hours = uptimeMs / 3600000;
    int minutes = (uptimeMs % 3600000) / 60000;
    int seconds = ((uptimeMs % 3600000) % 60000) / 1000;
    
    AppendToTerminal("All systems nominal. 0 background tasks running.");
    
    std::stringstream ssMem;
    ssMem << std::fixed << std::setprecision(1);
    ssMem << "Memory " << physMemUsed << " GB / " << totalPhysMem << " GB [" << memInfo.dwMemoryLoad << "%]";
    AppendToTerminal(ssMem.str());
    
    std::stringstream ssCpu;
    ssCpu << "CPU avg across " << sysInfo.dwNumberOfProcessors << " cores";
    AppendToTerminal(ssCpu.str());
    
    std::stringstream ssUp;
    ssUp << "Uptime " << hours << "h " << minutes << "m " << seconds << "s";
    AppendToTerminal(ssUp.str());
    AppendToTerminal("Tasks None");
}


#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "dwmapi.lib")

#define WM_APP_TERMINAL_OUTPUT (WM_APP + 1)

HWND hwndMain = NULL;
HWND hwndTree = NULL;
HWND hwndEditor = NULL;
HWND hwndTerminal = NULL;
HWND hwndInput = NULL;

int LEFT_PANE_WIDTH = 250;
int BOTTOM_PANE_HEIGHT = 200;
const int INPUT_HEIGHT = 25;
const int SPLITTER_SIZE = 5;

bool isDraggingVert = false;
bool isDraggingHorz = false;
int  trackerPos     = -1;  // last drawn ghost line position (screen coords in client space)

// Draw or erase a ghost splitter line using XOR (call twice to erase)
void DrawSplitterTracker(HWND hwnd, int pos, bool horz) {
    HDC hdc = GetDC(hwnd);
    RECT rc; GetClientRect(hwnd, &rc);
    HBRUSH hb = CreateSolidBrush(RGB(90, 90, 90));
    HBRUSH old = (HBRUSH)SelectObject(hdc, hb);
    if (horz) {
        PatBlt(hdc, LEFT_PANE_WIDTH + SPLITTER_SIZE, pos - 2, rc.right, 4, PATINVERT);
    } else {
        PatBlt(hdc, pos - 2, 0, 4, rc.bottom, PATINVERT);
    }
    SelectObject(hdc, old);
    DeleteObject(hb);
    ReleaseDC(hwnd, hdc);
}

HINSTANCE hInst;
bool psycheMode = false;
HBRUSH hDarkBrush = NULL;

// ── Workspace config ─────────────────────────────────────────────────────────
// Stored in %APPDATA%\Spatial Studio\workspace.cfg (one line: the root path)
std::string g_workspaceRoot;   // e.g. "C:\Users\alice\my-project"
std::string stateDir;          // set once workspace is known

std::string GetConfigDir() {
    char buf[MAX_PATH];
    ExpandEnvironmentStringsA("%APPDATA%\\Axi IDE", buf, MAX_PATH);
    return std::string(buf);
}

void LoadWorkspaceConfig() {
    std::string cfgPath = GetConfigDir() + "\\workspace.cfg";
    std::ifstream f(cfgPath);
    if (f.is_open()) {
        std::getline(f, g_workspaceRoot);
        // trim trailing whitespace / CR
        while (!g_workspaceRoot.empty() &&
               (g_workspaceRoot.back() == '\r' || g_workspaceRoot.back() == '\n' ||
                g_workspaceRoot.back() == ' '))
            g_workspaceRoot.pop_back();
    }
    if (!g_workspaceRoot.empty()) {
        stateDir = g_workspaceRoot + "\\.psyche_state";
    } else {
        // Fallback: no root declared yet — psyche IPC unavailable
        stateDir = GetConfigDir() + "\\.psyche_state";
    }
}

void SaveWorkspaceConfig(const std::string& root) {
    std::string dir = GetConfigDir();
    CreateDirectoryA(dir.c_str(), NULL);
    std::ofstream f(dir + "\\workspace.cfg");
    f << root << "\n";
    g_workspaceRoot = root;
    stateDir = root + "\\.psyche_state";
    CreateDirectoryA(stateDir.c_str(), NULL);
}
// ─────────────────────────────────────────────────────────────────────────────

std::vector<std::string> terminalQueue;
std::mutex terminalMutex;

void AppendToTerminal(const std::string& text) {
    SendMessageA(hwndTerminal, 2171, 0, 0); 
    SendMessageA(hwndTerminal, 2282, text.length(), (LPARAM)text.c_str());
    SendMessageA(hwndTerminal, 2282, 1, (LPARAM)"\n");
    int length = SendMessageA(hwndTerminal, 2006, 0, 0);
    SendMessageA(hwndTerminal, 2025, length, 0);
    SendMessageA(hwndTerminal, 2171, 1, 0); 
}

void RunCommandAsync(std::string cmd) {
    std::string execCmd = "cmd.exe /C \"cd C:\\Ethos && " + cmd + " 2>&1\"";

    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hReadPipe, hWritePipe;
    CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si = {};
    si.cb = sizeof(STARTUPINFOA);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hWritePipe;
    si.hStdError  = hWritePipe;
    si.hStdInput  = NULL;

    PROCESS_INFORMATION pi = {};
    if (CreateProcessA(NULL, (LPSTR)execCmd.c_str(), NULL, NULL, TRUE,
                       CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(hWritePipe);

        char buf[256];
        DWORD bytesRead;
        std::string partial;
        while (ReadFile(hReadPipe, buf, sizeof(buf) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buf[bytesRead] = '\0';
            partial += buf;
            // Flush complete lines
            size_t pos;
            while ((pos = partial.find('\n')) != std::string::npos) {
                std::string line = partial.substr(0, pos);
                if (!line.empty() && line.back() == '\r') line.pop_back();
                std::lock_guard<std::mutex> lock(terminalMutex);
                terminalQueue.push_back(line);
                PostMessage(hwndMain, WM_APP_TERMINAL_OUTPUT, 0, 0);
                partial = partial.substr(pos + 1);
            }
        }
        // Flush any remaining output
        if (!partial.empty()) {
            std::lock_guard<std::mutex> lock(terminalMutex);
            terminalQueue.push_back(partial);
            PostMessage(hwndMain, WM_APP_TERMINAL_OUTPUT, 0, 0);
        }

        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        std::lock_guard<std::mutex> lock(terminalMutex);
        terminalQueue.push_back("[ERROR] Failed to launch command.");
        PostMessage(hwndMain, WM_APP_TERMINAL_OUTPUT, 0, 0);
        CloseHandle(hWritePipe);
    }
    CloseHandle(hReadPipe);
}

struct TreeNode {
    std::string name;
    std::map<std::string, TreeNode*> children;
    HTREEITEM hItem;
};

// Maps each HTREEITEM -> full absolute path on disk (leaf files only)
std::map<HTREEITEM, std::string> treeItemPaths;

void InsertTreeItem(HWND hwndTree, TreeNode* node, HTREEITEM hParent, const std::string& parentPath) {
    TVINSERTSTRUCTA tvis = {0};
    tvis.hParent = hParent;
    tvis.hInsertAfter = TVI_SORT;
    tvis.item.mask = TVIF_TEXT;
    tvis.item.pszText = (LPSTR)node->name.c_str();

    node->hItem = (HTREEITEM)SendMessageA(hwndTree, TVM_INSERTITEMA, 0, (LPARAM)&tvis);

    // Build accumulated path — skip root display label (parentPath == "" AND hParent == TVI_ROOT)
    std::string fullPath = parentPath.empty() ? "" : parentPath + "\\" + node->name;
    // For the first real level below root, fullPath is still empty; start it from this node's name
    if (fullPath.empty() && hParent != TVI_ROOT) {
        fullPath = node->name;
    } else if (fullPath.empty() && hParent == TVI_ROOT) {
        // This IS the root display node — don't record a path; recurse with empty
        for (auto const& [key, child] : node->children) {
            InsertTreeItem(hwndTree, child, node->hItem, "");
        }
        return;
    }

    if (node->children.empty()) {
        // Leaf = file. Manifest paths: ethos-logos\AGENTS.md → C:\Ethos\ethos-logos\AGENTS.md
        treeItemPaths[node->hItem] = "C:\\Ethos\\" + fullPath;
    }

    for (auto const& [key, child] : node->children) {
        InsertTreeItem(hwndTree, child, node->hItem, fullPath);
    }
}

// Directories to skip during filesystem walk
static const std::vector<std::string> SKIP_DIRS = {
    ".git", ".axi", "node_modules", "bin", "obj", "bootstrap",
    ".user_uploaded", "__pycache__", ".vs", ".vscode"
};

bool ShouldSkipDir(const std::string& name) {
    for (const auto& skip : SKIP_DIRS) {
        if (_stricmp(name.c_str(), skip.c_str()) == 0) return true;
    }
    return false;
}

// Recursive native Win32 filesystem walker - populates the TreeView directly
void WalkDirectory(const std::string& dirPath, HTREEITEM hParent) {
    WIN32_FIND_DATAA ffd;
    std::string searchPath = dirPath + "\\*";
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &ffd);
    if (hFind == INVALID_HANDLE_VALUE) return;

    std::vector<std::string> dirs, files;
    do {
        std::string name = ffd.cFileName;
        if (name == "." || name == "..") continue;
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!ShouldSkipDir(name)) dirs.push_back(name);
        } else {
            files.push_back(name);
        }
    } while (FindNextFileA(hFind, &ffd));
    FindClose(hFind);

    // Sort both lists
    std::sort(dirs.begin(), dirs.end(), [](const std::string& a, const std::string& b){
        return _stricmp(a.c_str(), b.c_str()) < 0;
    });
    std::sort(files.begin(), files.end(), [](const std::string& a, const std::string& b){
        return _stricmp(a.c_str(), b.c_str()) < 0;
    });

    // Insert directories first
    for (const auto& dname : dirs) {
        TVINSERTSTRUCTA tvis = {0};
        tvis.hParent = hParent;
        tvis.hInsertAfter = TVI_LAST;
        tvis.item.mask = TVIF_TEXT;
        tvis.item.pszText = (LPSTR)dname.c_str();
        HTREEITEM hDir = (HTREEITEM)SendMessageA(hwndTree, TVM_INSERTITEMA, 0, (LPARAM)&tvis);
        WalkDirectory(dirPath + "\\" + dname, hDir);
    }

    // Then files
    for (const auto& fname : files) {
        std::string fullPath = dirPath + "\\" + fname;
        TVINSERTSTRUCTA tvis = {0};
        tvis.hParent = hParent;
        tvis.hInsertAfter = TVI_LAST;
        tvis.item.mask = TVIF_TEXT;
        tvis.item.pszText = (LPSTR)fname.c_str();
        HTREEITEM hFile = (HTREEITEM)SendMessageA(hwndTree, TVM_INSERTITEMA, 0, (LPARAM)&tvis);
        treeItemPaths[hFile] = fullPath;
    }
}

void LoadFileSystemTree() {
    treeItemPaths.clear();
    SendMessageA(hwndTree, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
    SendMessage(hwndTree, WM_SETREDRAW, FALSE, 0);

    bool isSmartLoaded = false;
    if (!g_workspaceRoot.empty()) {
        std::string axiPath = g_workspaceRoot + "\\.axi";
        DWORD attrib = GetFileAttributesA(axiPath.c_str());
        isSmartLoaded = (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY));
    }

    if (isSmartLoaded) {
        // Live filesystem tree — insert root node and walk from g_workspaceRoot
        TVINSERTSTRUCTA tvis = {0};
        tvis.hParent = TVI_ROOT;
        tvis.hInsertAfter = TVI_LAST;
        tvis.item.mask = TVIF_TEXT;
        tvis.item.pszText = (LPSTR)g_workspaceRoot.c_str();
        HTREEITEM hRoot = (HTREEITEM)SendMessageA(hwndTree, TVM_INSERTITEMA, 0, (LPARAM)&tvis);

        WalkDirectory(g_workspaceRoot, hRoot);
        SendMessageA(hwndTree, TVM_EXPAND, TVE_EXPAND, (LPARAM)hRoot);
    } else {
        // FOSS mock data — no working root declared yet
        TVINSERTSTRUCTA tvis = {0};
        tvis.hParent = TVI_ROOT;
        tvis.hInsertAfter = TVI_LAST;
        tvis.item.mask = TVIF_TEXT;
        tvis.item.pszText = (LPSTR)"Unmapped Workspace (Mock)";
        HTREEITEM hRoot = (HTREEITEM)SendMessageA(hwndTree, TVM_INSERTITEMA, 0, (LPARAM)&tvis);

        // Mock src/
        auto addMockDir = [&](HTREEITEM parent, const char* dname) {
            TVINSERTSTRUCTA t = {0}; t.hParent = parent; t.hInsertAfter = TVI_LAST;
            t.item.mask = TVIF_TEXT; t.item.pszText = (LPSTR)dname;
            return (HTREEITEM)SendMessageA(hwndTree, TVM_INSERTITEMA, 0, (LPARAM)&t);
        };
        auto addMockFile = [&](HTREEITEM parent, const char* fname) {
            TVINSERTSTRUCTA t = {0}; t.hParent = parent; t.hInsertAfter = TVI_LAST;
            t.item.mask = TVIF_TEXT; t.item.pszText = (LPSTR)fname;
            return (HTREEITEM)SendMessageA(hwndTree, TVM_INSERTITEMA, 0, (LPARAM)&t);
        };

        HTREEITEM hSrc  = addMockDir(hRoot,  "src");
                          addMockFile(hSrc,   "main.cpp");
                          addMockFile(hSrc,   "utils.h");
        HTREEITEM hDocs = addMockDir(hRoot,  "docs");
                          addMockFile(hDocs,  "README.md");
        HTREEITEM hTest = addMockDir(hRoot,  "tests");
                          addMockFile(hTest,  "test_main.cpp");
                          addMockFile(hRoot,  ".gitignore");

        SendMessageA(hwndTree, TVM_EXPAND, TVE_EXPAND, (LPARAM)hRoot);
    }

    SendMessage(hwndTree, WM_SETREDRAW, TRUE, 0);
}

LRESULT CALLBACK InputProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_KEYDOWN && wParam == VK_RETURN) {
        char buffer[1024] = {0};
        GetWindowTextA(hwnd, buffer, 1024);
        std::string cmd = buffer;
        SetWindowTextA(hwnd, "");
        if (!cmd.empty()) {
            AppendToTerminal("natlp ~ " + cmd);
            if (cmd == "import psyche") {
                psycheMode = true;
                AppendToTerminal("[SYSTEM] Psyche Cognitive Daemon linked to NatLP standard input.");
                AppendToTerminal("[SYSTEM] Native Axi cognitive core linked.");
                AppendToTerminal("[PSYCHE] I am online and listening. Type 'exit' to detach.");
            } else if (psycheMode) {
                if (cmd == "exit" || cmd == "detach") {
                    psycheMode = false;
                    AppendToTerminal("[SYSTEM] Psyche detached. Returning to local Axi shell.");
                } else {
                    AppendToTerminal("[PSYCHE] Processing IPC input...");
                    std::string inPath = stateDir + "\\chat_input.json";
                    std::ofstream out(inPath);
                    out << "{\"text\":\"" << cmd << "\"}\n";
                    out.close();
                }
            } else {
                if (cmd.find("axi root ") == 0) {
                    std::string newRoot = cmd.substr(9);
                    SaveWorkspaceConfig(newRoot);
                    AppendToTerminal("[SYSTEM] Workspace root updated to: " + newRoot);
                    LoadFileSystemTree();
                } else {
                    std::thread(RunCommandAsync, cmd).detach();
                    if (cmd.find("add") != std::string::npos || cmd.find("wrap") != std::string::npos) {
                        LoadFileSystemTree();
                    }
                }
            }
        }
        return 0;
    }
    // Suppress ding on Enter key - intercept WM_CHAR for '\r'
    if (uMsg == WM_CHAR && (wParam == VK_RETURN || wParam == '\r')) {
        return 0;
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_NOTIFY: {
            NMHDR* nmhdr = (NMHDR*)lParam;
            if (nmhdr->hwndFrom == hwndTree && nmhdr->code == TVN_SELCHANGEDA) {
                NMTREEVIEWA* nmtv = (NMTREEVIEWA*)lParam;
                HTREEITEM selected = nmtv->itemNew.hItem;

                auto it = treeItemPaths.find(selected);
                if (it != treeItemPaths.end()) {
                    const std::string& path = it->second;

                    // Flip forward-slashes just in case
                    std::string diskPath = path;
                    for (char& c : diskPath) if (c == '/') c = '\\';

                    std::ifstream f(diskPath, std::ios::binary);
                    if (f.is_open()) {
                        std::string content((std::istreambuf_iterator<char>(f)),
                                             std::istreambuf_iterator<char>());
                        f.close();

                        // Load into editor
                        SendMessageA(hwndEditor, 2171, 0, 0); // SCI_SETREADONLY false
                        SendMessageA(hwndEditor, 2181, 0, (LPARAM)content.c_str()); // SCI_SETTEXT
                        SendMessageA(hwndEditor, 2025, 0, 0); // SCI_GOTOPOS 0

                        // Update title bar with filename
                        size_t slash = diskPath.find_last_of("\\/");
                        std::string fname = (slash != std::string::npos) ? diskPath.substr(slash + 1) : diskPath;
                        std::string title = fname + " - Axi IDE {FOSS Edition}";
                        SetWindowTextA(hwnd, title.c_str());
                        AppendToTerminal("[EDITOR] Opened: " + diskPath);
                    } else {
                        AppendToTerminal("[EDITOR] Cannot open: " + path);
                    }
                }
            }
            break;
        }
        case WM_CREATE: {
            hwndMain = hwnd;
            
            // Enable Dark Mode Title Bar
            BOOL dark = FALSE;
            DwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark)); // DWMWA_USE_IMMERSIVE_DARK_MODE = 20
            
            hwndTree = CreateWindowExA(0, WC_TREEVIEWA, "", WS_VISIBLE | WS_CHILD | TVS_HASBUTTONS | TVS_SHOWSELALWAYS, 0, 0, 0, 0, hwnd, NULL, hInst, NULL);
            SetWindowTheme(hwndTree, L"Explorer", NULL);
            SendMessageA(hwndTree, TVM_SETBKCOLOR, 0, RGB(236, 231, 217));
            SendMessageA(hwndTree, TVM_SETTEXTCOLOR, 0, RGB(74, 74, 74));

            hwndEditor = CreateWindowExA(0, "Scintilla", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, hInst, NULL);
            SendMessageA(hwndEditor, 2052, 32, 0xD9E7EC); // #ECE7D9 bg
            SendMessageA(hwndEditor, 2051, 32, 0x4A4A4A); // text
            SendMessageA(hwndEditor, 2050, 0, 0); 
            SendMessageA(hwndEditor, 2242, 0, 40); 
            SendMessageA(hwndEditor, 2052, 33, 0xC9D9DF); // #DFD9C9 margin bg
            SendMessageA(hwndEditor, 2051, 33, 0x888888); // margin fg
            SendMessageA(hwndEditor, 2242, 1, 0);
            SendMessageA(hwndEditor, 2069, 0x5A4AB0, 0); // #B04A5A caret
            SendMessageA(hwndEditor, 2130, 0, 0); // SCI_SETHSCROLLBAR false

            hwndTerminal = CreateWindowExA(0, "Scintilla", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, hInst, NULL);
            SendMessageA(hwndTerminal, 2052, 32, 0xD9E7EC); 
            SendMessageA(hwndTerminal, 2051, 32, 0xB4886B); // #6B88B4
            SendMessageA(hwndTerminal, 2050, 0, 0); 
            SendMessageA(hwndTerminal, 2242, 0, 0); 
            SendMessageA(hwndTerminal, 2242, 1, 0); 
            SendMessageA(hwndTerminal, 2069, 0x5A4AB0, 0); 
            SendMessageA(hwndTerminal, 2130, 0, 0); // SCI_SETHSCROLLBAR false
            SendMessageA(hwndTerminal, 2268, 1, 0); // SCI_SETWRAPMODE word
            
            hwndInput = CreateWindowExA(WS_EX_CLIENTEDGE, "Scintilla", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 0, 0, 0, 0, hwnd, NULL, hInst, NULL);
            HFONT hFont = CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Consolas");
            SendMessageA(hwndInput, WM_SETFONT, (WPARAM)hFont, TRUE);
            SetWindowSubclass(hwndInput, InputProc, 1, 0);
            
            LoadFileSystemTree();
            SetTimer(hwnd, 1, 500, NULL);
            PrintSystemStats();
            return 0;
        }
        case WM_TIMER: {
            if (psycheMode) {
                std::string outPath = stateDir + "\\\\chat_output.json";
                std::ifstream file(outPath);
                if (file.is_open()) {
                    std::string line, text;
                    while (std::getline(file, line)) {
                        size_t pos = line.find("\"text\":");
                        if (pos != std::string::npos) {
                            size_t start = line.find("\"", pos + 7);
                            size_t end = line.find("\"", start + 1);
                            if (start != std::string::npos && end != std::string::npos) text = line.substr(start + 1, end - start - 1);
                        }
                    }
                    file.close();
                    DeleteFileA(outPath.c_str());
                    if (!text.empty()) AppendToTerminal("[PSYCHE] " + text);
                }
            }
            return 0;
        }
        case WM_APP_TERMINAL_OUTPUT: {
            std::lock_guard<std::mutex> lock(terminalMutex);
            for (const auto& line : terminalQueue) AppendToTerminal(line);
            terminalQueue.clear();
            return 0;
        }
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLOREDIT: {
            if ((HWND)lParam == hwndInput) {
                HDC hdc = (HDC)wParam;
                SetTextColor(hdc, RGB(166, 123, 102));
                SetBkColor(hdc, RGB(236, 231, 217));
                SetBkMode(hdc, OPAQUE);
                return (LRESULT)hDarkBrush;
            }
            break;
        }
        case WM_SIZE: {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            if (hwndTree && hwndEditor && hwndTerminal && hwndInput) {
                int rightPaneWidth = width - LEFT_PANE_WIDTH - SPLITTER_SIZE;
                int topPaneHeight = height - BOTTOM_PANE_HEIGHT - SPLITTER_SIZE - INPUT_HEIGHT;

                // Batch all 4 moves into one atomic screen update — eliminates flicker
                HDWP hdwp = BeginDeferWindowPos(4);
                hdwp = DeferWindowPos(hdwp, hwndTree,     NULL, 0,                                0,                              LEFT_PANE_WIDTH, height,             SWP_NOZORDER | SWP_NOACTIVATE);
                hdwp = DeferWindowPos(hdwp, hwndEditor,   NULL, LEFT_PANE_WIDTH + SPLITTER_SIZE,  0,                              rightPaneWidth,  topPaneHeight,      SWP_NOZORDER | SWP_NOACTIVATE);
                hdwp = DeferWindowPos(hdwp, hwndTerminal, NULL, LEFT_PANE_WIDTH + SPLITTER_SIZE,  topPaneHeight + SPLITTER_SIZE,  rightPaneWidth,  BOTTOM_PANE_HEIGHT, SWP_NOZORDER | SWP_NOACTIVATE);
                hdwp = DeferWindowPos(hdwp, hwndInput,    NULL, LEFT_PANE_WIDTH + SPLITTER_SIZE,  height - INPUT_HEIGHT,          rightPaneWidth,  INPUT_HEIGHT,       SWP_NOZORDER | SWP_NOACTIVATE);
                EndDeferWindowPos(hdwp);
            }
            return 0;
        }
        case WM_SETCURSOR: {
            POINT pt; GetCursorPos(&pt); ScreenToClient(hwnd, &pt);
            RECT rc; GetClientRect(hwnd, &rc);
            int topPaneHeight = rc.bottom - BOTTOM_PANE_HEIGHT - SPLITTER_SIZE - INPUT_HEIGHT;
            if (pt.x >= LEFT_PANE_WIDTH && pt.x <= LEFT_PANE_WIDTH + SPLITTER_SIZE) { SetCursor(LoadCursor(NULL, IDC_SIZEWE)); return TRUE; }
            if (pt.x > LEFT_PANE_WIDTH && pt.y >= topPaneHeight && pt.y <= topPaneHeight + SPLITTER_SIZE) { SetCursor(LoadCursor(NULL, IDC_SIZENS)); return TRUE; }
            break;
        }
        case WM_LBUTTONDOWN: {
            POINT pt; pt.x = (short)LOWORD(lParam); pt.y = (short)HIWORD(lParam);
            RECT rc; GetClientRect(hwnd, &rc);
            int topPaneHeight = rc.bottom - BOTTOM_PANE_HEIGHT - SPLITTER_SIZE - INPUT_HEIGHT;

            if (pt.x >= LEFT_PANE_WIDTH && pt.x <= LEFT_PANE_WIDTH + SPLITTER_SIZE) {
                isDraggingVert = true;
                SetCapture(hwnd);
            } else if (pt.x > LEFT_PANE_WIDTH && pt.y >= topPaneHeight && pt.y <= topPaneHeight + SPLITTER_SIZE) {
                isDraggingHorz = true;
                SetCapture(hwnd);
            }
            break;
        }
        case WM_MOUSEMOVE: {
            if (isDraggingVert || isDraggingHorz) {
                RECT rc; GetClientRect(hwnd, &rc);
                int x = (short)LOWORD(lParam), y = (short)HIWORD(lParam);
                int W = rc.right, H = rc.bottom;

                if (isDraggingVert)
                    LEFT_PANE_WIDTH = std::max(50, std::min(x, W - 100));
                if (isDraggingHorz)
                    BOTTOM_PANE_HEIGHT = std::max(50, std::min(H - y - INPUT_HEIGHT - SPLITTER_SIZE, H - 100));

                int rpw = W - LEFT_PANE_WIDTH - SPLITTER_SIZE;
                int tph = H - BOTTOM_PANE_HEIGHT - SPLITTER_SIZE - INPUT_HEIGHT;

                // Atomic batch — all four children move in one compositor pass
                HDWP hdwp = BeginDeferWindowPos(4);
                hdwp = DeferWindowPos(hdwp, hwndTree,     NULL, 0,                               0,                         LEFT_PANE_WIDTH, H,                  SWP_NOZORDER | SWP_NOACTIVATE);
                hdwp = DeferWindowPos(hdwp, hwndEditor,   NULL, LEFT_PANE_WIDTH + SPLITTER_SIZE, 0,                         rpw,             tph,                 SWP_NOZORDER | SWP_NOACTIVATE);
                hdwp = DeferWindowPos(hdwp, hwndTerminal, NULL, LEFT_PANE_WIDTH + SPLITTER_SIZE, tph + SPLITTER_SIZE,       rpw,             BOTTOM_PANE_HEIGHT,  SWP_NOZORDER | SWP_NOACTIVATE);
                hdwp = DeferWindowPos(hdwp, hwndInput,    NULL, LEFT_PANE_WIDTH + SPLITTER_SIZE, H - INPUT_HEIGHT,          rpw,             INPUT_HEIGHT,        SWP_NOZORDER | SWP_NOACTIVATE);
                EndDeferWindowPos(hdwp);
            }
            break;
        }
        case WM_LBUTTONUP: {
            isDraggingVert = false;
            isDraggingHorz = false;
            trackerPos = -1;
            ReleaseCapture();
            break;
        }
        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam; RECT rc; GetClientRect(hwnd, &rc); FillRect(hdc, &rc, hDarkBrush); return 1;
        }
        case WM_DESTROY: {
            PostQuitMessage(0); return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;
    
    // Load user workspace configuration
    LoadWorkspaceConfig();

    hDarkBrush = CreateSolidBrush(RGB(223, 217, 201));
    CreateDirectoryA(GetConfigDir().c_str(), NULL);
    if (!stateDir.empty()) CreateDirectoryA(stateDir.c_str(), NULL);
    INITCOMMONCONTROLSEX icex = { sizeof(INITCOMMONCONTROLSEX), ICC_TREEVIEW_CLASSES }; InitCommonControlsEx(&icex);
    HMODULE hScintilla = LoadLibraryA("Scintilla.dll");
    
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "SpatialStudioClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = hDarkBrush;
    RegisterClassA(&wc);

    // Size to 85% of primary monitor, centered
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int winW = (int)(screenW * 0.85);
    int winH = (int)(screenH * 0.85);
    int winX = (screenW - winW) / 2;
    int winY = (screenH - winH) / 2;

    HWND hwnd = CreateWindowExA(0, "SpatialStudioClass", "Axi IDE {FOSS Edition}",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE,
        winX, winY, winW, winH, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, SW_SHOWNORMAL);

    // Force layout recalculation after window is visible
    RECT rc; GetClientRect(hwnd, &rc);
    SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.right, rc.bottom));
    
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    DeleteObject(hDarkBrush);
    return 0;
}

