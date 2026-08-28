#ifndef RENDERING_C
#define RENDERING_C

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    bool is_vsync_enabled;
    void* native_window_handle;
} AxiomViewport;

// Event structure for native offline input handling
typedef enum {
    AXIOM_EVENT_NONE = 0,
    AXIOM_EVENT_KEY_DOWN,
    AXIOM_EVENT_MOUSE_CLICK,
    AXIOM_EVENT_WINDOW_CLOSE
} AxiomEventType;

typedef struct {
    AxiomEventType type;
    uint32_t key_code;
    float mouse_x;
    float mouse_y;
} AxiomEvent;

bool axiom_rendering_init(AxiomViewport* viewport, uint32_t width, uint32_t height, const char* title) {
    viewport->width = width;
    viewport->height = height;
    viewport->is_vsync_enabled = true;
    viewport->native_window_handle = NULL;
    return true;
}

void axiom_rendering_clear(uint32_t hex_color) {
    // Clear frame-buffer
}

void axiom_rendering_draw_box(float x, float y, float w, float h, uint32_t fill_color) {
    // Push geometry to GPU pipeline
}

/**
 * Polls native OS events directly without blocking or dependencies on heavy GUI frameworks.
 */
bool axiom_rendering_poll_event(AxiomEvent* out_event) {
    // Stubbed for direct OS event loop hook (Win32 PeekMessage / X11 XPending)
    out_event->type = AXIOM_EVENT_NONE;
    return false;
}

void axiom_rendering_present(AxiomViewport* viewport) {
    // Swap buffers
}

#endif // RENDERING_C
