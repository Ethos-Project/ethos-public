/* ====================================================================
 * axi GENERATED ABI BOUNDARY
 * STRICT COMPLIANCE: EULER_POOL STATIC ALLOCATION & UINT64 WRAPPING
 * MEMORY FLATTENING: Arrays and Maps are flattened into static limits
 * ==================================================================== */

#ifndef axi_GENERATED_ABI_H
#define axi_GENERATED_ABI_H

#include <stdint.h>
#include <stdbool.h>

typedef int ViewMode;
enum ViewMode_Variants {
    ViewMode_Bar,
    ViewMode_Window,
};

typedef int MessageType;
enum MessageType_Variants {
    MessageType_User,
    MessageType_AI,
    MessageType_System,
    MessageType_Error,
};

typedef struct {
    uint32_t id;
    MessageType msg_type;
    uint64_t timestamp;
    uint32_t content_ptr;
} Message;

typedef struct {
    uint32_t id;
    char label[256];
    uint32_t message_count;
    uint32_t messages_head;
} Session;

typedef struct {
    ViewMode view_mode;
    uint32_t active_session_id;
    uint32_t sessions_count;
    bool is_output_expanded;
    char input_buffer[256];
    int32_t history_idx;
} ConsoleState;

/* Lock-Free IPC Ring Buffer for Message */
typedef struct {
    uint64_t producer_sequence;
    uint64_t consumer_sequence;
    uint32_t capacity;
    Message* euler_pool_buffer; // Must map to static memory
} MessageQueue;

static inline bool MessageQueue_push(MessageQueue* q, Message item) {
    uint64_t distance = (uint64_t)(q->producer_sequence - q->consumer_sequence);
    if (distance >= q->capacity) return false; // Queue Full
    q->euler_pool_buffer[q->producer_sequence % q->capacity] = item;
    q->producer_sequence++; // Unsigned wrapping prevents overflow
    return true;
}

/* Lock-Free IPC Ring Buffer for Session */
typedef struct {
    uint64_t producer_sequence;
    uint64_t consumer_sequence;
    uint32_t capacity;
    Session* euler_pool_buffer; // Must map to static memory
} SessionQueue;

static inline bool SessionQueue_push(SessionQueue* q, Session item) {
    uint64_t distance = (uint64_t)(q->producer_sequence - q->consumer_sequence);
    if (distance >= q->capacity) return false; // Queue Full
    q->euler_pool_buffer[q->producer_sequence % q->capacity] = item;
    q->producer_sequence++; // Unsigned wrapping prevents overflow
    return true;
}

/* Lock-Free IPC Ring Buffer for ConsoleState */
typedef struct {
    uint64_t producer_sequence;
    uint64_t consumer_sequence;
    uint32_t capacity;
    ConsoleState* euler_pool_buffer; // Must map to static memory
} ConsoleStateQueue;

static inline bool ConsoleStateQueue_push(ConsoleStateQueue* q, ConsoleState item) {
    uint64_t distance = (uint64_t)(q->producer_sequence - q->consumer_sequence);
    if (distance >= q->capacity) return false; // Queue Full
    q->euler_pool_buffer[q->producer_sequence % q->capacity] = item;
    q->producer_sequence++; // Unsigned wrapping prevents overflow
    return true;
}

#endif // axi_GENERATED_ABI_H
