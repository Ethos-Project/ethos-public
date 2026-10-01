#include "axi_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <wchar.h>
#include <limits.h>

AxFailPoint ax_fail_point = AX_FAIL_NONE;

void ax_diag(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    fputc('\n', stderr);
    va_end(args);
}

int ax_join(wchar_t *out, size_t cap, const wchar_t *left, const wchar_t *right) {
    int n = _snwprintf(out, cap, L"%ls\\%ls", left, right);
    return n >= 0 && (size_t)n < cap;
}

int ax_utf8_to_wide(const char *text, size_t length, wchar_t *out, size_t cap) {
    int need;
    if (length > INT_MAX || cap > INT_MAX) return 0;
    need = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, (int)length, out, (int)cap - 1);
    if (need <= 0 || (size_t)need >= cap) return 0;
    out[need] = L'\0';
    return 1;
}

int ax_wide_to_utf8(const wchar_t *text, char *out, size_t cap) {
    int need = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text, -1, out, (int)cap, NULL, NULL);
    return need > 0;
}

int ax_is_identifier(const char *value) {
    size_t i, n = strlen(value);
    if (n == 0 || n > 64) return 0;
    for (i = 0; i < n; ++i) {
        unsigned char c = (unsigned char)value[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9') || (i > 0 && (c == '.' || c == '_' || c == '-')))) return 0;
    }
    return 1;
}

int ax_is_lower_hex_id(const char *value, size_t length) {
    size_t i;
    if (length != 64) return 0;
    for (i = 0; i < length; ++i) if (!((value[i] >= '0' && value[i] <= '9') || (value[i] >= 'a' && value[i] <= 'f'))) return 0;
    return 1;
}

void ax_hex(const unsigned char digest[32], char out[AX_ID_CAP]) {
    static const char digits[] = "0123456789abcdef";
    size_t i;
    for (i = 0; i < 32; ++i) { out[i * 2] = digits[digest[i] >> 4]; out[i * 2 + 1] = digits[digest[i] & 15]; }
    out[64] = '\0';
}

int ax_unhex(const char text[64], unsigned char out[32]) {
    size_t i;
    if (!ax_is_lower_hex_id(text, 64)) return 0;
    for (i = 0; i < 32; ++i) {
        unsigned char a = (unsigned char)(text[i * 2] <= '9' ? text[i * 2] - '0' : text[i * 2] - 'a' + 10);
        unsigned char b = (unsigned char)(text[i * 2 + 1] <= '9' ? text[i * 2 + 1] - '0' : text[i * 2 + 1] - 'a' + 10);
        out[i] = (unsigned char)((a << 4) | b);
    }
    return 1;
}

int ax_write_all(HANDLE handle, const void *bytes, size_t length) {
    const unsigned char *cursor = (const unsigned char *)bytes;
    while (length) {
        DWORD chunk = length > MAXDWORD ? MAXDWORD : (DWORD)length, written = 0;
        if (!WriteFile(handle, cursor, chunk, &written, NULL) || written != chunk) return 0;
        cursor += written; length -= written;
    }
    return 1;
}

int ax_read_exact(HANDLE handle, void *bytes, size_t length) {
    unsigned char *cursor = (unsigned char *)bytes;
    while (length) {
        DWORD chunk = length > MAXDWORD ? MAXDWORD : (DWORD)length, got = 0;
        if (!ReadFile(handle, cursor, chunk, &got, NULL) || got != chunk) return 0;
        cursor += got; length -= got;
    }
    return 1;
}

int ax_flush(HANDLE handle) { return FlushFileBuffers(handle) != 0; }

int ax_hash_begin(AxHash *h) {
    DWORD object_length = 0, returned = 0;
    memset(h, 0, sizeof(*h));
    if (BCryptOpenAlgorithmProvider(&h->algorithm, BCRYPT_SHA256_ALGORITHM, NULL, 0) < 0 ||
        BCryptGetProperty(h->algorithm, BCRYPT_OBJECT_LENGTH, (PUCHAR)&object_length, sizeof(object_length), &returned, 0) < 0) {
        ax_hash_abort(h); return 0;
    }
    h->object = (unsigned char *)malloc(object_length);
    if (!h->object || BCryptCreateHash(h->algorithm, &h->hash, h->object, object_length, NULL, 0, 0) < 0) {
        ax_hash_abort(h); return 0;
    }
    return 1;
}

int ax_hash_update(AxHash *h, const void *bytes, size_t length) {
    const unsigned char *cursor = (const unsigned char *)bytes;
    while (length) {
        ULONG chunk = length > ULONG_MAX ? ULONG_MAX : (ULONG)length;
        if (BCryptHashData(h->hash, (PUCHAR)cursor, chunk, 0) < 0) return 0;
        cursor += chunk; length -= chunk;
    }
    return 1;
}

int ax_hash_finish(AxHash *h, unsigned char digest[32]) {
    int ok = BCryptFinishHash(h->hash, digest, 32, 0) >= 0;
    ax_hash_abort(h);
    return ok;
}

void ax_hash_abort(AxHash *h) {
    if (h->hash) BCryptDestroyHash(h->hash);
    if (h->algorithm) BCryptCloseAlgorithmProvider(h->algorithm, 0);
    free(h->object); memset(h, 0, sizeof(*h));
}

int ax_hash_file(HANDLE file, unsigned char digest[32], uint64_t *size) {
    AxHash hash; unsigned char buffer[AX_IO_CHUNK]; DWORD got; LARGE_INTEGER zero;
    zero.QuadPart = 0;
    if (!SetFilePointerEx(file, zero, NULL, FILE_BEGIN) || !ax_hash_begin(&hash)) return 0;
    *size = 0;
    do {
        if (!ReadFile(file, buffer, sizeof(buffer), &got, NULL)) { ax_hash_abort(&hash); return 0; }
        if (got && !ax_hash_update(&hash, buffer, got)) { ax_hash_abort(&hash); return 0; }
        *size += got;
    } while (got);
    return ax_hash_finish(&hash, digest) && SetFilePointerEx(file, zero, NULL, FILE_BEGIN);
}

static int real_dir(const wchar_t *path) {
    DWORD a = GetFileAttributesW(path);
    return a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY) && !(a & FILE_ATTRIBUTE_REPARSE_POINT);
}

static int real_file(const wchar_t *path) {
    DWORD a = GetFileAttributesW(path);
    return a != INVALID_FILE_ATTRIBUTES && !(a & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT));
}

static int read_small(const wchar_t *path, char *buffer, size_t cap, size_t *length) {
    HANDLE h = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    LARGE_INTEGER n; DWORD got;
    if (h == INVALID_HANDLE_VALUE || !GetFileSizeEx(h, &n) || n.QuadPart < 0 || (uint64_t)n.QuadPart >= cap ||
        !ReadFile(h, buffer, (DWORD)n.QuadPart, &got, NULL) || got != (DWORD)n.QuadPart) { if (h != INVALID_HANDLE_VALUE) CloseHandle(h); return 0; }
    CloseHandle(h); buffer[got] = '\0'; *length = got; return 1;
}

int ax_store_validate(const wchar_t *root, wchar_t store[AX_PATH_CAP]) {
    wchar_t path[AX_PATH_CAP]; char format[32]; size_t n;
    if (!real_dir(root) || !ax_join(store, AX_PATH_CAP, root, L".axi") || !real_dir(store) ||
        !ax_join(path, AX_PATH_CAP, store, L"FORMAT") || !real_file(path) || !read_small(path, format, sizeof(format), &n) ||
        n != 11 || memcmp(format, "axi-dvcs 1\n", 11) != 0) return AX_STORE;
    if (!ax_join(path, AX_PATH_CAP, store, L"objects") || !real_dir(path) ||
        !ax_join(path, AX_PATH_CAP, store, L"refs\\wip") || !real_dir(path) ||
        !ax_join(path, AX_PATH_CAP, store, L"refs\\previous") || !real_dir(path) ||
        !ax_join(path, AX_PATH_CAP, store, L"tmp") || !real_dir(path) ||
        !ax_join(path, AX_PATH_CAP, store, L"wrap-manifest.toon") || !real_file(path)) return AX_STORE;
    return AX_OK;
}

int ax_store_discover(wchar_t root[AX_PATH_CAP], wchar_t store[AX_PATH_CAP]) {
    wchar_t candidate[AX_PATH_CAP]; DWORD a;
    if (!GetCurrentDirectoryW(AX_PATH_CAP, root)) return AX_STORE;
    for (;;) {
        if (!ax_join(candidate, AX_PATH_CAP, root, L".axi")) return AX_STORE;
        a = GetFileAttributesW(candidate);
        if (a != INVALID_FILE_ATTRIBUTES) return ax_store_validate(root, store);
        { wchar_t *slash = wcsrchr(root, L'\\');
          if (!slash || (slash == root + 2 && root[1] == L':')) break;
          *slash = L'\0'; }
    }
    return AX_STORE;
}

static int create_dir(const wchar_t *p) { return CreateDirectoryW(p, NULL) != 0; }
static int create_durable_file(const wchar_t *p, const char *text) {
    HANDLE h = CreateFileW(p, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
    int ok;
    if (h == INVALID_HANDLE_VALUE) return 0;
    ok = ax_write_all(h, text, strlen(text)) && ax_flush(h) && CloseHandle(h);
    if (!ok) { CloseHandle(h); DeleteFileW(p); }
    return ok;
}

int ax_store_init(const wchar_t *root) {
    wchar_t store[AX_PATH_CAP] = {0}, objects[AX_PATH_CAP] = {0};
    wchar_t refs[AX_PATH_CAP] = {0}, wip[AX_PATH_CAP] = {0};
    wchar_t previous[AX_PATH_CAP] = {0}, tmp[AX_PATH_CAP] = {0};
    wchar_t format[AX_PATH_CAP] = {0}, mf[AX_PATH_CAP] = {0};
    if (!real_dir(root) || !ax_join(store, AX_PATH_CAP, root, L".axi")) return AX_STORE;
    if (GetFileAttributesW(store) != INVALID_FILE_ATTRIBUTES || !create_dir(store)) return AX_STORE;
    if (!ax_join(objects, AX_PATH_CAP, store, L"objects") || !create_dir(objects) ||
        !ax_join(refs, AX_PATH_CAP, store, L"refs") || !create_dir(refs) ||
        !ax_join(wip, AX_PATH_CAP, refs, L"wip") || !create_dir(wip) ||
        !ax_join(previous, AX_PATH_CAP, refs, L"previous") || !create_dir(previous) ||
        !ax_join(tmp, AX_PATH_CAP, store, L"tmp") || !create_dir(tmp) ||
        !ax_join(format, AX_PATH_CAP, store, L"FORMAT") || !create_durable_file(format, "axi-dvcs 1\n") ||
        !ax_join(mf, AX_PATH_CAP, store, L"wrap-manifest.toon") ||
        !create_durable_file(mf, "Manifest {\n  format: axi-wrap-manifest-v1\n  session_id: bootstrap\n  issue_ids: []\n  context_ids: []\n  paths: []\n}\n") ||
        ax_store_validate(root, store) != AX_OK) {
        if (mf[0]) DeleteFileW(mf);
        if (format[0]) DeleteFileW(format);
        if (previous[0]) RemoveDirectoryW(previous);
        if (wip[0]) RemoveDirectoryW(wip);
        if (refs[0]) RemoveDirectoryW(refs);
        if (tmp[0]) RemoveDirectoryW(tmp);
        if (objects[0]) RemoveDirectoryW(objects);
        RemoveDirectoryW(store);
        return AX_STORE;
    }
    return AX_OK;
}

int ax_object_path(const wchar_t *store, const char id[AX_ID_CAP], wchar_t out[AX_PATH_CAP]) {
    wchar_t wide_id[AX_ID_CAP], fanout[3], objects[AX_PATH_CAP], dir[AX_PATH_CAP];
    if (!ax_is_lower_hex_id(id, strlen(id)) || !ax_utf8_to_wide(id, 64, wide_id, AX_ID_CAP)) return 0;
    fanout[0] = wide_id[0]; fanout[1] = wide_id[1]; fanout[2] = 0;
    return ax_join(objects, AX_PATH_CAP, store, L"objects") && ax_join(dir, AX_PATH_CAP, objects, fanout) && ax_join(out, AX_PATH_CAP, dir, wide_id + 2);
}

int ax_read_ref(const wchar_t *store, const char *session, char id[AX_ID_CAP], int *exists) {
    wchar_t ws[AX_ID_CAP], dir[AX_PATH_CAP], path[AX_PATH_CAP]; char bytes[66]; size_t n;
    *exists = 0;
    if (!ax_is_identifier(session) || !ax_utf8_to_wide(session, strlen(session), ws, AX_ID_CAP) ||
        !ax_join(dir, AX_PATH_CAP, store, L"refs\\wip") || !ax_join(path, AX_PATH_CAP, dir, ws)) return AX_MANIFEST;
    if (GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES) return GetLastError() == ERROR_FILE_NOT_FOUND ? AX_OK : AX_CORRUPT;
    if (!real_file(path) || !read_small(path, bytes, sizeof(bytes), &n) || n != 65 || bytes[64] != '\n' || !ax_is_lower_hex_id(bytes, 64)) return AX_CORRUPT;
    memcpy(id, bytes, 64); id[64] = 0; *exists = 1; return AX_OK;
}

static int write_ref_temp(const wchar_t *store, const char *tag, const char id[AX_ID_CAP], wchar_t out[AX_PATH_CAP]) {
    wchar_t tmp[AX_PATH_CAP]; unsigned long i; char bytes[65]; HANDLE h;
    memcpy(bytes, id, 64); bytes[64] = '\n';
    if (!ax_join(tmp, AX_PATH_CAP, store, L"tmp")) return 0;
    for (i = 0; i < 100; ++i) {
        wchar_t name[128]; _snwprintf(name, 128, L"ref-%hs-%lu-%lu.tmp", tag, (unsigned long)GetCurrentProcessId(), i);
        if (!ax_join(out, AX_PATH_CAP, tmp, name)) return 0;
        h = CreateFileW(out, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
        if (h != INVALID_HANDLE_VALUE) break;
        if (GetLastError() != ERROR_FILE_EXISTS) return 0;
    }
    if (i == 100) return 0;
    if (ax_fail_point == AX_FAIL_REF_WRITE || !ax_write_all(h, bytes, sizeof(bytes)) || ax_fail_point == AX_FAIL_REF_FLUSH || !ax_flush(h) || !CloseHandle(h)) { CloseHandle(h); DeleteFileW(out); return 0; }
    return 1;
}

int ax_advance_ref(const wchar_t *store, const char *session, const char id[AX_ID_CAP], const char *old_id) {
    wchar_t ws[AX_ID_CAP], current_dir[AX_PATH_CAP], previous_dir[AX_PATH_CAP], current[AX_PATH_CAP], previous[AX_PATH_CAP], current_tmp[AX_PATH_CAP], previous_tmp[AX_PATH_CAP];
    char check[AX_ID_CAP]; int exists, code; AxObject verified;
    if (!ax_utf8_to_wide(session, strlen(session), ws, AX_ID_CAP) || !ax_join(current_dir, AX_PATH_CAP, store, L"refs\\wip") ||
        !ax_join(previous_dir, AX_PATH_CAP, store, L"refs\\previous") || !ax_join(current, AX_PATH_CAP, current_dir, ws) || !ax_join(previous, AX_PATH_CAP, previous_dir, ws) ||
        !write_ref_temp(store, "current", id, current_tmp)) return AX_REF;
    previous_tmp[0] = 0;
    if (old_id && !write_ref_temp(store, "previous", old_id, previous_tmp)) { DeleteFileW(current_tmp); return AX_REF; }
    if (old_id && !MoveFileExW(previous_tmp, previous, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) { DeleteFileW(previous_tmp); DeleteFileW(current_tmp); return AX_REF; }
    if (ax_fail_point == AX_FAIL_REF_PROMOTE || !MoveFileExW(current_tmp, current, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) { DeleteFileW(current_tmp); return AX_REF; }
    code = ax_read_ref(store, session, check, &exists);
    if (ax_fail_point == AX_FAIL_REF_VERIFY || code != AX_OK || !exists || strcmp(check, id) || ax_object_verify(store, id, &verified) != AX_OK) {
        if (old_id && ax_fail_point != AX_FAIL_REF_ROLLBACK) {
            if (!write_ref_temp(store, "rollback", old_id, current_tmp) || !MoveFileExW(current_tmp, current, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) return AX_REF;
            code = ax_read_ref(store, session, check, &exists);
            if (code != AX_OK || !exists || strcmp(check, old_id)) return AX_REF;
        } else if (!old_id && ax_fail_point != AX_FAIL_REF_ROLLBACK) {
            if (!DeleteFileW(current) || GetFileAttributesW(current) != INVALID_FILE_ATTRIBUTES) return AX_REF;
        } else return AX_REF;
        return AX_REF;
    }
    ax_object_free(&verified);
    return AX_OK;
}
