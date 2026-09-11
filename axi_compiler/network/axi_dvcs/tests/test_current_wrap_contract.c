#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

enum {
    PATH_CAPACITY = 4096,
    SESSION_REF_SIZE = 65
};

static int join_path(char *destination, size_t capacity, const char *left, const char *right) {
    int written = snprintf(destination, capacity, "%s\\%s", left, right);
    return written >= 0 && (size_t)written < capacity;
}

static int create_directory(const char *path) {
    if (CreateDirectoryA(path, NULL) != 0) {
        return 1;
    }
    return GetLastError() == ERROR_ALREADY_EXISTS;
}

static int write_text_file(const char *path, const char *contents) {
    HANDLE file = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD expected = (DWORD)strlen(contents);
    DWORD written = 0;
    int success;

    if (file == INVALID_HANDLE_VALUE) {
        return 0;
    }
    success = WriteFile(file, contents, expected, &written, NULL) != 0 && written == expected;
    if (CloseHandle(file) == 0) {
        success = 0;
    }
    return success;
}

static int open_directory_for_cleanup(const char *path, HANDLE *directory, int *is_reparse_point) {
    FILE_ATTRIBUTE_TAG_INFO attributes;

    *directory = CreateFileA(path, DELETE | FILE_LIST_DIRECTORY | FILE_READ_ATTRIBUTES,
                             FILE_SHARE_READ, NULL, OPEN_EXISTING,
                             FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (*directory == INVALID_HANDLE_VALUE) {
        return 0;
    }
    if (GetFileInformationByHandleEx(*directory, FileAttributeTagInfo, &attributes,
                                     sizeof(attributes)) == 0 ||
        (attributes.FileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
        CloseHandle(*directory);
        *directory = INVALID_HANDLE_VALUE;
        return 0;
    }
    *is_reparse_point = (attributes.FileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
    return 1;
}

static int delete_open_directory(HANDLE directory) {
    FILE_DISPOSITION_INFO disposition;
    int success;

    disposition.DeleteFile = TRUE;
    success = SetFileInformationByHandle(directory, FileDispositionInfo, &disposition,
                                         sizeof(disposition)) != 0;
    if (CloseHandle(directory) == 0) {
        success = 0;
    }
    return success;
}

static int remove_tree(const char *path) {
    WIN32_FIND_DATAA entry;
    HANDLE directory;
    HANDLE search;
    char pattern[PATH_CAPACITY];
    int is_reparse_point;
    int success = 1;

    if (!open_directory_for_cleanup(path, &directory, &is_reparse_point)) {
        return 0;
    }
    if (is_reparse_point) {
        return delete_open_directory(directory);
    }
    if (!join_path(pattern, sizeof(pattern), path, "*")) {
        CloseHandle(directory);
        return 0;
    }
    search = FindFirstFileA(pattern, &entry);
    if (search == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            return delete_open_directory(directory);
        }
        CloseHandle(directory);
        return 0;
    }

    do {
        char child[PATH_CAPACITY];
        if (strcmp(entry.cFileName, ".") == 0 || strcmp(entry.cFileName, "..") == 0) {
            continue;
        }
        if (!join_path(child, sizeof(child), path, entry.cFileName)) {
            success = 0;
            continue;
        }
        if ((entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            if (!remove_tree(child)) {
                success = 0;
            }
        } else {
            SetFileAttributesA(child, FILE_ATTRIBUTE_NORMAL);
            if (DeleteFileA(child) == 0) {
                success = 0;
            }
        }
    } while (FindNextFileA(search, &entry) != 0);

    FindClose(search);
    if (!success) {
        CloseHandle(directory);
        return 0;
    }
    return delete_open_directory(directory);
}

static int prepare_fixture(char *root, size_t root_capacity) {
    char path[PATH_CAPACITY];
    char axi_root[PATH_CAPACITY];
    char suffix[128];
    DWORD attempt;
    const char *manifest =
        "Manifest {\n"
        "  format: axi-wrap-manifest-v1\n"
        "  session_id: red-baseline-session\n"
        "  issue_ids: [\"issue-red\"]\n"
        "  context_ids: [\"context-red\"]\n"
        "  paths: [\"payload.txt\"]\n"
        "}\n";

    char temporary_path[PATH_CAPACITY];

    root[0] = '\0';
    if (GetTempPathA((DWORD)sizeof(temporary_path), temporary_path) == 0) {
        return 0;
    }
    for (attempt = 0; attempt < 100; ++attempt) {
        int written = snprintf(suffix, sizeof(suffix), "axi-wrap-red-%lu-%lu-%lu",
                               (unsigned long)GetCurrentProcessId(),
                               (unsigned long)GetTickCount(),
                               (unsigned long)attempt);
        if (written < 0 || (size_t)written >= sizeof(suffix) ||
            !join_path(path, sizeof(path), temporary_path, suffix)) {
            return 0;
        }
        if (CreateDirectoryA(path, NULL) != 0) {
            if ((size_t)snprintf(root, root_capacity, "%s", path) >= root_capacity) {
                root[0] = '\0';
                return 0;
            }
            break;
        }
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            return 0;
        }
    }
    if (attempt == 100) {
        return 0;
    }

    if (!join_path(axi_root, sizeof(axi_root), root, ".axi") ||
        !create_directory(axi_root)) {
        return 0;
    }
    if (!join_path(path, sizeof(path), axi_root, "FORMAT") ||
        !write_text_file(path, "axi-dvcs 1\n")) {
        return 0;
    }
    if (!join_path(path, sizeof(path), axi_root, "objects") || !create_directory(path) ||
        !join_path(path, sizeof(path), axi_root, "refs") || !create_directory(path) ||
        !join_path(path, sizeof(path), axi_root, "refs\\wip") || !create_directory(path) ||
        !join_path(path, sizeof(path), axi_root, "refs\\previous") || !create_directory(path) ||
        !join_path(path, sizeof(path), axi_root, "tmp") || !create_directory(path)) {
        return 0;
    }
    if (!join_path(path, sizeof(path), axi_root, "wrap-manifest.toon") ||
        !write_text_file(path, manifest) ||
        !join_path(path, sizeof(path), root, "payload.txt") ||
        !write_text_file(path, "red baseline payload\n")) {
        return 0;
    }
    return 1;
}

static int run_wrap(const char *axi_executable, const char *root, DWORD *exit_code) {
    STARTUPINFOA startup;
    PROCESS_INFORMATION process;
    char command_line[PATH_CAPACITY * 2];
    DWORD wait_result;
    int written = snprintf(command_line, sizeof(command_line),
                           "\"%s\" wrap --root \"%s\"", axi_executable, root);

    if (written < 0 || (size_t)written >= sizeof(command_line)) {
        return 0;
    }
    ZeroMemory(&startup, sizeof(startup));
    ZeroMemory(&process, sizeof(process));
    startup.cb = sizeof(startup);
    if (CreateProcessA(NULL, command_line, NULL, NULL, FALSE, 0, NULL, root,
                       &startup, &process) == 0) {
        return 0;
    }
    wait_result = WaitForSingleObject(process.hProcess, INFINITE);
    if (wait_result != WAIT_OBJECT_0 || GetExitCodeProcess(process.hProcess, exit_code) == 0) {
        CloseHandle(process.hThread);
        CloseHandle(process.hProcess);
        return 0;
    }
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    return 1;
}

static int is_lower_hex_ref(const char *value) {
    size_t index;
    for (index = 0; index < 64; ++index) {
        if (!((value[index] >= '0' && value[index] <= '9') ||
              (value[index] >= 'a' && value[index] <= 'f'))) {
            return 0;
        }
    }
    return value[64] == '\n';
}

static int verify_durable_state(const char *root, char *failure, size_t failure_capacity) {
    char ref_path[PATH_CAPACITY];
    char object_path[PATH_CAPACITY];
    char ref[SESSION_REF_SIZE];
    HANDLE file;
    LARGE_INTEGER size;
    DWORD read = 0;
    DWORD attributes;
    int object_path_length;

    if (!join_path(ref_path, sizeof(ref_path), root, ".axi\\refs\\wip\\red-baseline-session")) {
        return 0;
    }
    attributes = GetFileAttributesA(ref_path);
    if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
        snprintf(failure, failure_capacity,
                 "FAIL: axi wrap exited 0 but did not create durable session ref '%s'. Console output is not evidence of durability.",
                 ref_path);
        return 0;
    }
    file = CreateFileA(ref_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE || GetFileSizeEx(file, &size) == 0 ||
        size.QuadPart != SESSION_REF_SIZE || ReadFile(file, ref, sizeof(ref), &read, NULL) == 0 ||
        read != sizeof(ref)) {
        if (file != INVALID_HANDLE_VALUE) {
            CloseHandle(file);
        }
        snprintf(failure, failure_capacity,
                 "FAIL: axi wrap exited 0 but session ref '%s' is not exactly a lowercase 64-hex object ID plus newline.",
                 ref_path);
        return 0;
    }
    CloseHandle(file);
    if (!is_lower_hex_ref(ref)) {
        snprintf(failure, failure_capacity,
                 "FAIL: axi wrap exited 0 but session ref '%s' is not exactly a lowercase 64-hex object ID plus newline.",
                 ref_path);
        return 0;
    }
    ref[64] = '\0';
    object_path_length = snprintf(object_path, sizeof(object_path), "%s\\.axi\\objects\\%.2s\\%s", root, ref, ref + 2);
    if (object_path_length < 0 || (size_t)object_path_length >= sizeof(object_path)) {
        return 0;
    }
    attributes = GetFileAttributesA(object_path);
    if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
        snprintf(failure, failure_capacity,
                 "FAIL: axi wrap exited 0 but session ref addresses no durable object '%s'.",
                 object_path);
        return 0;
    }
    return 1;
}

int main(int argc, char **argv) {
    char root[PATH_CAPACITY];
    char failure[PATH_CAPACITY + 256];
    DWORD exit_code = 0;
    int cleanup_success;
    int test_result = 1;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <absolute-path-to-axi.exe>\n", argv[0]);
        return 2;
    }
    if (!prepare_fixture(root, sizeof(root))) {
        if (root[0] != '\0' && !remove_tree(root)) {
            fprintf(stderr, "FAIL: fixture setup failed and cleanup of synthetic root '%s' also failed.\n", root);
            return 2;
        }
        fprintf(stderr, "FAIL: could not create the isolated synthetic version 1 DVCS root.\n");
        return 2;
    }
    if (!run_wrap(argv[1], root, &exit_code)) {
        snprintf(failure, sizeof(failure), "FAIL: could not launch axi wrap in isolated root '%s' (Win32 error %lu).",
                 root, (unsigned long)GetLastError());
    } else if (exit_code != 0) {
        snprintf(failure, sizeof(failure), "FAIL: axi wrap exited %lu; durable success requires exit 0 and verified ref/object state.",
                 (unsigned long)exit_code);
    } else if (verify_durable_state(root, failure, sizeof(failure))) {
        snprintf(failure, sizeof(failure), "PASS: axi wrap exited 0 and created a durable verified session ref and object.");
        test_result = 0;
    }

    cleanup_success = remove_tree(root);
    if (!cleanup_success) {
        fprintf(stderr, "FAIL: cleanup of synthetic root '%s' failed.\n", root);
        return 2;
    }
    if (test_result == 0) {
        puts(failure);
    } else {
        fprintf(stderr, "%s\n", failure);
    }
    return test_result;
}
