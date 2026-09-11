// =====================================================================
// MODULE: core::io
// DESCRIPTION: Axi standard library for bare-metal I/O operations.
// Integrates closely with core::mem for Euler Pool allocation.
// =====================================================================

@C_Include("<stdio.h>")
@C_Include("<stdlib.h>")

import core.mem

node c_io_subsystem {
`
    // Reads an entire file into the Euler Pool
    char* axi_read_file(const char* filepath) {
        FILE *f = fopen(filepath, "rb");
        if (!f) return NULL;
        
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);

        // Allocate directly from the Euler Pool using C-to-Axi handoff
        char *buffer = (char *)axi_bump_alloc(fsize + 1);
        if (!buffer) {
            fclose(f);
            return NULL;
        }

        fread(buffer, fsize, 1, f);
        fclose(f);
        buffer[fsize] = 0;
        
        return buffer;
    }

    int axi_write_file(const char* filepath, const char* content) {
        FILE *f = fopen(filepath, "wb");
        if (!f) return 0; // false
        
        fputs(content, f);
        fclose(f);
        return 1; // true
    }
`
}

// Axi Semantic High-Level Wrappers
HOW TO read(file_path):
    PUT c_call("axi_read_file", file_path) IN content_ptr
    IF content_ptr == NULL:
        WRITE "[AXI IO] FATAL: File read failed." TO SCREEN
        RETURN NULL
    RETURN content_ptr

HOW TO write(file_path, content):
    PUT c_call("axi_write_file", file_path, content) IN success
    IF success == 0:
        WRITE "[AXI IO] FATAL: File write failed." TO SCREEN
    RETURN success
