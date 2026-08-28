#ifndef EULER_POOL_H
#define EULER_POOL_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

typedef struct EulerFreeNode {
    struct EulerFreeNode* next;
} EulerFreeNode;

// Multi-page tracking structure to support heavy applications dynamically
typedef struct EulerPage {
    uint8_t* memory;
    struct EulerPage* next_page;
} EulerPage;

typedef struct {
    size_t chunk_size;
    size_t chunks_per_page;
    size_t total_allocated_bytes;
    EulerPage* page_head;
    EulerFreeNode* free_head;
} EulerPool;

/**
 * Initializes a scalable Euler memory pool with an initial page size.
 */
static inline void euler_pool_init(EulerPool* pool, size_t chunk_size, size_t initial_chunks_per_page) {
    size_t aligned_chunk_size = (chunk_size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
    if (aligned_chunk_size < sizeof(EulerFreeNode)) {
        aligned_chunk_size = sizeof(EulerFreeNode);
    }

    pool->chunk_size = aligned_chunk_size;
    pool->chunks_per_page = initial_chunks_per_page;
    pool->total_allocated_bytes = 0;
    pool->page_head = NULL;
    pool->free_head = NULL;
}

/**
 * Internal helper to allocate a new memory page when capacity is exhausted.
 */
static inline bool euler_pool_grow(EulerPool* pool) {
    size_t page_bytes = pool->chunk_size * pool->chunks_per_page;
    EulerPage* new_page = (EulerPage*)malloc(sizeof(EulerPage));
    if (!new_page) return false;

    new_page->memory = (uint8_t*)malloc(page_bytes);
    if (!new_page->memory) {
        free(new_page);
        return false;
    }

    new_page->next_page = pool->page_head;
    pool->page_head = new_page;
    pool->total_allocated_bytes += page_bytes;

    // Link all new chunks into the free list
    for (size_t i = 0; i < pool->chunks_per_page; i++) {
        uint8_t* chunk_addr = new_page->memory + (i * pool->chunk_size);
        EulerFreeNode* node = (EulerFreeNode*)chunk_addr;
        node->next = pool->free_head;
        pool->free_head = node;
    }

    return true;
}

/**
 * O(1) constant time allocation with automatic page expansion for heavy workloads.
 */
static inline void* euler_alloc(EulerPool* pool) {
    if (pool->free_head == NULL) {
        // Expand pool dynamically instead of hard-crashing on limits
        if (!euler_pool_grow(pool)) {
            assert(0 && "Euler Pool Critical: Failed to allocate new memory page.");
            return NULL;
        }
    }

    EulerFreeNode* node = pool->free_head;
    pool->free_head = pool->free_head->next;

    memset(node, 0, pool->chunk_size);
    return (void*)node;
}

/**
 * O(1) constant time deallocation.
 */
static inline void euler_free(EulerPool* pool, void* ptr) {
    if (ptr == NULL) return;

    EulerFreeNode* node = (EulerFreeNode*)ptr;
    node->next = pool->free_head;
    pool->free_head = node;
}

/**
 * Cleans up all dynamically allocated pages on application exit.
 */
static inline void euler_pool_destroy(EulerPool* pool) {
    EulerPage* current = pool->page_head;
    while (current != NULL) {
        EulerPage* next = current->next_page;
        free(current->memory);
        free(current);
        current = next;
    }
    pool->page_head = NULL;
    pool->free_head = NULL;
    pool->total_allocated_bytes = 0;
}

#endif // EULER_POOL_H
