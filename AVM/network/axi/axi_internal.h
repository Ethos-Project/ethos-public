#ifndef axi_INTERNAL_H
#define axi_INTERNAL_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <stdint.h>
#include <stddef.h>

#define AX_PATH_CAP 32768
#define AX_ID_CAP 65
#define AX_IO_CHUNK 65536

enum {
    AX_OK = 0,
    AX_USAGE = 2,
    AX_STORE = 3,
    AX_MANIFEST = 4,
    AX_CAPTURE = 5,
    AX_OBJECT = 6,
    AX_REF = 7,
    AX_CORRUPT = 8,
    AX_DESTINATION = 9
};

typedef struct {
    char *value;
    size_t length;
} AxString;

typedef struct {
    char session[AX_ID_CAP];
    AxString *issues;
    size_t issue_count;
    AxString *contexts;
    size_t context_count;
    AxString *paths;
    size_t path_count;
} AxManifest;

typedef struct {
    char *path;
    size_t path_length;
    HANDLE handle;
    uint64_t size;
    uint64_t object_offset;
    unsigned char digest[32];
    FILE_ID_INFO id_before;
    FILE_BASIC_INFO basic_before;
} AxPayload;

typedef struct {
    char session[AX_ID_CAP];
    AxString *issues;
    size_t issue_count;
    AxString *contexts;
    size_t context_count;
    AxPayload *payloads;
    size_t payload_count;
    unsigned char state_digest[32];
    int has_parent;
    unsigned char parent_id[32];
    unsigned char object_id[32];
} AxObject;

typedef struct {
    BCRYPT_ALG_HANDLE algorithm;
    BCRYPT_HASH_HANDLE hash;
    unsigned char *object;
} AxHash;

typedef enum {
    AX_FAIL_NONE = 0,
    AX_FAIL_OBJECT_WRITE,
    AX_FAIL_OBJECT_FLUSH,
    AX_FAIL_OBJECT_PROMOTE,
    AX_FAIL_OBJECT_VERIFY,
    AX_FAIL_REF_WRITE,
    AX_FAIL_REF_FLUSH,
    AX_FAIL_REF_PROMOTE,
    AX_FAIL_REF_VERIFY,
    AX_FAIL_REF_ROLLBACK
} AxFailPoint;

extern AxFailPoint ax_fail_point;

void ax_diag(const char *format, ...);
int ax_join(wchar_t *out, size_t cap, const wchar_t *left, const wchar_t *right);
int ax_utf8_to_wide(const char *text, size_t length, wchar_t *out, size_t cap);
int ax_wide_to_utf8(const wchar_t *text, char *out, size_t cap);
int ax_is_identifier(const char *value);
int ax_is_safe_relative_path(const char *value, size_t length);
int ax_is_lower_hex_id(const char *value, size_t length);
void ax_hex(const unsigned char digest[32], char out[AX_ID_CAP]);
int ax_unhex(const char text[64], unsigned char out[32]);
int ax_write_all(HANDLE handle, const void *bytes, size_t length);
int ax_read_exact(HANDLE handle, void *bytes, size_t length);
int ax_flush(HANDLE handle);
int ax_hash_begin(AxHash *hash);
int ax_hash_update(AxHash *hash, const void *bytes, size_t length);
int ax_hash_finish(AxHash *hash, unsigned char digest[32]);
void ax_hash_abort(AxHash *hash);
int ax_hash_file(HANDLE file, unsigned char digest[32], uint64_t *size);
int ax_store_validate(const wchar_t *root, wchar_t store[AX_PATH_CAP]);
int ax_store_discover(wchar_t root[AX_PATH_CAP], wchar_t store[AX_PATH_CAP]);
int ax_store_init(const wchar_t *root);
int ax_read_ref(const wchar_t *store, const char *session, char id[AX_ID_CAP], int *exists);
int ax_advance_ref(const wchar_t *store, const char *session, const char id[AX_ID_CAP], const char *old_id);
int ax_object_path(const wchar_t *store, const char id[AX_ID_CAP], wchar_t out[AX_PATH_CAP]);
int ax_manifest_load(const wchar_t *path, AxManifest *manifest);
void ax_manifest_free(AxManifest *manifest);
int ax_capture_open(const wchar_t *root, AxManifest *manifest, AxObject *object);
void ax_object_free(AxObject *object);
int ax_state_digest(AxObject *object);
int ax_object_publish(const wchar_t *store, AxObject *object, char id[AX_ID_CAP]);
int ax_object_verify(const wchar_t *store, const char id[AX_ID_CAP], AxObject *object);
int ax_wrap(const wchar_t *root, const wchar_t *manifest_path, char object_id[AX_ID_CAP], int *already);
int ax_resume(const wchar_t *root, const char *session, const wchar_t *destination);
int ax_run_workflows(const wchar_t *root, const char *event_name);

#endif
