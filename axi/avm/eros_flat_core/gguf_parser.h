#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// =====================================================================
// GGUF v3 PARSER: Zero-Copy Tensor Extraction from Memory-Mapped GGUF
// Architecture: Reads directly from the EulerPool / MapViewOfFile pointer.
// No allocation for tensor data — everything is a pointer into the map.
// =====================================================================

// GGUF value types
enum {
    GGUF_TYPE_UINT8   = 0,  GGUF_TYPE_INT8    = 1,
    GGUF_TYPE_UINT16  = 2,  GGUF_TYPE_INT16   = 3,
    GGUF_TYPE_UINT32  = 4,  GGUF_TYPE_INT32   = 5,
    GGUF_TYPE_FLOAT32 = 6,  GGUF_TYPE_BOOL    = 7,
    GGUF_TYPE_STRING  = 8,  GGUF_TYPE_ARRAY   = 9,
    GGUF_TYPE_UINT64  = 10, GGUF_TYPE_INT64   = 11,
    GGUF_TYPE_FLOAT64 = 12
};

// GGML tensor quantization types
enum {
    GGML_TYPE_F32     = 0,  GGML_TYPE_F16     = 1,
    GGML_TYPE_Q4_0    = 2,  GGML_TYPE_Q4_1    = 3,
    GGML_TYPE_Q5_0    = 6,  GGML_TYPE_Q5_1    = 7,
    GGML_TYPE_Q8_0    = 8,  GGML_TYPE_Q8_1    = 9,
    GGML_TYPE_Q2_K    = 10, GGML_TYPE_Q3_K    = 11,
    GGML_TYPE_Q4_K    = 12, GGML_TYPE_Q5_K    = 13,
    GGML_TYPE_Q6_K    = 14, GGML_TYPE_IQ2_XXS = 16,
    GGML_TYPE_IQ2_XS  = 17, GGML_TYPE_IQ3_XXS = 18,
    GGML_TYPE_IQ1_S   = 19, GGML_TYPE_IQ4_NL  = 20,
    GGML_TYPE_IQ3_S   = 21, GGML_TYPE_IQ2_S   = 22,
    GGML_TYPE_IQ4_XS  = 23, GGML_TYPE_I8      = 24,
    GGML_TYPE_I16     = 25, GGML_TYPE_I32     = 26,
    GGML_TYPE_I64     = 27, GGML_TYPE_F64     = 28,
    GGML_TYPE_IQ1_M   = 29, GGML_TYPE_BF16    = 30
};

// Block size and type size for quantized formats
static inline int ggml_blck_size(int type) {
    switch (type) {
        case GGML_TYPE_F32:  return 1;
        case GGML_TYPE_F16:  return 1;
        case GGML_TYPE_Q4_0: return 32;
        case GGML_TYPE_Q4_1: return 32;
        case GGML_TYPE_Q8_0: return 32;
        case GGML_TYPE_Q4_K: return 256;
        case GGML_TYPE_Q5_K: return 256;
        case GGML_TYPE_Q6_K: return 256;
        case GGML_TYPE_BF16: return 1;
        default: return 1;
    }
}

static inline size_t ggml_type_size(int type) {
    switch (type) {
        case GGML_TYPE_F32:  return 4;
        case GGML_TYPE_F16:  return 2;
        case GGML_TYPE_Q4_0: return 18;    // 2 + 16 bytes per 32 elements
        case GGML_TYPE_Q4_1: return 20;
        case GGML_TYPE_Q8_0: return 34;    // 2 + 32 bytes per 32 elements
        case GGML_TYPE_Q4_K: return 144;   // per 256 elements
        case GGML_TYPE_Q5_K: return 176;
        case GGML_TYPE_Q6_K: return 210;
        case GGML_TYPE_BF16: return 2;
        default: return 0;
    }
}

// ---------------------------------------------------------
// FP16 <-> FP32 Conversion
// ---------------------------------------------------------
static inline float fp16_to_fp32(uint16_t h) {
    uint32_t sign = (uint32_t)(h >> 15) << 31;
    uint32_t exponent = (h >> 10) & 0x1F;
    uint32_t mantissa = h & 0x3FF;

    if (exponent == 0) {
        if (mantissa == 0) {
            uint32_t result = sign;
            float f; memcpy(&f, &result, 4); return f;
        }
        // Denormalized
        exponent = 1;
        while (!(mantissa & 0x400)) { mantissa <<= 1; exponent--; }
        mantissa &= 0x3FF;
        uint32_t result = sign | ((exponent + 127 - 15) << 23) | (mantissa << 13);
        float f; memcpy(&f, &result, 4); return f;
    } else if (exponent == 31) {
        uint32_t result = sign | 0x7F800000 | (mantissa << 13);
        float f; memcpy(&f, &result, 4); return f;
    }

    uint32_t result = sign | ((exponent + 127 - 15) << 23) | (mantissa << 13);
    float f; memcpy(&f, &result, 4); return f;
}

// ---------------------------------------------------------
// Q4_K Dequantization (the "M" in Q4_K_M is a quality preset, format is Q4_K)
// Block: 256 elements = 144 bytes
// Layout: fp16 d, fp16 dmin, uint8_t scales[12], uint8_t qs[128]
// ---------------------------------------------------------
static inline void dequantize_q4_k(const void* block_ptr, float* out, int n_elements) {
    int n_blocks = n_elements / 256;
    const uint8_t* ptr = (const uint8_t*)block_ptr;

    for (int b = 0; b < n_blocks; b++) {
        uint16_t d_raw, dmin_raw;
        memcpy(&d_raw,    ptr + 0, 2);
        memcpy(&dmin_raw, ptr + 2, 2);
        float d    = fp16_to_fp32(d_raw);
        float dmin = fp16_to_fp32(dmin_raw);

        const uint8_t* scales = ptr + 4;
        const uint8_t* qs     = ptr + 16;

        // Decode the 12 bytes of scales into 8 scale/min pairs
        // Each sub-block is 32 elements. 256 / 32 = 8 sub-blocks.
        uint8_t sc[8], mn[8];
        for (int i = 0; i < 4; i++) {
            sc[i]     = scales[i] & 0x3F;
            sc[i + 4] = (scales[i + 4] & 0x3F) | ((scales[i + 8] & 0x0F) << 4);
            // Simplified: extract min nibbles
            mn[i]     = scales[i] >> 6;
            mn[i + 4] = scales[i + 4] >> 6;
        }

        for (int sb = 0; sb < 8; sb++) {
            float scale = d * sc[sb];
            float min_val = dmin * mn[sb];
            const uint8_t* q = qs + sb * 16;

            for (int j = 0; j < 16; j++) {
                out[b * 256 + sb * 32 + j]      = scale * (float)(q[j] & 0x0F) - min_val;
                out[b * 256 + sb * 32 + j + 16]  = scale * (float)(q[j] >> 4)   - min_val;
            }
        }

        ptr += 144;
    }
}

// ---------------------------------------------------------
// Q8_0 Dequantization (simpler, used for some tensors)
// Block: 32 elements = 34 bytes (fp16 d + 32 int8 quants)
// ---------------------------------------------------------
static inline void dequantize_q8_0(const void* block_ptr, float* out, int n_elements) {
    int n_blocks = n_elements / 32;
    const uint8_t* ptr = (const uint8_t*)block_ptr;

    for (int b = 0; b < n_blocks; b++) {
        uint16_t d_raw;
        memcpy(&d_raw, ptr, 2);
        float d = fp16_to_fp32(d_raw);
        const int8_t* qs = (const int8_t*)(ptr + 2);

        for (int j = 0; j < 32; j++) {
            out[b * 32 + j] = d * (float)qs[j];
        }
        ptr += 34;
    }
}

// ---------------------------------------------------------
// GGUF Tensor Info (parsed from the file header)
// ---------------------------------------------------------
#define GGUF_MAX_TENSORS 512
#define GGUF_MAX_NAME    128

typedef struct {
    char     name[GGUF_MAX_NAME];
    uint32_t n_dims;
    uint64_t dims[4];
    uint32_t type;      // quantization type
    uint64_t offset;    // offset from start of tensor data section
    uint64_t n_elements;
} GGUFTensorInfo;

// ---------------------------------------------------------
// GGUF Model Config (extracted from metadata)
// ---------------------------------------------------------
typedef struct {
    uint32_t n_layers;
    uint32_t n_heads;
    uint32_t n_kv_heads;
    uint32_t hidden_size;
    uint32_t intermediate_size;
    uint32_t vocab_size;
    uint32_t context_length;
    uint32_t embedding_length;
    uint32_t head_dim;
    float    rope_freq_base;
    float    layer_norm_eps;
    char     architecture[64];
    char     model_type[64];
} GGUFModelConfig;

// ---------------------------------------------------------
// GGUF Vocabulary Entry
// ---------------------------------------------------------
#define GGUF_MAX_VOCAB  65536
#define GGUF_MAX_TOKEN  256

typedef struct {
    char  text[GGUF_MAX_TOKEN];
    float score;
    int   type; // 1=normal, 2=unknown, 3=control, 4=user_defined, 5=unused, 6=byte
} GGUFVocabEntry;

// ---------------------------------------------------------
// Parsed GGUF Model (the master struct)
// ---------------------------------------------------------
typedef struct {
    // Raw memory map
    void*    base_ptr;
    uint64_t file_size;

    // Header
    uint32_t version;
    uint64_t n_tensors;
    uint64_t n_kv;

    // Parsed data
    GGUFModelConfig   config;
    GGUFTensorInfo    tensors[GGUF_MAX_TENSORS];
    uint64_t          tensor_data_offset; // absolute offset where tensor data begins

    // Vocabulary
    GGUFVocabEntry    vocab[GGUF_MAX_VOCAB];
    uint32_t          vocab_size;
} GGUFModel;

// ---------------------------------------------------------
// GGUF CURSOR: Tracks read position in the memory map
// ---------------------------------------------------------
typedef struct {
    const uint8_t* base;
    uint64_t       pos;
    uint64_t       size;
} GGUFCursor;

static inline uint8_t  cursor_u8(GGUFCursor* c)  { uint8_t  v; memcpy(&v, c->base + c->pos, 1); c->pos += 1; return v; }
static inline uint16_t cursor_u16(GGUFCursor* c) { uint16_t v; memcpy(&v, c->base + c->pos, 2); c->pos += 2; return v; }
static inline uint32_t cursor_u32(GGUFCursor* c) { uint32_t v; memcpy(&v, c->base + c->pos, 4); c->pos += 4; return v; }
static inline int32_t  cursor_i32(GGUFCursor* c) { int32_t  v; memcpy(&v, c->base + c->pos, 4); c->pos += 4; return v; }
static inline uint64_t cursor_u64(GGUFCursor* c) { uint64_t v; memcpy(&v, c->base + c->pos, 8); c->pos += 8; return v; }
static inline float    cursor_f32(GGUFCursor* c) { float    v; memcpy(&v, c->base + c->pos, 4); c->pos += 4; return v; }

static inline void cursor_str(GGUFCursor* c, char* out, int max_len) {
    uint64_t len = cursor_u64(c);
    int copy_len = (len < (uint64_t)(max_len - 1)) ? (int)len : (max_len - 1);
    memcpy(out, c->base + c->pos, copy_len);
    out[copy_len] = '\0';
    c->pos += len;
}

// Skip a GGUF value (used when we don't care about a metadata key)
static void cursor_skip_value(GGUFCursor* c, uint32_t type) {
    switch (type) {
        case GGUF_TYPE_UINT8:   c->pos += 1; break;
        case GGUF_TYPE_INT8:    c->pos += 1; break;
        case GGUF_TYPE_UINT16:  c->pos += 2; break;
        case GGUF_TYPE_INT16:   c->pos += 2; break;
        case GGUF_TYPE_UINT32:  c->pos += 4; break;
        case GGUF_TYPE_INT32:   c->pos += 4; break;
        case GGUF_TYPE_FLOAT32: c->pos += 4; break;
        case GGUF_TYPE_FLOAT64: c->pos += 8; break;
        case GGUF_TYPE_BOOL:    c->pos += 1; break;
        case GGUF_TYPE_UINT64:  c->pos += 8; break;
        case GGUF_TYPE_INT64:   c->pos += 8; break;
        case GGUF_TYPE_STRING:  {
            uint64_t len = cursor_u64(c);
            c->pos += len;
            break;
        }
        case GGUF_TYPE_ARRAY: {
            uint32_t arr_type = cursor_u32(c);
            uint64_t arr_len  = cursor_u64(c);
            for (uint64_t i = 0; i < arr_len; i++) {
                cursor_skip_value(c, arr_type);
            }
            break;
        }
    }
}

// ---------------------------------------------------------
// MAIN PARSER: Parse a memory-mapped GGUF file
// ---------------------------------------------------------
static int gguf_parse(void* mapped_ptr, uint64_t file_size, GGUFModel* model) {
    memset(model, 0, sizeof(GGUFModel));
    model->base_ptr  = mapped_ptr;
    model->file_size = file_size;

    GGUFCursor c = { (const uint8_t*)mapped_ptr, 0, file_size };

    // 1. HEADER
    uint32_t magic = cursor_u32(&c);
    if (magic != 0x46554747) { // "GGUF"
        printf("[GGUF PARSER] Invalid magic: 0x%08X\n", magic);
        return 0;
    }

    model->version   = cursor_u32(&c);
    model->n_tensors = cursor_u64(&c);
    model->n_kv      = cursor_u64(&c);

    printf("[GGUF PARSER] Version: %u | Tensors: %llu | Metadata KV: %llu\n",
           model->version, (unsigned long long)model->n_tensors, (unsigned long long)model->n_kv);

    // 2. METADATA KV PAIRS
    for (uint64_t i = 0; i < model->n_kv; i++) {
        char key[256] = {0};
        cursor_str(&c, key, sizeof(key));
        uint32_t vtype = cursor_u32(&c);

        // Extract architecture parameters we care about
        if (strcmp(key, "general.architecture") == 0 && vtype == GGUF_TYPE_STRING) {
            cursor_str(&c, model->config.architecture, sizeof(model->config.architecture));
        }
        else if (strcmp(key, "general.name") == 0 && vtype == GGUF_TYPE_STRING) {
            cursor_str(&c, model->config.model_type, sizeof(model->config.model_type));
        }
        else if (strstr(key, ".block_count") && vtype == GGUF_TYPE_UINT32) {
            model->config.n_layers = cursor_u32(&c);
        }
        else if (strstr(key, ".attention.head_count\"") == NULL && strstr(key, ".attention.head_count_kv") && vtype == GGUF_TYPE_UINT32) {
            model->config.n_kv_heads = cursor_u32(&c);
        }
        else if (strstr(key, ".attention.head_count") && !strstr(key, "kv") && vtype == GGUF_TYPE_UINT32) {
            model->config.n_heads = cursor_u32(&c);
        }
        else if (strstr(key, ".embedding_length") && vtype == GGUF_TYPE_UINT32) {
            model->config.hidden_size = cursor_u32(&c);
            model->config.embedding_length = model->config.hidden_size;
        }
        else if (strstr(key, ".feed_forward_length") && vtype == GGUF_TYPE_UINT32) {
            model->config.intermediate_size = cursor_u32(&c);
        }
        else if (strstr(key, ".context_length") && vtype == GGUF_TYPE_UINT32) {
            model->config.context_length = cursor_u32(&c);
        }
        else if (strstr(key, ".attention.layer_norm_epsilon") && vtype == GGUF_TYPE_FLOAT32) {
            model->config.layer_norm_eps = cursor_f32(&c);
        }
        else if (strstr(key, ".rope.freq_base") && vtype == GGUF_TYPE_FLOAT32) {
            model->config.rope_freq_base = cursor_f32(&c);
        }
        // Extract vocabulary
        else if (strcmp(key, "tokenizer.ggml.tokens") == 0 && vtype == GGUF_TYPE_ARRAY) {
            uint32_t arr_type = cursor_u32(&c);
            uint64_t arr_len  = cursor_u64(&c);
            model->vocab_size = (arr_len < GGUF_MAX_VOCAB) ? (uint32_t)arr_len : GGUF_MAX_VOCAB;
            model->config.vocab_size = model->vocab_size;
            for (uint64_t j = 0; j < arr_len; j++) {
                if (j < GGUF_MAX_VOCAB && arr_type == GGUF_TYPE_STRING) {
                    cursor_str(&c, model->vocab[j].text, GGUF_MAX_TOKEN);
                } else {
                    cursor_skip_value(&c, arr_type);
                }
            }
        }
        else if (strcmp(key, "tokenizer.ggml.scores") == 0 && vtype == GGUF_TYPE_ARRAY) {
            uint32_t arr_type = cursor_u32(&c);
            uint64_t arr_len  = cursor_u64(&c);
            for (uint64_t j = 0; j < arr_len; j++) {
                if (j < GGUF_MAX_VOCAB && arr_type == GGUF_TYPE_FLOAT32) {
                    model->vocab[j].score = cursor_f32(&c);
                } else {
                    cursor_skip_value(&c, arr_type);
                }
            }
        }
        else if (strcmp(key, "tokenizer.ggml.token_type") == 0 && vtype == GGUF_TYPE_ARRAY) {
            uint32_t arr_type = cursor_u32(&c);
            uint64_t arr_len  = cursor_u64(&c);
            for (uint64_t j = 0; j < arr_len; j++) {
                if (j < GGUF_MAX_VOCAB && arr_type == GGUF_TYPE_INT32) {
                    model->vocab[j].type = cursor_i32(&c);
                } else {
                    cursor_skip_value(&c, arr_type);
                }
            }
        }
        else {
            cursor_skip_value(&c, vtype);
        }
    }

    // Derive head_dim
    if (model->config.n_heads > 0) {
        model->config.head_dim = model->config.hidden_size / model->config.n_heads;
    }
    // Default rope freq
    if (model->config.rope_freq_base == 0.0f) {
        model->config.rope_freq_base = 10000.0f;
    }
    if (model->config.layer_norm_eps == 0.0f) {
        model->config.layer_norm_eps = 1e-5f;
    }

    printf("[GGUF PARSER] Architecture: %s (%s)\n", model->config.architecture, model->config.model_type);
    printf("[GGUF PARSER] Layers: %u | Heads: %u (KV: %u) | Hidden: %u | FFN: %u | Vocab: %u\n",
           model->config.n_layers, model->config.n_heads, model->config.n_kv_heads,
           model->config.hidden_size, model->config.intermediate_size, model->config.vocab_size);

    // 3. TENSOR INFO TABLE
    if (model->n_tensors > GGUF_MAX_TENSORS) {
        printf("[GGUF PARSER] WARNING: %llu tensors exceeds max %d, truncating.\n",
               (unsigned long long)model->n_tensors, GGUF_MAX_TENSORS);
    }

    uint64_t n_parse = (model->n_tensors < GGUF_MAX_TENSORS) ? model->n_tensors : GGUF_MAX_TENSORS;
    for (uint64_t i = 0; i < n_parse; i++) {
        GGUFTensorInfo* t = &model->tensors[i];
        cursor_str(&c, t->name, GGUF_MAX_NAME);
        t->n_dims = cursor_u32(&c);
        t->n_elements = 1;
        for (uint32_t d = 0; d < t->n_dims; d++) {
            t->dims[d] = cursor_u64(&c);
            t->n_elements *= t->dims[d];
        }
        t->type   = cursor_u32(&c);
        t->offset = cursor_u64(&c);
    }
    // Skip remaining tensors if truncated
    for (uint64_t i = n_parse; i < model->n_tensors; i++) {
        char skip_name[256];
        cursor_str(&c, skip_name, sizeof(skip_name));
        uint32_t nd = cursor_u32(&c);
        for (uint32_t d = 0; d < nd; d++) cursor_u64(&c);
        cursor_u32(&c); cursor_u64(&c);
    }

    // 4. COMPUTE TENSOR DATA OFFSET (aligned to 32 bytes)
    uint64_t alignment = 32;
    model->tensor_data_offset = (c.pos + alignment - 1) & ~(alignment - 1);

    printf("[GGUF PARSER] Tensor data begins at offset: 0x%llX\n",
           (unsigned long long)model->tensor_data_offset);

    return 1;
}

// ---------------------------------------------------------
// TENSOR LOOKUP: Find a tensor by name and return a pointer to its raw data
// ---------------------------------------------------------
static inline const GGUFTensorInfo* gguf_find_tensor(const GGUFModel* model, const char* name) {
    uint64_t n = (model->n_tensors < GGUF_MAX_TENSORS) ? model->n_tensors : GGUF_MAX_TENSORS;
    for (uint64_t i = 0; i < n; i++) {
        if (strcmp(model->tensors[i].name, name) == 0) {
            return &model->tensors[i];
        }
    }
    return NULL;
}

// Get raw pointer to tensor data in the memory map
static inline const void* gguf_tensor_data(const GGUFModel* model, const GGUFTensorInfo* info) {
    return (const uint8_t*)model->base_ptr + model->tensor_data_offset + info->offset;
}

// Dequantize a tensor into a float buffer (caller must allocate)
static inline void gguf_dequantize(const GGUFModel* model, const GGUFTensorInfo* info, float* out) {
    const void* data = gguf_tensor_data(model, info);

    switch (info->type) {
        case GGML_TYPE_F32:
            memcpy(out, data, info->n_elements * sizeof(float));
            break;
        case GGML_TYPE_F16:
            for (uint64_t i = 0; i < info->n_elements; i++) {
                out[i] = fp16_to_fp32(((const uint16_t*)data)[i]);
            }
            break;
        case GGML_TYPE_Q4_K:
            dequantize_q4_k(data, out, (int)info->n_elements);
            break;
        case GGML_TYPE_Q8_0:
            dequantize_q8_0(data, out, (int)info->n_elements);
            break;
        default:
            printf("[GGUF DEQUANT] Unsupported type %u for tensor '%s'\n", info->type, info->name);
            memset(out, 0, info->n_elements * sizeof(float));
            break;
    }
}

// ---------------------------------------------------------
// TOKENIZER: Simple BPE encoding using the GGUF vocabulary
// ---------------------------------------------------------
// Encode a single byte as a token (fallback for unknown characters)
static inline int gguf_byte_token(const GGUFModel* model, uint8_t byte) {
    char target[16];
    snprintf(target, sizeof(target), "<0x%02X>", byte);
    for (uint32_t i = 0; i < model->vocab_size; i++) {
        if (strcmp(model->vocab[i].text, target) == 0) return (int)i;
    }
    return -1;
}

// Find a token by exact text match
static inline int gguf_find_token(const GGUFModel* model, const char* text, int len) {
    for (uint32_t i = 0; i < model->vocab_size; i++) {
        if ((int)strlen(model->vocab[i].text) == len &&
            memcmp(model->vocab[i].text, text, len) == 0) {
            return (int)i;
        }
    }
    return -1;
}

// Simple greedy BPE tokenizer (encode text to token IDs)
static int gguf_tokenize(const GGUFModel* model, const char* text, int* tokens, int max_tokens) {
    int n_tokens = 0;
    int text_len = (int)strlen(text);
    int pos = 0;

    while (pos < text_len && n_tokens < max_tokens) {
        // Greedy: try longest match first
        int best_len = 0;
        int best_id  = -1;

        // Try decreasing lengths
        int max_try = text_len - pos;
        if (max_try > GGUF_MAX_TOKEN - 1) max_try = GGUF_MAX_TOKEN - 1;

        for (int try_len = max_try; try_len >= 1; try_len--) {
            int id = gguf_find_token(model, text + pos, try_len);
            if (id >= 0) {
                best_len = try_len;
                best_id  = id;
                break;
            }
        }

        if (best_id >= 0) {
            tokens[n_tokens++] = best_id;
            pos += best_len;
        } else {
            // Fallback: encode as byte token
            int byte_id = gguf_byte_token(model, (uint8_t)text[pos]);
            if (byte_id >= 0) {
                tokens[n_tokens++] = byte_id;
            }
            pos++;
        }
    }

    return n_tokens;
}

// Detokenize: convert token IDs back to text
static int gguf_detokenize(const GGUFModel* model, const int* tokens, int n_tokens, char* out, int max_len) {
    int written = 0;
    for (int i = 0; i < n_tokens && written < max_len - 1; i++) {
        if (tokens[i] >= 0 && tokens[i] < (int)model->vocab_size) {
            const char* piece = model->vocab[tokens[i]].text;
            int piece_len = (int)strlen(piece);

            // Handle byte tokens like <0xAB>
            if (piece_len == 6 && piece[0] == '<' && piece[1] == '0' && piece[2] == 'x') {
                unsigned int byte_val;
                if (sscanf(piece, "<0x%02X>", &byte_val) == 1) {
                    out[written++] = (char)byte_val;
                    continue;
                }
            }

            int copy = (piece_len < max_len - written - 1) ? piece_len : (max_len - written - 1);
            memcpy(out + written, piece, copy);
            written += copy;
        }
    }
    out[written] = '\0';
    return written;
}
