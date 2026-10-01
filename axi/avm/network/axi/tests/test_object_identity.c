#include "../axi_internal.h"

#include <stdio.h>
#include <string.h>
#include <wchar.h>

int wmain(int argc, wchar_t **argv) {
    wchar_t store[AX_PATH_CAP];
    char session[AX_ID_CAP], ref_id[AX_ID_CAP], verified_id[AX_ID_CAP];
    AxObject object;
    int exists = 0, code;

    if (!ax_is_safe_relative_path("nested/payload.bin", 18) ||
        ax_is_safe_relative_path("../escape.bin", 13) ||
        ax_is_safe_relative_path(".axi/FORMAT", 13) ||
        ax_is_safe_relative_path("nested\\payload.bin", 18) ||
        ax_is_safe_relative_path("C:/escape.bin", 13)) {
        fputs("FAIL: shared payload-path safety contract is inconsistent\n", stderr);
        return 1;
    }
    if (argc != 3 || !ax_wide_to_utf8(argv[2], session, sizeof(session))) {
        fputs("FAIL: usage test_object_identity <root> <session>\n", stderr);
        return 2;
    }
    memset(&object, 0, sizeof(object));
    code = ax_store_validate(argv[1], store);
    if (code != AX_OK) {
        fprintf(stderr, "FAIL: store validation returned %d\n", code);
        return 1;
    }
    code = ax_read_ref(store, session, ref_id, &exists);
    if (code != AX_OK || !exists) {
        fprintf(stderr, "FAIL: ref resolution returned %d exists=%d\n", code, exists);
        return 1;
    }
    code = ax_object_verify(store, ref_id, &object);
    if (code != AX_OK) {
        fprintf(stderr, "FAIL: object verification returned %d\n", code);
        return 1;
    }
    ax_hex(object.object_id, verified_id);
    ax_object_free(&object);
    if (strcmp(ref_id, verified_id) != 0) {
        fprintf(stderr, "FAIL: verified object_id %s differs from ref %s\n",
                verified_id, ref_id);
        return 1;
    }
    puts("PASS: shared path safety and content-addressed object identity verified");
    return 0;
}
