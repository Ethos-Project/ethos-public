#pragma once

struct FFI_SharedMemory {
    void* buffer;
    int heap_head;
    char dynamic_heap[4096];
};

inline float euler_get_prior_surprise(FFI_SharedMemory*, const char*) { return 0.0f; }
inline float euler_get_precision(FFI_SharedMemory*, const char*) { return 1.0f; }
inline int euler_alloc(FFI_SharedMemory*, int) { return 0; }
inline void euler_pool_init(FFI_SharedMemory*) {}
