/* ====================================================================
 * axi GENERATED ABI BOUNDARY
 * STRICT COMPLIANCE: EULER_POOL STATIC ALLOCATION & UINT64 WRAPPING
 * MEMORY FLATTENING: Arrays and Maps are flattened into static limits
 * ==================================================================== */

#ifndef axi_GENERATED_ABI_H
#define axi_GENERATED_ABI_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t file_id;
    uint64_t revision;
} QueryDb;

typedef struct {
    uint64_t symbol_hash;
    uint8_t param_count;
    bool is_pure;
    bool has_contracts;
} ItemSignature;

/* Lock-Free IPC Ring Buffer for QueryDb */
typedef struct {
    uint64_t producer_sequence;
    uint64_t consumer_sequence;
    uint32_t capacity;
    QueryDb* euler_pool_buffer; // Must map to static memory
} QueryDbQueue;

static inline bool QueryDbQueue_push(QueryDbQueue* q, QueryDb item) {
    uint64_t distance = (uint64_t)(q->producer_sequence - q->consumer_sequence);
    if (distance >= q->capacity) return false; // Queue Full
    q->euler_pool_buffer[q->producer_sequence % q->capacity] = item;
    q->producer_sequence++; // Unsigned wrapping prevents overflow
    return true;
}

/* Lock-Free IPC Ring Buffer for ItemSignature */
typedef struct {
    uint64_t producer_sequence;
    uint64_t consumer_sequence;
    uint32_t capacity;
    ItemSignature* euler_pool_buffer; // Must map to static memory
} ItemSignatureQueue;

static inline bool ItemSignatureQueue_push(ItemSignatureQueue* q, ItemSignature item) {
    uint64_t distance = (uint64_t)(q->producer_sequence - q->consumer_sequence);
    if (distance >= q->capacity) return false; // Queue Full
    q->euler_pool_buffer[q->producer_sequence % q->capacity] = item;
    q->producer_sequence++; // Unsigned wrapping prevents overflow
    return true;
}

/* ====================================================================
 * QUERY & COMPUTE KERNEL EXPORTS
 * ==================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

// @query (Memoized incrementally)
extern GreenNode parse_cst(const QueryDb* db, const uint8_t* source_bytes);

// @query (Memoized incrementally)
extern ItemSignature* extract_signatures(const QueryDb* db, const GreenNode* root);

// @query (Memoized incrementally)
extern bool verify_item_contracts(const QueryDb* db, uint64_t item_hash);

#ifdef __cplusplus
}
#endif

#endif // axi_GENERATED_ABI_H
