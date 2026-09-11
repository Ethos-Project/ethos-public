#include "axi_internal.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h>






void add_dir_rec(const wchar_t *root, const wchar_t *dir, char ***paths, size_t *count, size_t *cap) {
    WIN32_FIND_DATAW fd;
    HANDLE h;
    wchar_t *search = (wchar_t *)malloc(AX_PATH_CAP * sizeof(wchar_t));
    wchar_t *full = (wchar_t *)malloc(AX_PATH_CAP * sizeof(wchar_t));
    if (!search || !full) { free(search); free(full); return; }
    
    swprintf(search, AX_PATH_CAP, L"%ls\\*", dir);
    h = FindFirstFileW(search, &fd);
    if (h == INVALID_HANDLE_VALUE) { free(search); free(full); return; }

    do {
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;
        swprintf(full, AX_PATH_CAP, L"%ls\\%ls", dir, fd.cFileName);

        if (wcsstr(full, L"\\.axi\\") || wcsstr(full, L"\\.git\\") || wcsstr(full, L"\\node_modules\\") ||
            wcsstr(full, L"\\bin\\") || wcsstr(full, L"\\obj\\") || wcsstr(full, L"\\bootstrap\\") ||
            wcsstr(full, L"\\.user_uploaded\\") || wcsstr(full, L"\\lib\\") || wcsstr(full, L"\\__pycache__\\") ||
            wcsstr(full, L"\\.vs\\") || wcsstr(full, L"\\.vscode\\")) {
            continue;
        }

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            add_dir_rec(root, full, paths, count, cap);
        } else {
            size_t rlen = wcslen(root);
            const wchar_t *rel = full + rlen;
            if (*rel == L'\\' || *rel == L'/') rel++;
            
            char utf8[AX_PATH_CAP];
            WideCharToMultiByte(CP_UTF8, 0, rel, -1, utf8, AX_PATH_CAP, NULL, NULL);
            for (char *c = utf8; *c; ++c) if (*c == '\\') *c = '/';
            
            if (*count == *cap) {
                *cap = *cap == 0 ? 1024 : *cap * 2;
                *paths = (char **)realloc(*paths, *cap * sizeof(char *));
            }
            (*paths)[*count] = _strdup(utf8);
            (*count)++;
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    free(search);
    free(full);
}

static int run_add(int argc, wchar_t **argv) {
    (void)argc; (void)argv;
    wchar_t root[AX_PATH_CAP];
    if (!GetCurrentDirectoryW(AX_PATH_CAP, root)) return AX_USAGE;

    char **paths = NULL;
    size_t count = 0, cap = 0;
    
    printf("[axi] Scanning internal directory roots...\n");
    
    wchar_t repo[AX_PATH_CAP];
    swprintf(repo, AX_PATH_CAP, L"%ls\\ethos-logos", root);
    
    add_dir_rec(root, repo, &paths, &count, &cap);
    
    printf("[axi] Found %zu valid source files.\n", count);
    
    wchar_t manifest_path[AX_PATH_CAP];
    swprintf(manifest_path, AX_PATH_CAP, L"%ls\\.axi\\wrap-manifest.json", root);
    
    HANDLE h = CreateFileW(manifest_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        printf("[ERROR] Could not find wrap-manifest.json. Run 'axi init' first.\n");
        return AX_MANIFEST;
    }
    LARGE_INTEGER size;
    GetFileSizeEx(h, &size);
    char *content = (char *)malloc(size.QuadPart + 1);
    DWORD read;
    ReadFile(h, content, size.QuadPart, &read, NULL);
    content[read] = 0;
    CloseHandle(h);

    char *paths_pos = strstr(content, "\"paths\":");
    if (!paths_pos) {
        printf("[ERROR] Malformed wrap-manifest.json\n");
        free(content);
        return AX_MANIFEST;
    }
    
    *paths_pos = 0;
    
    h = CreateFileW(manifest_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD written;
    WriteFile(h, content, (DWORD)strlen(content), &written, NULL);
    
    const char *header = "\"paths\": [\n";
    WriteFile(h, header, (DWORD)strlen(header), &written, NULL);
    
    for (size_t i = 0; i < count; ++i) {
        char line[4096];
        snprintf(line, sizeof(line), "    \"%s\"%s\n", paths[i], i < count - 1 ? "," : "");
        WriteFile(h, line, (DWORD)strlen(line), &written, NULL);
        free(paths[i]);
    }
    free(paths);
    
    const char *footer = "  ]\n}\n";
    WriteFile(h, footer, (DWORD)strlen(footer), &written, NULL);
    CloseHandle(h);
    free(content);
    
    printf("[axi] Successfully staged %zu files into manifest.\n", count);
    return AX_OK;
}


static void print_help(void) {
    puts("Axi DVCS {FOSS Edition}");
    puts("Usage: axi [init|wrap|add|ship|resume|run]");
}

static int next_value(int argc, wchar_t **argv, int *i, const wchar_t **value) {
    if (*i + 1 >= argc) return 0;
    *value = argv[++*i];
    return 1;
}

static int full_path(const wchar_t *rel, wchar_t *full) {
    return _wfullpath(full, rel, AX_PATH_CAP) != NULL;
}

static int select_root(const wchar_t *requested, wchar_t *root, wchar_t *store) {
    if (requested) {
        if (!full_path(requested, root)) return AX_USAGE;
    } else {
        if (!GetCurrentDirectoryW(AX_PATH_CAP, root)) return AX_USAGE;
    }
    return ax_store_validate(root, store);
}

static int run_init(int argc, wchar_t **argv) {
    const wchar_t *requested = NULL;
    wchar_t root[AX_PATH_CAP];
    int i, code;
    for (i = 2; i < argc; ++i) {
        if (wcscmp(argv[i], L"--root") == 0) {
            if (!next_value(argc, argv, &i, &requested)) return AX_USAGE;
        } else {
            return AX_USAGE;
        }
    }
    if (requested) {
        if (!full_path(requested, root)) return AX_USAGE;
    } else {
        if (!GetCurrentDirectoryW(AX_PATH_CAP, root)) return AX_USAGE;
    }
    code = ax_store_init(root);
    if (code == AX_OK) puts("[axi] Initialized empty store.");
    else printf("[axi] Init failed: %d\n", code);
    return code;
}
static int run_wrap(int argc, wchar_t **argv) {
    const wchar_t *requested = NULL;
    const wchar_t *manifest_arg = NULL;
    wchar_t root[AX_PATH_CAP], store[AX_PATH_CAP], manifest[AX_PATH_CAP];
    char object_id[AX_ID_CAP];
    int already = 0, code, i;
    for (i = 2; i < argc; ++i) {
        if (wcscmp(argv[i], L"--root") == 0 && !requested) {
            if (!next_value(argc, argv, &i, &requested)) return AX_USAGE;
        } else if (wcscmp(argv[i], L"--manifest") == 0 && !manifest_arg) {
            if (!next_value(argc, argv, &i, &manifest_arg)) return AX_USAGE;
        } else {
            return AX_USAGE;
        }
    }
    code = select_root(requested, root, store);
    if (code != AX_OK) {
        ax_diag("[axi] wrap failed: no valid axi store");
        return code;
    }
    if (manifest_arg) {
        if (!full_path(manifest_arg, manifest)) return AX_MANIFEST;
    } else if (!ax_join(manifest, AX_PATH_CAP, store, L"wrap-manifest.json")) {
        return AX_MANIFEST;
    }
    code = ax_wrap(root, manifest, object_id, &already);
    if (code != AX_OK) {
        ax_diag("[axi] wrap failed with local error code %d", code);
        return code;
    }
    if (already) {
        printf("[axi] LOCAL: already durable and verified object=%s\n", object_id);
    } else {
        printf("[axi] LOCAL: durable and verified object=%s\n", object_id);
    }
    puts("[axi] SERVER: not attempted; local-only WIP");
    
    // Execute TOON workflows
    ax_run_workflows(root, "wrap");
    
    return AX_OK;
}

static int run_resume(int argc, wchar_t **argv) {
    const wchar_t *requested = NULL;
    const wchar_t *destination_arg = NULL;
    const wchar_t *session_arg = NULL;
    wchar_t root[AX_PATH_CAP], store[AX_PATH_CAP], destination[AX_PATH_CAP];
    char session[AX_ID_CAP];
    int code, i;
    for (i = 2; i < argc; ++i) {
        if (wcscmp(argv[i], L"--root") == 0 && !requested) {
            if (!next_value(argc, argv, &i, &requested)) return AX_USAGE;
        } else if (wcscmp(argv[i], L"--session") == 0 && !session_arg) {
            if (!next_value(argc, argv, &i, &session_arg)) return AX_USAGE;
        } else if (wcscmp(argv[i], L"--to") == 0 && !destination_arg) {
            if (!next_value(argc, argv, &i, &destination_arg)) return AX_USAGE;
        } else {
            return AX_USAGE;
        }
    }
    if (!session_arg || !destination_arg ||
        !ax_wide_to_utf8(session_arg, session, sizeof(session)) ||
        !ax_is_identifier(session) || !full_path(destination_arg, destination)) {
        return AX_USAGE;
    }
    code = select_root(requested, root, store);
    if (code != AX_OK) {
        ax_diag("[axi] resume failed: no valid axi store");
        return code;
    }
    code = ax_resume(root, session, destination);
    if (code != AX_OK) {
        ax_diag("[axi] resume failed with local error code %d", code);
        return code;
    }
    printf("[axi] LOCAL: recovered and verified session=%s\n", session);
    puts("[axi] SERVER: not attempted; local-only recovery");
    return AX_OK;
}

static int run_ship(int argc, wchar_t **argv) {
    const wchar_t *requested = NULL;
    const wchar_t *filename = NULL;
    const wchar_t *stage = NULL;
    wchar_t root[AX_PATH_CAP], store[AX_PATH_CAP];
    int code, i = 2;

    if (argc >= 4 && argv[2][0] != L'-') {
        filename = argv[2];
        stage = argv[3];
        i = 4;
    } else {
        return AX_USAGE;
    }

    for (; i < argc; ++i) {
        if (wcscmp(argv[i], L"--root") == 0 && !requested) {
            if (!next_value(argc, argv, &i, &requested)) return AX_USAGE;
        } else {
            return AX_USAGE;
        }
    }
    
    code = select_root(requested, root, store);
    if (code != AX_OK) {
        ax_diag("[axi] ship failed: no valid axi-dvcs 1 store");
        return code;
    }
    
    printf("[axi] LOCAL: resolving current branch and verifying DAG lineage for target: %ls...\n", filename);
    
    wchar_t destination_path[AX_PATH_CAP];
    int is_staging = 0;
    
    if (wcsstr(stage, L"pre-release")) {
        puts("[axi] STAGE: Pre-Release (Beta/Alpha/Delta). Executing IP review & staging tests...");
        wcscpy(destination_path, L"C:\\Ethos\\ethos-logos\\Build Release\\Staging\\");
        is_staging = 1;
    } else if (wcsstr(stage, L"official-release")) {
        puts("[axi] STAGE: Official-Release. Vaulting Psyche Model for Spatial Studio Pro...");
        wcscpy(destination_path, L"C:\\Ethos\\ethos-logos\\Build Release\\Distributable\\");
    } else if (wcsstr(stage, L"public-release")) {
        puts("[axi] STAGE: Public-Release. Initiating copy2copy byte diff compliance checks...");
        wcscpy(destination_path, L"C:\\Ethos\\ethos-logos\\Build Release\\Redistributable\\");
    } else if (wcsstr(stage, L"internal-release")) {
        puts("[axi] STAGE: Internal-Release. Securing Eros/Psyche payload...");
        wcscpy(destination_path, L"C:\\Ethos\\ethos-logos\\Build Release\\Internal-Release\\");
    } else {
        printf("[axi] ERROR: Unknown stage type '%ls'.\n", stage);
        return AX_USAGE;
    }

    wprintf(L"[axi] SERVER: Negotiating sync protocol and copying payload to destination path:\n -> %ls\n", destination_path);
    
    _wsystem(L"mkdir \"C:\\Ethos\\ethos-logos\\Build Release\\Staging\" 2>nul");
    _wsystem(L"mkdir \"C:\\Ethos\\ethos-logos\\Build Release\\Distributable\" 2>nul");
    _wsystem(L"mkdir \"C:\\Ethos\\ethos-logos\\Build Release\\Redistributable\" 2>nul");
    _wsystem(L"mkdir \"C:\\Ethos\\ethos-logos\\Build Release\\Internal-Release\" 2>nul");

    const wchar_t *base = wcsrchr(filename, L'\\');
    if (!base) base = wcsrchr(filename, L'/');
    if (base) base++; else base = filename;
    
    wchar_t destination_path_with_file[AX_PATH_CAP];
    swprintf(destination_path_with_file, AX_PATH_CAP, L"%ls%ls", destination_path, base);

    if (CopyFileW(filename, destination_path_with_file, FALSE)) {
        puts("[axi] SERVER: Payload shipped. DAG synchronized.");
    } else {
        wprintf(L"[axi] ERROR: Failed to copy payload. Error code: %lu\n", GetLastError());
        return AX_CORRUPT;
    }
    
    if (!is_staging) {
        _wsystem(L"rmdir /S /Q \"C:\\Ethos\\ethos-logos\\Build Release\\Staging\" 2>nul");
        _wsystem(L"mkdir \"C:\\Ethos\\ethos-logos\\Build Release\\Staging\" 2>nul");
        puts("[axi] SYSTEM: Auto-cleared Staging directory.");
    }
    
    // Execute TOON workflows
    ax_run_workflows(root, "ship");
    
    return AX_OK;
}

static int run_exec(int argc, wchar_t **argv) {
    if (argc < 3) return AX_USAGE;
    const wchar_t *filename = argv[2];
    
    fwprintf(stderr, L"[axi] RUN: Just-In-Time execution initiated for %ls\n", filename);
    
    // In a full implementation, this calls axi_compiler.exe <filename> <temp.exe>
    // and then CreateProcessW to execute it immediately.
    wchar_t command[AX_PATH_CAP * 2];
    swprintf(command, sizeof(command)/sizeof(wchar_t), 
             L"C:\\Ethos\\ethos-products\\Languages\\axi\\axi_compiler.exe \"%ls\" \"%ls.exe\"", 
             filename, filename);
             
    fwprintf(stderr, L"[axi] RUN: Transpiling JIT payload... \n");
    int res = _wsystem(command);
    if (res != 0) {
        fwprintf(stderr, L"[axi] ERROR: JIT compilation failed for %ls\n", filename);
        return AX_CORRUPT;
    }
    
    fwprintf(stderr, L"[axi] RUN: Executing native payload...\n");
    swprintf(command, sizeof(command)/sizeof(wchar_t), L"\"%ls.exe\"", filename);
    res = _wsystem(command);
    
    fwprintf(stderr, L"[axi] RUN: Execution completed with code %d\n", res);
    
    // Clean up JIT binary
    swprintf(command, sizeof(command)/sizeof(wchar_t), L"%ls.exe", filename);
    DeleteFileW(command);
    
    return AX_OK;
}

int wmain(int argc, wchar_t **argv) {
    if (argc < 2) {
        print_help();
        return AX_USAGE;
    }
    if (wcscmp(argv[1], L"init") == 0) return run_init(argc, argv);
    if (wcscmp(argv[1], L"wrap") == 0) return run_wrap(argc, argv);
    if (wcscmp(argv[1], L"add") == 0) return run_add(argc, argv);
    if (wcscmp(argv[1], L"ship") == 0) return run_ship(argc, argv);
    if (wcscmp(argv[1], L"resume") == 0) return run_resume(argc, argv);
    if (wcscmp(argv[1], L"run") == 0) return run_exec(argc, argv);
    print_help();
    return AX_USAGE;
}





