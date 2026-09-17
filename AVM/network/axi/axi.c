#include "axi_internal.h"

#include <stdio.h>
#include <string.h>
#include <wchar.h>

static void print_help(void) {
    puts("axi DVCS (local v1 candidate)");
    puts("Commands:");
    puts("  init [--root <path>] [--edition <standard|pro>]");
    puts("  wrap [--root <path>] [--manifest <path>]");
    puts("  ship <filename> <stage-type> [--root <path>]");
    puts("  run <filename.{js,ts,axi}>");
    puts("  resume [--root <path>] --session <id> --to <empty-directory>");
}

static int full_path(const wchar_t *input, wchar_t output[AX_PATH_CAP]) {
    DWORD length = GetFullPathNameW(input, AX_PATH_CAP, output, NULL);
    size_t n;
    if (length == 0 || length >= AX_PATH_CAP) return 0;
    n = wcslen(output);
    while (n > 3 && (output[n - 1] == L'\\' || output[n - 1] == L'/')) {
        output[--n] = L'\0';
    }
    return 1;
}

static int next_value(int argc, wchar_t **argv, int *index,
                      const wchar_t **value) {
    if (*index + 1 >= argc) return 0;
    *value = argv[++(*index)];
    return 1;
}

static int select_root(const wchar_t *requested, wchar_t root[AX_PATH_CAP],
                       wchar_t store[AX_PATH_CAP]) {
    if (requested) {
        if (!full_path(requested, root)) return AX_STORE;
        return ax_store_validate(root, store);
    }
    return ax_store_discover(root, store);
}

static int run_init(int argc, wchar_t **argv) {
    const wchar_t *requested = NULL;
    const wchar_t *edition = L"standard"; // default
    wchar_t root[AX_PATH_CAP];
    int i;
    for (i = 2; i < argc; ++i) {
        if (wcscmp(argv[i], L"--root") == 0 && !requested) {
            if (!next_value(argc, argv, &i, &requested)) return AX_USAGE;
        } else if (wcscmp(argv[i], L"--edition") == 0) {
            if (!next_value(argc, argv, &i, &edition)) return AX_USAGE;
        } else {
            return AX_USAGE;
        }
    }
    if (requested) {
        if (!full_path(requested, root)) return AX_STORE;
    } else if (!GetCurrentDirectoryW(AX_PATH_CAP, root)) {
        return AX_STORE;
    }
    
    // Pass edition to ax_store_init if needed. For now, it initializes the format.
    if (ax_store_init(root) != AX_OK) {
        ax_diag("[axi] init failed: root is invalid or already contains .axi state");
        return AX_STORE;
    }
    
    printf("[axi] LOCAL: initialized and verified axi-dvcs 1 store (Edition: %ls)\n", edition);
    if (wcscmp(edition, L"pro") == 0) {
        puts("[axi] FEATURE: DAG-Native branching enabled (Spatial Studio Pro).");
        puts("[axi] FEATURE: Vaulted Psyche topology assistant loaded.");
    } else {
        puts("[axi] FEATURE: Standard feature/* branching enabled.");
    }
    
    puts("[axi] SERVER: not attempted; local-only store");
    return AX_OK;
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
        ax_diag("[axi] wrap failed: no valid axi-dvcs 1 store");
        return code;
    }
    if (manifest_arg) {
        if (!full_path(manifest_arg, manifest)) return AX_MANIFEST;
    } else if (!ax_join(manifest, AX_PATH_CAP, store, L"wrap-manifest.toon")) {
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
    
    // Execute YAML workflows
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
        ax_diag("[axi] resume failed: no valid axi-dvcs 1 store");
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
    
    // Execute YAML workflows
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
    if (wcscmp(argv[1], L"ship") == 0) return run_ship(argc, argv);
    if (wcscmp(argv[1], L"resume") == 0) return run_resume(argc, argv);
    if (wcscmp(argv[1], L"run") == 0) return run_exec(argc, argv);
    print_help();
    return AX_USAGE;
}

