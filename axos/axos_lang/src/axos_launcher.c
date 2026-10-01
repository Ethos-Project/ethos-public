#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define AX_PATH_CAP 32768

enum { AX_OK = 0, AX_USAGE = 2, AX_INPUT = 3, AX_BACKEND = 5 };

typedef int (*AVM_Start_Func)(int argc, char** argv);

#include "bom_manifest.h"

static int regular_file(const wchar_t* path) {
    DWORD a = GetFileAttributesW(path);
    return a != INVALID_FILE_ATTRIBUTES && !(a & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT));
}

static int join_path(wchar_t out[AX_PATH_CAP], const wchar_t* left, const wchar_t* right) {
    int n = _snwprintf(out, AX_PATH_CAP, L"%ls\\%ls", left, right);
    return n >= 0 && n < AX_PATH_CAP;
}

// Dynamically resolves avm_core.dll strictly via:
// 1. Alongside axos.exe (the Allforge installed-unit pattern)
// 2. The Allforge toolchain registry/resolver (WO #33 pattern)
// 3. Fail loudly with a clear message naming the missing DLL (no hardcoded product paths, no env vars, no silent fallback).
static HMODULE load_avm_core(void) {
    wchar_t exe_dir[AX_PATH_CAP];
    wchar_t dll_path[AX_PATH_CAP];
    DWORD n = GetModuleFileNameW(NULL, exe_dir, AX_PATH_CAP);
    wchar_t* s;

    // 1. Same directory as axos.exe (the Allforge installed-unit pattern)
    if (n && n < AX_PATH_CAP && (s = wcsrchr(exe_dir, L'\\'))) {
        *s = 0;
        SetDllDirectoryW(exe_dir);
        if (join_path(dll_path, exe_dir, L"avm_core.dll") && regular_file(dll_path)) {
            return LoadLibraryW(dll_path);
        }
    }

    // 2. The Allforge toolchain registry/resolver (WO #33 pattern)
    wchar_t cur[AX_PATH_CAP];
    wcsncpy(cur, exe_dir, AX_PATH_CAP);
    for (int depth = 0; depth < 5; ++depth) {
        wchar_t reg_file[AX_PATH_CAP];
        if (join_path(reg_file, cur, L"Allforge\\toolchains\\registry.yaml") && regular_file(reg_file)) {
            FILE* f = _wfopen(reg_file, L"rt");
            if (f) {
                char line[1024];
                int in_avm = 0;
                while (fgets(line, sizeof(line), f)) {
                    if (strstr(line, "name: avm") || strstr(line, "name: \"avm\"")) {
                        in_avm = 1;
                    } else if (in_avm && strstr(line, "- name:") && !strstr(line, "avm_core") && !strstr(line, "axos")) {
                        in_avm = 0;
                    }
                    if (in_avm && strstr(line, "avm_core.dll")) {
                        char* p = strstr(line, "path:");
                        if (p) {
                            p += 5;
                            while (*p == ' ' || *p == '"' || *p == '\'') p++;
                            char path_a[MAX_PATH];
                            int len = 0;
                            while (*p && *p != '\r' && *p != '\n' && *p != '"' && *p != '\'') {
                                if (*p == '\\' && *(p + 1) == '\\') p++;
                                path_a[len++] = *p++;
                            }
                            path_a[len] = '\0';
                            wchar_t path_w[AX_PATH_CAP];
                            MultiByteToWideChar(CP_UTF8, 0, path_a, -1, path_w, AX_PATH_CAP);
                            if (regular_file(path_w)) {
                                fclose(f);
                                wchar_t dir_w[AX_PATH_CAP];
                                wcsncpy(dir_w, path_w, AX_PATH_CAP);
                                wchar_t* ds = wcsrchr(dir_w, L'\\');
                                if (ds) { *ds = 0; SetDllDirectoryW(dir_w); }
                                return LoadLibraryW(path_w);
                            }
                        }
                    }
                }
                fclose(f);
            }
            break;
        }
        wchar_t* up = wcsrchr(cur, L'\\');
        if (!up) break;
        *up = 0;
    }

    // 3. Fail loudly naming the missing DLL
    fwprintf(stderr, L"[FATAL] Failed to resolve avm_core.dll: runtime library not found alongside axos.exe or in Allforge toolchain registry.\n");
    return NULL;
}

static int run_avm(int argc, wchar_t** argv, int arg_offset) {
    HMODULE hAvm = load_avm_core();
    if (!hAvm) {
        DWORD err = GetLastError();
        fwprintf(stderr, L"[FATAL] Could not load avm_core.dll (Error Code: %lu)\n", err);
        return AX_BACKEND;
    }

    int avm_argc = argc - arg_offset;
    char** avm_argv = (char**)malloc(sizeof(char*) * (avm_argc + 1));
    if (!avm_argv) return AX_BACKEND;

    char prog_name[MAX_PATH];
    WideCharToMultiByte(CP_UTF8, 0, argv[0], -1, prog_name, sizeof(prog_name), NULL, NULL);
    avm_argv[0] = _strdup(prog_name);

    for (int i = 1; i < avm_argc; ++i) {
        char arg_buf[AX_PATH_CAP];
        WideCharToMultiByte(CP_UTF8, 0, argv[i + arg_offset], -1, arg_buf, sizeof(arg_buf), NULL, NULL);
        avm_argv[i] = _strdup(arg_buf);
    }
    avm_argv[avm_argc] = NULL;

    AVM_Start_Func avm_start = (AVM_Start_Func)GetProcAddress(hAvm, "AVM_Start");
    if (avm_start) {
        int ret = avm_start(avm_argc, avm_argv);
        for (int i = 0; i < avm_argc; ++i) free(avm_argv[i]);
        free(avm_argv);
        return ret;
    }

    for (int i = 0; i < avm_argc; ++i) free(avm_argv[i]);
    free(avm_argv);
    return AX_BACKEND;
}

static void help(void) {
    puts("Axos Language Toolchain CLI");
    puts("Usage:");
    puts("  Interactive Shell (REPL):");
    puts("    axos");
    puts("  Run mode (AVM Bytecode Engine):");
    puts("    axos <input.axos> [args...]");
    puts("    axos run <input.axos> [args...]");
    puts("    axos debug <input.axos>");
}

int wmain(int argc, wchar_t** argv) {
    if (argc >= 2 && (!_wcsicmp(argv[1], L"--version") || !_wcsicmp(argv[1], L"-v") || !_wcsicmp(argv[1], L"-V"))) {
        printf("axos=0.1.0\n");
        printf("kernel=axi\n");
        print_bom_manifest("");
        return AX_OK;
    }

    if (argc < 2) {
        return run_avm(argc, argv, 0);
    }

    if (!_wcsicmp(argv[1], L"query")) {
        if (argc < 4) { help(); return AX_USAGE; }
        return run_avm(argc, argv, 0);
    }

    if (!_wcsicmp(argv[1], L"run")) {
        if (argc < 3) { help(); return AX_USAGE; }
        return run_avm(argc, argv, 1);
    }

    if (!_wcsicmp(argv[1], L"debug")) {
        if (argc < 3) { help(); return AX_USAGE; }
        return run_avm(argc, argv, 0);
    }

    const wchar_t* ext = wcsrchr(argv[1], L'.');
    if (ext && (!_wcsicmp(ext, L".axos") || !_wcsicmp(ext, L".axi") || !_wcsicmp(ext, L".allos"))) {
        return run_avm(argc, argv, 0);
    }

    if (argc == 2 && regular_file(argv[1])) {
        return run_avm(argc, argv, 0);
    }

    /* Unknown subcommand guard */
    if (argv[1][0] != L'-' && !regular_file(argv[1]) && !wcsrchr(argv[1], L'.')) {
        fwprintf(stderr, L"axos: unknown subcommand '%ls'\n\n", argv[1]);
        help();
        return AX_USAGE;
    }

    return run_avm(argc, argv, 0);
}
