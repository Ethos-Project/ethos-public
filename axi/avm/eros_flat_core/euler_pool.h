#pragma once
#include <stdint.h>
#include <windows.h>

// =====================================================================
// EULER POOL: Cognitive Memory Allocation
// =====================================================================
typedef struct {
    void* base_ptr;
    uint64_t capacity_bytes;
    
    // The Cognitive Partitioning Strategy:
    uint64_t structural_offset;  // Permanent: GGUF Model weights, core architecture
    uint64_t short_term_offset;  // Working Memory: The Context Window (chat history)
    uint64_t transient_offset;   // Scratchpad: Immediate math, incoming audio bytes, scratch data
} EulerPool;

static inline EulerPool euler_init(uint64_t size_in_bytes) {
    void* mem = VirtualAlloc(NULL, size_in_bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    return (EulerPool){ mem, size_in_bytes, 0, 0, 0 };
}

// Locks the GGUF weights. This boundary is never flushed.
static inline void euler_lock_structural(EulerPool* pool, uint64_t size) {
    pool->structural_offset = size;
    pool->short_term_offset = size;
    pool->transient_offset = size;
}

// Stores conversational context. Slides forward as conversation continues.
static inline void euler_commit_short_term(EulerPool* pool, uint64_t size) {
    pool->short_term_offset += size;
    pool->transient_offset = pool->short_term_offset;
}

// ONLY flushes the transient scratchpad (the math/sensory processing).
// Eros retains his conversational context window perfectly intact.
static inline void euler_flush_transient(EulerPool* pool) {
    pool->transient_offset = pool->short_term_offset; 
}
