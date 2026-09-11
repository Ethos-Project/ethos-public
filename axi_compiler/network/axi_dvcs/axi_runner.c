#include "axi_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Extremely basic TOON parser specifically designed for internal Axi Actions
// Looks for "on: [event_name]" and "run: command"

static int parse_and_run_toon(const wchar_t *toon_file, const char *event_name) {
    FILE *f = _wfopen(toon_file, L"rb");
    if (!f) return AX_OK; // Ignore unreadable files

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *content = malloc(size + 1);
    if (!content) { fclose(f); return AX_CORRUPT; }

    fread(content, 1, size, f);
    content[size] = '\0';
    fclose(f);

    // Simple TOON check for event binding: on: [event_name]
    char event_trigger[256];
    snprintf(event_trigger, sizeof(event_trigger), "on: [%s]", event_name);
    
    if (strstr(content, event_trigger)) {
        wprintf(L"[axi-runner] Trigger matched %hs in %ls\n", event_name, toon_file);
        
        // Scan for run: "command"
        char *run_ptr = strstr(content, "run: \"");
        if (run_ptr) {
            run_ptr += 6; // skip run: "
            char *end_quote = strchr(run_ptr, '"');
            if (end_quote) {
                *end_quote = '\0';
                
                wchar_t command[4096];
                ax_utf8_to_wide(run_ptr, strlen(run_ptr), command, sizeof(command)/sizeof(wchar_t));
                
                wprintf(L"[axi-runner] Executing step: %ls\n", command);
                int res = _wsystem(command);
                if (res != 0) {
                    wprintf(L"[axi-runner] ERROR: Step failed with code %d\n", res);
                } else {
                    wprintf(L"[axi-runner] Step completed successfully.\n");
                }
            }
        }
    }
    
    free(content);
    return AX_OK;
}

int ax_run_workflows(const wchar_t *root, const char *event_name) {
    wchar_t workflows_dir[AX_PATH_CAP];
    if (!ax_join(workflows_dir, AX_PATH_CAP, root, L".axi\\workflows\\*.toon")) {
        return AX_OK; // no workflows
    }

    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(workflows_dir, &fd);
    if (hFind == INVALID_HANDLE_VALUE) {
        return AX_OK; // No TOON workflows found
    }

    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            wchar_t toon_file[AX_PATH_CAP];
            ax_join(toon_file, AX_PATH_CAP, root, L".axi\\workflows");
            ax_join(toon_file, AX_PATH_CAP, toon_file, fd.cFileName);
            
            parse_and_run_toon(toon_file, event_name);
        }
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);
    return AX_OK;
}
