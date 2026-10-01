#pragma once
#include "euler_pool.h"

// =====================================================================
// EROS DATA BUS: The Central Nervous System
// =====================================================================
typedef enum {
    EROS_SIGNAL_TEXT = 0,
    EROS_SIGNAL_AUDIO = 1,
    EROS_SIGNAL_TENSOR = 2
} ErosSignalType;

// FFI_SharedMemory acts as the universal sensory bus for Eros.
// No data is copied. The .allos libraries (audio, speech, net) write directly to this buffer.
typedef struct {
    void* buffer;       // Pointer natively mapped inside the EulerPool
    uint32_t size;      // Size of the sensory payload
    ErosSignalType data_type; 
} FFI_SharedMemory;

typedef struct {
    EulerPool* cognitive_pool;
    FFI_SharedMemory active_signal;
} ErosBus;

static inline ErosBus eros_bus_mount(EulerPool* pool) {
    return (ErosBus){ pool, {NULL, 0, EROS_SIGNAL_TEXT} };
}

static inline void eros_bus_transmit(ErosBus* bus, ErosSignalType type, uint32_t size) {
    // Maps the sensory data directly into the contiguous Euler pool
    bus->active_signal.buffer = (char*)bus->cognitive_pool->base_ptr + bus->cognitive_pool->allocated_offset;
    bus->active_signal.size = size;
    bus->active_signal.data_type = type;
    
    // Advance the pool offset (Zero-latency allocation)
    bus->cognitive_pool->allocated_offset += size;
}
