/* ====================================================================
 * axi GENERATED ABI BOUNDARY
 * STRICT COMPLIANCE: EULER_POOL STATIC ALLOCATION & UINT64 WRAPPING
 * MEMORY FLATTENING: Arrays and Maps are flattened into static limits
 * ==================================================================== */

#ifndef axi_GENERATED_ABI_H
#define axi_GENERATED_ABI_H

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t CommitmentStatus;
enum CommitmentStatus_Variants {
    CommitmentStatus_Observed = 1,
    CommitmentStatus_InferredNotAsserted = 2,
    CommitmentStatus_Rejected = 3,
};

typedef enum {
    AgentStateTag_Idle,
    AgentStateTag_Running,
    AgentStateTag_Errored,
} AgentStateTag;

typedef struct {
    AgentStateTag tag;
    union {
        struct {
            uint32_t _0;
        } Running;
        struct {
            char reason[256];
            uint32_t code;
        } Errored;
    } data;
} AgentState;

typedef struct {
    uint32_t start_char;
    uint32_t end_char;
} TokenSpan;

typedef struct {
    TokenSpan raw_span;
    char assembler_module_id[256];
    char module_version[256];
    char rules_triggered[32][256];
    uint32_t rules_triggered_count;
    char upstream_dependencies[32][256];
    uint32_t upstream_dependencies_count;
    bool is_inferred;
} EvidenceLineage;

typedef struct {
    float hypothesis_confidence;
    float validation_confidence;
    CommitmentStatus commitment_status;
} EvidenceDisposition;

typedef struct {
    char signal_id[256];
    char module_id[256];
    char module_version[256];
    char evidence_kind[256];
    TokenSpan span;
    struct { char key[128]; uint64_t value; } payload[16];
    uint32_t payload_count;
    EvidenceDisposition disposition;
    EvidenceLineage lineage;
} CanonicalEvidenceCell;

typedef struct {
    char signal_id[256];
    char assembler_version[256];
    CanonicalEvidenceCell graphemic_evidence[32];
    uint32_t graphemic_evidence_count;
    CanonicalEvidenceCell morphological_evidence[32];
    uint32_t morphological_evidence_count;
    CanonicalEvidenceCell syntactic_evidence[32];
    uint32_t syntactic_evidence_count;
    CanonicalEvidenceCell semantic_evidence[32];
    uint32_t semantic_evidence_count;
    CanonicalEvidenceCell inferred_evidence[32];
    uint32_t inferred_evidence_count;
    char tamper_evident_digest[256];
} LinguisticEvidenceEnvelope;

/* Lock-Free IPC Ring Buffer for TokenSpan */
typedef struct {
    uint64_t producer_sequence;
    uint64_t consumer_sequence;
    uint32_t capacity;
    TokenSpan* euler_pool_buffer; // Must map to static memory
} TokenSpanQueue;

static inline bool TokenSpanQueue_push(TokenSpanQueue* q, TokenSpan item) {
    uint64_t distance = (uint64_t)(q->producer_sequence - q->consumer_sequence);
    if (distance >= q->capacity) return false; // Queue Full
    q->euler_pool_buffer[q->producer_sequence % q->capacity] = item;
    q->producer_sequence++; // Unsigned wrapping prevents overflow
    return true;
}

/* Lock-Free IPC Ring Buffer for EvidenceLineage */
typedef struct {
    uint64_t producer_sequence;
    uint64_t consumer_sequence;
    uint32_t capacity;
    EvidenceLineage* euler_pool_buffer; // Must map to static memory
} EvidenceLineageQueue;

static inline bool EvidenceLineageQueue_push(EvidenceLineageQueue* q, EvidenceLineage item) {
    uint64_t distance = (uint64_t)(q->producer_sequence - q->consumer_sequence);
    if (distance >= q->capacity) return false; // Queue Full
    q->euler_pool_buffer[q->producer_sequence % q->capacity] = item;
    q->producer_sequence++; // Unsigned wrapping prevents overflow
    return true;
}

/* Lock-Free IPC Ring Buffer for EvidenceDisposition */
typedef struct {
    uint64_t producer_sequence;
    uint64_t consumer_sequence;
    uint32_t capacity;
    EvidenceDisposition* euler_pool_buffer; // Must map to static memory
} EvidenceDispositionQueue;

static inline bool EvidenceDispositionQueue_push(EvidenceDispositionQueue* q, EvidenceDisposition item) {
    uint64_t distance = (uint64_t)(q->producer_sequence - q->consumer_sequence);
    if (distance >= q->capacity) return false; // Queue Full
    q->euler_pool_buffer[q->producer_sequence % q->capacity] = item;
    q->producer_sequence++; // Unsigned wrapping prevents overflow
    return true;
}

/* Lock-Free IPC Ring Buffer for CanonicalEvidenceCell */
typedef struct {
    uint64_t producer_sequence;
    uint64_t consumer_sequence;
    uint32_t capacity;
    CanonicalEvidenceCell* euler_pool_buffer; // Must map to static memory
} CanonicalEvidenceCellQueue;

static inline bool CanonicalEvidenceCellQueue_push(CanonicalEvidenceCellQueue* q, CanonicalEvidenceCell item) {
    uint64_t distance = (uint64_t)(q->producer_sequence - q->consumer_sequence);
    if (distance >= q->capacity) return false; // Queue Full
    q->euler_pool_buffer[q->producer_sequence % q->capacity] = item;
    q->producer_sequence++; // Unsigned wrapping prevents overflow
    return true;
}

/* Lock-Free IPC Ring Buffer for LinguisticEvidenceEnvelope */
typedef struct {
    uint64_t producer_sequence;
    uint64_t consumer_sequence;
    uint32_t capacity;
    LinguisticEvidenceEnvelope* euler_pool_buffer; // Must map to static memory
} LinguisticEvidenceEnvelopeQueue;

static inline bool LinguisticEvidenceEnvelopeQueue_push(LinguisticEvidenceEnvelopeQueue* q, LinguisticEvidenceEnvelope item) {
    uint64_t distance = (uint64_t)(q->producer_sequence - q->consumer_sequence);
    if (distance >= q->capacity) return false; // Queue Full
    q->euler_pool_buffer[q->producer_sequence % q->capacity] = item;
    q->producer_sequence++; // Unsigned wrapping prevents overflow
    return true;
}

#endif // axi_GENERATED_ABI_H
