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
} axiViewport;

// Event structure for native offline input handling
typedef enum {
    axi_EVENT_NONE = 0,
    axi_EVENT_KEY_DOWN,
    axi_EVENT_MOUSE_CLICK,
    axi_EVENT_WINDOW_CLOSE
} axiEventType;

typedef struct {
    axiEventType type;
    uint32_t key_code;
    float mouse_x;
    float mouse_y;
} axiEvent;

bool axi_rendering_init(axiViewport* viewport, uint32_t width, uint32_t height, const char* title) {
    viewport->width = width;
    viewport->height = height;
    viewport->is_vsync_enabled = true;
    viewport->native_window_handle = NULL;
    return true;
}

void axi_rendering_clear(uint32_t hex_color) {
    // Clear frame-buffer
}

void axi_rendering_draw_box(float x, float y, float w, float h, uint32_t fill_color) {
    // Push geometry to GPU pipeline
}

/**
 * Polls native OS events directly without blocking or dependencies on heavy GUI frameworks.
 */
bool axi_rendering_poll_event(axiEvent* out_event) {
    // Stubbed for direct OS event loop hook (Win32 PeekMessage / X11 XPending)
    out_event->type = axi_EVENT_NONE;
    return false;
}

void axi_rendering_present(axiViewport* viewport) {
    // Swap buffers
}

#endif // RENDERING_C
