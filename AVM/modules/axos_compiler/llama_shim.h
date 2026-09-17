#ifndef LLAMA_SHIM_H
#define LLAMA_SHIM_H
#include <stdbool.h>
typedef struct llama_model llama_model;
typedef struct llama_context llama_context;

inline void llama_backend_init(void) {}
inline llama_model* llama_load_model_from_file(const char* path_model, int params) { return (llama_model*)1; }
inline llama_context* llama_new_context_with_model(llama_model* model, int params) { return (llama_context*)1; }
inline int llama_decode(llama_context* ctx, void* batch) { return 0; }
inline void llama_backend_free(void) {}
#endif
