#ifndef AXI_TERMINAL_H
#define AXI_TERMINAL_H

#include <stddef.h>

#if defined(_WIN32) && defined(AXI_TERMINAL_BUILD)
#define AXI_TERMINAL_API __declspec(dllexport)
#else
#define AXI_TERMINAL_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AxiTerminal AxiTerminal;

AXI_TERMINAL_API AxiTerminal *axi_terminal_create(void);
AXI_TERMINAL_API void axi_terminal_destroy(AxiTerminal *terminal);

AXI_TERMINAL_API void axi_terminal_insert_text(AxiTerminal *terminal, const char *text);
AXI_TERMINAL_API void axi_terminal_backspace(AxiTerminal *terminal);
AXI_TERMINAL_API void axi_terminal_history_previous(AxiTerminal *terminal);
AXI_TERMINAL_API void axi_terminal_history_next(AxiTerminal *terminal);
AXI_TERMINAL_API int axi_terminal_submit(AxiTerminal *terminal);

AXI_TERMINAL_API const char *axi_terminal_prompt(const AxiTerminal *terminal);
AXI_TERMINAL_API size_t axi_terminal_line_count(const AxiTerminal *terminal);
AXI_TERMINAL_API const char *axi_terminal_line_at(const AxiTerminal *terminal, size_t index);

// --- Axiom ABI Wrappers ---
#include "raylib.h"
#include <stdint.h>

static AxiTerminal* global_terminal = NULL;

static inline void term_init() {
    if (!global_terminal) global_terminal = axi_terminal_create();
}

static inline void term_update() {
    if (!global_terminal) return;
    
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32) && (key <= 125)) {
            char str[2] = { (char)key, '\0' };
            axi_terminal_insert_text(global_terminal, str);
        }
        key = GetCharPressed();
    }
    
    if (IsKeyPressed(KEY_BACKSPACE)) {
        axi_terminal_backspace(global_terminal);
    }
    
    if (IsKeyPressed(KEY_ENTER)) {
        axi_terminal_submit(global_terminal);
    }
    
    if (IsKeyPressed(KEY_UP)) {
        axi_terminal_history_previous(global_terminal);
    }
    
    if (IsKeyPressed(KEY_DOWN)) {
        axi_terminal_history_next(global_terminal);
    }
}

static inline const char* term_get_prompt() {
    return global_terminal ? axi_terminal_prompt(global_terminal) : "";
}

static inline int32_t term_get_line_count() {
    return global_terminal ? (int32_t)axi_terminal_line_count(global_terminal) : 0;
}

static inline const char* term_get_line(int32_t index) {
    return global_terminal ? axi_terminal_line_at(global_terminal, (size_t)index) : "";
}

#ifdef __cplusplus
}
#endif

#endif
