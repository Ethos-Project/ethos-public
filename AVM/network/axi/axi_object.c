#include "axi_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AX_OBJECT_MAGIC "AXIOBJ1"
#define AX_OBJECT_MAGIC_SIZE 7

static char *copy_text(const char *text, size_t length) {
    char *copy = (char *)malloc(length + 1);
    if (!copy) return NULL;
    memcpy(copy, text, length);
    copy[length] = '\0';
    return copy;
}

static int regular_file(const wchar_t *path) {
    DWORD attributes = GetFileAttributesW(path);
    return attributes != INVALID_FILE_ATTRIBUTES &&
        !(attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT));
}

static int put_u32(HANDLE handle, uint32_t value) { return ax_write_all(handle, &value, sizeof(value)); }
static int put_u64(HANDLE handle, uint64_t value) { return ax_write_all(handle, &value, sizeof(value)); }
static int get_u32(HANDLE handle, uint32_t *value) { return ax_read_exact(handle, value, sizeof(*value)); }
static int get_u64(HANDLE handle, uint64_t *value) { return ax_read_exact(handle, value, sizeof(*value)); }

static int digest_payload(AxPayload *payload) {
    uint64_t size;
    if (!ax_hash_file(payload->handle, payload->digest, &size)) return 0;
    payload->size = size;
    return 1;
}

void ax_object_free(AxObject *object) {
    size_t i;
    if (!object) return;
    for (i = 0; i < object->payload_count; ++i) {
        if (object->payloads[i].handle && object->payloads[i].handle != INVALID_HANDLE_VALUE) {
            CloseHandle(object->payloads[i].handle);
        }
        free(object->payloads[i].path);
    }
    free(object->payloads);
    memset(object, 0, sizeof(*object));
}

int ax_capture_open(const wchar_t *root, AxManifest *manifest, AxObject *object) {
    size_t i;
    memset(object, 0, sizeof(*object));
    if (!manifest->session[0] || !ax_is_identifier(manifest->session) || !manifest->path_count) return AX_MANIFEST;
    memcpy(object->session, manifest->session, sizeof(object->session));
    object->payloads = (AxPayload *)calloc(manifest->path_count, sizeof(*object->payloads));
    if (!object->payloads) return AX_CAPTURE;
    object->payload_count = manifest->path_count;
    for (i = 0; i < manifest->path_count; ++i) {
        wchar_t relative[AX_PATH_CAP], full[AX_PATH_CAP];
        AxPayload *payload = &object->payloads[i];
        if (!ax_is_safe_relative_path(manifest->paths[i].value, manifest->paths[i].length) ||
            !ax_utf8_to_wide(manifest->paths[i].value, manifest->paths[i].length, relative, AX_PATH_CAP) ||
            !ax_join(full, AX_PATH_CAP, root, relative) || !regular_file(full)) {
            ax_object_free(object); return AX_CAPTURE;
        }
        payload->path = copy_text(manifest->paths[i].value, manifest->paths[i].length);
        payload->path_length = manifest->paths[i].length;
        payload->handle = CreateFileW(full, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
        if (!payload->path || payload->handle == INVALID_HANDLE_VALUE || !digest_payload(payload)) {
            ax_object_free(object); return AX_CAPTURE;
        }
    }
    return AX_OK;
}

int ax_state_digest(AxObject *object) {
    AxHash hash;
    size_t i;
    if (!ax_hash_begin(&hash) ||
        !ax_hash_update(&hash, "axi-dvcs-state-v1\n", 18) ||
        !ax_hash_update(&hash, object->session, strlen(object->session)) ||
        !ax_hash_update(&hash, "\n", 1)) {
        ax_hash_abort(&hash); return AX_OBJECT;
    }
    for (i = 0; i < object->payload_count; ++i) {
        AxPayload *payload = &object->payloads[i];
        if (!ax_hash_update(&hash, payload->path, payload->path_length) ||
            !ax_hash_update(&hash, "\0", 1) ||
            !ax_hash_update(&hash, &payload->size, sizeof(payload->size)) ||
            !ax_hash_update(&hash, payload->digest, sizeof(payload->digest))) {
            ax_hash_abort(&hash); return AX_OBJECT;
        }
    }
    return ax_hash_finish(&hash, object->state_digest) ? AX_OK : AX_OBJECT;
}

static int write_payload(HANDLE destination, AxPayload *payload) {
    unsigned char buffer[AX_IO_CHUNK];
    LARGE_INTEGER zero;
    DWORD read;
    zero.QuadPart = 0;
    if (!SetFilePointerEx(payload->handle, zero, NULL, FILE_BEGIN)) return 0;
    do {
        if (!ReadFile(payload->handle, buffer, sizeof(buffer), &read, NULL)) return 0;
        if (read && !ax_write_all(destination, buffer, read)) return 0;
    } while (read);
    return 1;
}

int ax_object_publish(const wchar_t *store, AxObject *object, char id[AX_ID_CAP]) {
    wchar_t temporary[AX_PATH_CAP], objects[AX_PATH_CAP], fanout_path[AX_PATH_CAP], final[AX_PATH_CAP];
    HANDLE file = INVALID_HANDLE_VALUE;
    unsigned char digest[32];
    uint64_t ignored_size = 0;
    uint32_t session_length = (uint32_t)strlen(object->session);
    uint32_t payload_count = (uint32_t)object->payload_count;
    size_t i;
    int ok = 0;
    unsigned char no_parent[32] = {0};

    if (ax_state_digest(object) != AX_OK || !ax_join(temporary, AX_PATH_CAP, store, L"tmp\\object-write.tmp")) return AX_OBJECT;
    file = CreateFileW(temporary, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
    if (file == INVALID_HANDLE_VALUE ||
        !ax_write_all(file, AX_OBJECT_MAGIC, AX_OBJECT_MAGIC_SIZE) ||
        !put_u32(file, 1) || !put_u32(file, session_length) || !put_u32(file, payload_count) ||
        !ax_write_all(file, object->state_digest, sizeof(object->state_digest)) ||
        !ax_write_all(file, object->has_parent ? object->parent_id : no_parent, sizeof(no_parent)) ||
        !ax_write_all(file, object->session, session_length)) goto done;
    for (i = 0; i < object->payload_count; ++i) {
        AxPayload *payload = &object->payloads[i];
        if (payload->path_length > UINT32_MAX || !put_u32(file, (uint32_t)payload->path_length) ||
            !put_u64(file, payload->size) || !ax_write_all(file, payload->digest, sizeof(payload->digest)) ||
            !ax_write_all(file, payload->path, payload->path_length) || !write_payload(file, payload)) goto done;
    }
    if (!ax_flush(file) || !CloseHandle(file)) { file = INVALID_HANDLE_VALUE; goto done; }
    file = INVALID_HANDLE_VALUE;
    file = CreateFileW(temporary, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (file == INVALID_HANDLE_VALUE || !ax_hash_file(file, digest, &ignored_size) || !CloseHandle(file)) { file = INVALID_HANDLE_VALUE; goto done; }
    file = INVALID_HANDLE_VALUE;
    ax_hex(digest, id);
    {
        wchar_t wide_id[AX_ID_CAP], fanout[3];
        if (!ax_utf8_to_wide(id, 64, wide_id, AX_ID_CAP) || !ax_join(objects, AX_PATH_CAP, store, L"objects")) goto done;
        fanout[0] = wide_id[0]; fanout[1] = wide_id[1]; fanout[2] = L'\0';
        if (!ax_join(fanout_path, AX_PATH_CAP, objects, fanout) ||
            (!CreateDirectoryW(fanout_path, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) ||
            !ax_join(final, AX_PATH_CAP, fanout_path, wide_id + 2)) goto done;
    }
    if (GetFileAttributesW(final) != INVALID_FILE_ATTRIBUTES) {
        DeleteFileW(temporary); ok = 1; goto done;
    }
    if (!MoveFileExW(temporary, final, MOVEFILE_WRITE_THROUGH)) goto done;
    ok = 1;
done:
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    if (!ok) DeleteFileW(temporary);
    return ok ? AX_OK : AX_OBJECT;
}

int ax_object_verify(const wchar_t *store, const char id[AX_ID_CAP], AxObject *object) {
    wchar_t path[AX_PATH_CAP];
    HANDLE file = INVALID_HANDLE_VALUE;
    unsigned char digest[32], magic[AX_OBJECT_MAGIC_SIZE], parent[32];
    uint64_t ignored_size = 0;
    uint32_t version, session_length, payload_count, path_length;
    size_t i;
    LARGE_INTEGER zero, position, skip;

    memset(object, 0, sizeof(*object));
    if (!ax_object_path(store, id, path) || !regular_file(path)) return AX_CORRUPT;
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (file == INVALID_HANDLE_VALUE || !ax_hash_file(file, digest, &ignored_size)) goto corrupt;
    { char actual[AX_ID_CAP]; ax_hex(digest, actual); if (strcmp(actual, id) != 0 || !ax_unhex(id, object->object_id)) goto corrupt; }
    zero.QuadPart = 0;
    if (!SetFilePointerEx(file, zero, NULL, FILE_BEGIN) || !ax_read_exact(file, magic, sizeof(magic)) ||
        memcmp(magic, AX_OBJECT_MAGIC, sizeof(magic)) != 0 || !get_u32(file, &version) || version != 1 ||
        !get_u32(file, &session_length) || !get_u32(file, &payload_count) || session_length == 0 || session_length >= AX_ID_CAP ||
        payload_count == 0 || payload_count > 65536 || !ax_read_exact(file, object->state_digest, sizeof(object->state_digest)) ||
        !ax_read_exact(file, parent, sizeof(parent)) || !ax_read_exact(file, object->session, session_length)) goto corrupt;
    object->session[session_length] = '\0';
    if (!ax_is_identifier(object->session)) goto corrupt;
    if (memcmp(parent, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", sizeof(parent)) != 0) {
        memcpy(object->parent_id, parent, sizeof(parent));
        object->has_parent = 1;
    }
    object->payloads = (AxPayload *)calloc(payload_count, sizeof(*object->payloads));
    if (!object->payloads) goto corrupt;
    object->payload_count = payload_count;
    for (i = 0; i < object->payload_count; ++i) {
        AxPayload *payload = &object->payloads[i];
        if (!get_u32(file, &path_length) || path_length == 0 || path_length >= AX_PATH_CAP ||
            !get_u64(file, &payload->size) || !ax_read_exact(file, payload->digest, sizeof(payload->digest))) goto corrupt;
        payload->path = (char *)malloc((size_t)path_length + 1);
        if (!payload->path || !ax_read_exact(file, payload->path, path_length)) goto corrupt;
        payload->path[path_length] = '\0';
        payload->path_length = path_length;
        if (!ax_is_safe_relative_path(payload->path, payload->path_length) ||
            !SetFilePointerEx(file, zero, &position, FILE_CURRENT)) goto corrupt;
        payload->object_offset = (uint64_t)position.QuadPart;
        skip.QuadPart = (LONGLONG)payload->size;
        if (skip.QuadPart < 0 || !SetFilePointerEx(file, skip, NULL, FILE_CURRENT)) goto corrupt;
    }
    CloseHandle(file);
    return AX_OK;
corrupt:
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    ax_object_free(object);
    return AX_CORRUPT;
}
