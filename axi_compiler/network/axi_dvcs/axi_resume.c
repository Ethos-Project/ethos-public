#include "axi_internal.h"

#include <string.h>

static int empty_directory(const wchar_t *path) {
    wchar_t pattern[AX_PATH_CAP];
    WIN32_FIND_DATAW entry;
    HANDLE find;
    DWORD attributes = GetFileAttributesW(path);
    if (attributes == INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY) ||
        (attributes & FILE_ATTRIBUTE_REPARSE_POINT) || !ax_join(pattern, AX_PATH_CAP, path, L"*")) return 0;
    find = FindFirstFileW(pattern, &entry);
    if (find == INVALID_HANDLE_VALUE) return GetLastError() == ERROR_FILE_NOT_FOUND;
    do {
        if (wcscmp(entry.cFileName, L".") != 0 && wcscmp(entry.cFileName, L"..") != 0) {
            FindClose(find);
            return 0;
        }
    } while (FindNextFileW(find, &entry));
    FindClose(find);
    return 1;
}

static int create_parent_directories(const wchar_t *destination, const wchar_t *relative) {
    wchar_t path[AX_PATH_CAP];
    wchar_t *cursor;
    if (!ax_join(path, AX_PATH_CAP, destination, relative)) return 0;
    for (cursor = path + wcslen(destination) + 1; *cursor; ++cursor) {
        if (*cursor == L'/') *cursor = L'\\';
        if (*cursor == L'\\') {
            *cursor = L'\0';
            if (!CreateDirectoryW(path, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) return 0;
            *cursor = L'\\';
        }
    }
    return 1;
}

int ax_resume(const wchar_t *root, const char *session, const wchar_t *destination) {
    wchar_t store[AX_PATH_CAP], object_path[AX_PATH_CAP];
    AxObject object;
    char id[AX_ID_CAP];
    int exists = 0;
    int code;
    HANDLE source = INVALID_HANDLE_VALUE;
    size_t i;

    memset(&object, 0, sizeof(object));
    if (!empty_directory(destination)) return AX_DESTINATION;
    code = ax_store_validate(root, store);
    if (code != AX_OK) goto done;
    code = ax_read_ref(store, session, id, &exists);
    if (code != AX_OK || !exists) { if (code == AX_OK) code = AX_CORRUPT; goto done; }
    code = ax_object_verify(store, id, &object);
    if (code != AX_OK || !ax_object_path(store, id, object_path)) { if (code == AX_OK) code = AX_CORRUPT; goto done; }
    source = CreateFileW(object_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (source == INVALID_HANDLE_VALUE) { code = AX_CORRUPT; goto done; }
    for (i = 0; i < object.payload_count; ++i) {
        AxPayload *payload = &object.payloads[i];
        wchar_t relative[AX_PATH_CAP], output[AX_PATH_CAP];
        LARGE_INTEGER offset;
        HANDLE target;
        uint64_t remaining = payload->size;
        unsigned char buffer[AX_IO_CHUNK];
        if (!ax_utf8_to_wide(payload->path, payload->path_length, relative, AX_PATH_CAP) ||
            !create_parent_directories(destination, relative) || !ax_join(output, AX_PATH_CAP, destination, relative)) {
            code = AX_DESTINATION; goto done;
        }
        target = CreateFileW(output, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        if (target == INVALID_HANDLE_VALUE) { code = AX_DESTINATION; goto done; }
        offset.QuadPart = (LONGLONG)payload->object_offset;
        if (offset.QuadPart < 0 || !SetFilePointerEx(source, offset, NULL, FILE_BEGIN)) {
            CloseHandle(target); code = AX_CORRUPT; goto done;
        }
        while (remaining) {
            DWORD chunk = remaining > AX_IO_CHUNK ? AX_IO_CHUNK : (DWORD)remaining;
            DWORD read = 0;
            if (!ReadFile(source, buffer, chunk, &read, NULL) || read != chunk || !ax_write_all(target, buffer, read)) {
                CloseHandle(target); code = AX_CORRUPT; goto done;
            }
            remaining -= read;
        }
        if (!ax_flush(target) || !CloseHandle(target)) { code = AX_CORRUPT; goto done; }
    }
    code = AX_OK;
done:
    if (source != INVALID_HANDLE_VALUE) CloseHandle(source);
    ax_object_free(&object);
    return code;
}
