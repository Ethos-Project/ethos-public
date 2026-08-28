#ifndef IO_C
#define IO_C

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    FILE* handle;
    size_t file_size;
    bool is_open;
} AxiomFile;

bool axiom_io_open_read(AxiomFile* file, const char* filepath) {
    file->handle = fopen(filepath, "rb");
    if (!file->handle) return false;
    fseek(file->handle, 0, SEEK_END);
    file->file_size = (size_t)ftell(file->handle);
    fseek(file->handle, 0, SEEK_SET);
    file->is_open = true;
    return true;
}

bool axiom_io_open_write(AxiomFile* file, const char* filepath) {
    file->handle = fopen(filepath, "wb");
    if (!file->handle) return false;
    file->file_size = 0;
    file->is_open = true;
    return true;
}

size_t axiom_io_read_block(AxiomFile* file, void* destination_buffer, size_t bytes_to_read) {
    if (!file->is_open || !file->handle) return 0;
    return fread(destination_buffer, 1, bytes_to_read, file->handle);
}

size_t axiom_io_write_block(AxiomFile* file, const void* source_buffer, size_t bytes_to_write) {
    if (!file->is_open || !file->handle) return 0;
    size_t written = fwrite(source_buffer, 1, bytes_to_write, file->handle);
    file->file_size += written;
    return written;
}

void axiom_io_close(AxiomFile* file) {
    if (file->is_open && file->handle) {
        fclose(file->handle);
        file->handle = NULL;
        file->is_open = false;
    }
}

#endif // IO_C
