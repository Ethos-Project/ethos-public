// core::graphics
// Raylib FFI Bindings for Axi

struct Color:
    i32 r
    i32 g
    i32 b
    i32 a

@C_Native
HOW TO init_window(width: i32, height: i32, title: str):
    InitWindow(width, height, title)

@C_Native
HOW TO window_should_close() -> i32:
    return WindowShouldClose()

@C_Native
HOW TO begin_drawing():
    BeginDrawing()

@C_Native
HOW TO end_drawing():
    EndDrawing()

@C_Native
HOW TO clear_background(r: i32, g: i32, b: i32, a: i32):
    Color c = { (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a };
    ClearBackground(c)

@C_Native
HOW TO draw_text(text: str, x: i32, y: i32, font_size: i32, r: i32, g: i32, b: i32, a: i32):
    Color c = { (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a };
    DrawText(text, x, y, font_size, c)

@C_Native
HOW TO handle_terminal_input():

@C_Native
HOW TO get_terminal_input() -> str:

@C_Native
HOW TO close_window():
    CloseWindow()
