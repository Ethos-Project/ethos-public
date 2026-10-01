#ifndef GRAPHICS_NATIVE_H
#define GRAPHICS_NATIVE_H

#include "raylib.h"
#include <stdint.h>

static inline void init_window(int width, int height, const char* title) {
    InitWindow(width, height, title);
}

static inline int window_should_close() {
    return WindowShouldClose() ? 1 : 0;
}

static inline void begin_drawing() {
    BeginDrawing();
}

static inline void end_drawing() {
    EndDrawing();
}

static inline void clear_background(int r, int g, int b, int a) {
    Color c = { (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a };
    ClearBackground(c);
}

static inline void draw_text(const char* text, int x, int y, int size, int r, int g, int b, int a) {
    Color c = { (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a };
    DrawText(text, x, y, size, c);
}

static inline void close_window() {
    CloseWindow();
}

static char terminal_input_buffer[512] = {0};
static int terminal_input_len = 0;

static inline void handle_terminal_input() {
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32) && (key <= 125) && (terminal_input_len < 511)) {
            terminal_input_buffer[terminal_input_len] = (char)key;
            terminal_input_buffer[terminal_input_len+1] = '\0';
            terminal_input_len++;
        }
        key = GetCharPressed();
    }
    
    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (terminal_input_len > 0) {
            terminal_input_len--;
            terminal_input_buffer[terminal_input_len] = '\0';
        }
    }
}

static inline const char* get_terminal_input() {
    return terminal_input_buffer;
}

#endif
