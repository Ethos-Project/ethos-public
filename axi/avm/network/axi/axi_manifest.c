#include "axi_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void skip_space(const char **cursor) {
    while (**cursor == ' ' || **cursor == '\t' || **cursor == '\r' || **cursor == '\n') ++*cursor;
}

static char *copy_value(const char *start, size_t length) {
    char *value = (char *)malloc(length + 1);
    if (!value) return NULL;
    memcpy(value, start, length);
    value[length] = '\0';
    return value;
}

static int compare_string(const void *left, const void *right) {
    const AxString *a = (const AxString *)left;
    const AxString *b = (const AxString *)right;
    size_t length = a->length < b->length ? a->length : b->length;
    int result = memcmp(a->value, b->value, length);
    if (result) return result;
    return (a->length > b->length) - (a->length < b->length);
}

static int append_string(AxString **items, size_t *count, size_t *capacity, const char *start, size_t length) {
    AxString *expanded;
    if (*count == *capacity) {
        size_t next = *capacity ? *capacity * 2 : 4;
        expanded = (AxString *)realloc(*items, next * sizeof(**items));
        if (!expanded) return 0;
        *items = expanded;
        *capacity = next;
    }
    (*items)[*count].value = copy_value(start, length);
    if (!(*items)[*count].value) return 0;
    (*items)[*count].length = length;
    ++*count;
    return 1;
}

static int parse_list(const char *value, AxString **items, size_t *count) {
    const char *cursor = value;
    size_t capacity = 0;
    *items = NULL;
    *count = 0;
    skip_space(&cursor);
    if (*cursor != '[') return 0;
    ++cursor;
    for (;;) {
        const char *start;
        skip_space(&cursor);
        if (*cursor == ']') return 1;
        if (*cursor != '"') return 0;
        start = ++cursor;
        while (*cursor && *cursor != '"') {
            if (*cursor == '\\' || (unsigned char)*cursor < 0x20) return 0;
            ++cursor;
        }
        if (*cursor != '"' || !append_string(items, count, &capacity, start, (size_t)(cursor - start))) return 0;
        ++cursor;
        skip_space(&cursor);
        if (*cursor == ']') return 1;
        if (*cursor != ',') return 0;
        ++cursor;
    }
}

int ax_is_safe_relative_path(const char *value, size_t length) {
    const char *component = value;
    size_t i;
    wchar_t probe[AX_PATH_CAP];
    if (!length || length >= AX_PATH_CAP || value[0] == '/' || value[0] == '\\' ||
        (length > 1 && value[1] == ':') || !ax_utf8_to_wide(value, length, probe, AX_PATH_CAP)) return 0;
    for (i = 0; i <= length; ++i) {
        if (i < length && (value[i] == '\\' || value[i] == '*' || value[i] == '?' || value[i] == '\0')) return 0;
        if (i == length || value[i] == '/') {
            size_t component_length = (size_t)(value + i - component);
            if (!component_length || (component_length == 1 && component[0] == '.') ||
                (component_length == 2 && component[0] == '.' && component[1] == '.')) return 0;
            if (component == value && component_length == 4 && _strnicmp(component, ".axi", 4) == 0) return 0;
            component = value + i + 1;
        }
    }
    return 1;
}

static int sort_validate(AxString *items, size_t count, int identifiers) {
    size_t i;
    qsort(items, count, sizeof(*items), compare_string);
    for (i = 0; i < count; ++i) {
        if ((identifiers && !ax_is_identifier(items[i].value)) ||
            (!identifiers && !ax_is_safe_relative_path(items[i].value, items[i].length)) ||
            (i && compare_string(&items[i - 1], &items[i]) == 0)) return 0;
    }
    return 1;
}

static const char *field_value(const char *text, const char *field) {
    size_t field_length = strlen(field);
    const char *line = text;
    while (*line) {
        const char *cursor = line;
        while (*cursor == ' ' || *cursor == '\t') ++cursor;
        if (strncmp(cursor, field, field_length) == 0 && cursor[field_length] == ':') {
            cursor += field_length + 1;
            skip_space(&cursor);
            return cursor;
        }
        line = strchr(line, '\n');
        if (!line) break;
        ++line;
    }
    return NULL;
}

static int parse_scalar(const char *value, char out[AX_ID_CAP]) {
    const char *end;
    size_t length;
    if (!value) return 0;
    if (*value == '"') {
        ++value;
        end = strchr(value, '"');
        if (!end) return 0;
    } else {
        end = value;
        while (*end && *end != '\r' && *end != '\n' && *end != ' ' && *end != '\t') ++end;
    }
    length = (size_t)(end - value);
    if (!length || length >= AX_ID_CAP) return 0;
    memcpy(out, value, length);
    out[length] = '\0';
    return 1;
}

static int read_manifest(const wchar_t *path, char **text) {
    HANDLE file; LARGE_INTEGER size; DWORD read;
    *text = NULL;
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (file == INVALID_HANDLE_VALUE || !GetFileSizeEx(file, &size) || size.QuadPart <= 0 || size.QuadPart > 16 * 1024 * 1024) {
        if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
        return 0;
    }
    *text = (char *)malloc((size_t)size.QuadPart + 1);
    if (!*text || !ReadFile(file, *text, (DWORD)size.QuadPart, &read, NULL) || read != (DWORD)size.QuadPart) {
        CloseHandle(file); free(*text); *text = NULL; return 0;
    }
    CloseHandle(file);
    (*text)[size.QuadPart] = '\0';
    return 1;
}

void ax_manifest_free(AxManifest *manifest) {
    size_t i;
    if (!manifest) return;
    for (i = 0; i < manifest->issue_count; ++i) free(manifest->issues[i].value);
    for (i = 0; i < manifest->context_count; ++i) free(manifest->contexts[i].value);
    for (i = 0; i < manifest->path_count; ++i) free(manifest->paths[i].value);
    free(manifest->issues);
    free(manifest->contexts);
    free(manifest->paths);
    memset(manifest, 0, sizeof(*manifest));
}

int ax_manifest_load(const wchar_t *path, AxManifest *manifest) {
    char *text = NULL;
    const char *format, *session, *issues, *contexts, *paths;
    int ok = 0;
    memset(manifest, 0, sizeof(*manifest));
    if (!read_manifest(path, &text) || !strstr(text, "Manifest {")) goto done;
    format = field_value(text, "format");
    session = field_value(text, "session_id");
    issues = field_value(text, "issue_ids");
    contexts = field_value(text, "context_ids");
    paths = field_value(text, "paths");
    if (!format || strncmp(format, "axi-wrap-manifest-v1", 20) != 0 ||
        !parse_scalar(session, manifest->session) || !ax_is_identifier(manifest->session) ||
        !issues || !contexts || !paths ||
        !parse_list(issues, &manifest->issues, &manifest->issue_count) ||
        !parse_list(contexts, &manifest->contexts, &manifest->context_count) ||
        !parse_list(paths, &manifest->paths, &manifest->path_count) ||
        !manifest->path_count || !sort_validate(manifest->issues, manifest->issue_count, 1) ||
        !sort_validate(manifest->contexts, manifest->context_count, 1) ||
        !sort_validate(manifest->paths, manifest->path_count, 0)) goto done;
    ok = 1;
done:
    free(text);
    if (!ok) ax_manifest_free(manifest);
    return ok ? AX_OK : AX_MANIFEST;
}
