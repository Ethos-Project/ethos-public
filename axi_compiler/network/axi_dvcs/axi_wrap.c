#include "axi_internal.h"

#include <string.h>

int ax_wrap(const wchar_t *root, const wchar_t *manifest_path, char object_id[AX_ID_CAP], int *already) {
    wchar_t store[AX_PATH_CAP];
    AxManifest manifest;
    AxObject object, current;
    char previous[AX_ID_CAP];
    int exists = 0;
    int code;

    *already = 0;
    memset(&manifest, 0, sizeof(manifest));
    memset(&object, 0, sizeof(object));
    memset(&current, 0, sizeof(current));
    code = ax_store_validate(root, store);
    if (code != AX_OK) goto done;
    code = ax_manifest_load(manifest_path, &manifest);
    if (code != AX_OK) goto done;
    code = ax_capture_open(root, &manifest, &object);
    if (code != AX_OK) goto done;
    code = ax_state_digest(&object);
    if (code != AX_OK) goto done;
    code = ax_read_ref(store, manifest.session, previous, &exists);
    if (code != AX_OK) goto done;
    if (exists) {
        code = ax_object_verify(store, previous, &current);
        if (code != AX_OK) goto done;
        if (memcmp(current.state_digest, object.state_digest, sizeof(object.state_digest)) == 0) {
            memcpy(object_id, previous, AX_ID_CAP);
            *already = 1;
            code = AX_OK;
            goto done;
        }
        object.has_parent = 1;
        if (!ax_unhex(previous, object.parent_id)) {
            code = AX_CORRUPT;
            goto done;
        }
    }
    code = ax_object_publish(store, &object, object_id);
    if (code != AX_OK) goto done;
    code = ax_advance_ref(store, manifest.session, object_id, exists ? previous : NULL);
done:
    ax_object_free(&current);
    ax_object_free(&object);
    ax_manifest_free(&manifest);
    return code;
}
