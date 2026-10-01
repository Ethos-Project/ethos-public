# eros_render_loop.axi
import node.graphics.canvas
import console_ui

# 1. Initialize Raylib window loop
canvas.init_window(1280, 720)

# Mock state initialization
state console_state_view_mode: int = 0
state console_state_is_output_expanded: bool = false
state console_state_input_buffer: char[256] = {0}
state input_len: int = 0


while sys.window_should_close() == false:
    # --- Input Handling ---
    state key: int = canvas.get_char_pressed()
    while key > 0:
        if key >= 32:
            if key <= 125:
                if input_len < 255:
                    console_state_input_buffer[input_len] = key
                    input_len = input_len + 1
                    console_state_input_buffer[input_len] = 0
        key = canvas.get_char_pressed()

    state kb_key: int = canvas.get_key_pressed()
    while kb_key > 0:
        if kb_key == 259:
            if input_len > 0:
                input_len = input_len - 1
                console_state_input_buffer[input_len] = 0
        if kb_key == 257:
            if console_state_is_output_expanded == false:
                console_state_is_output_expanded = true
            else:
                console_state_is_output_expanded = false
            input_len = 0
            console_state_input_buffer[input_len] = 0
        kb_key = canvas.get_key_pressed()

    # Input Handling (Mock)
    mouse_x = canvas.get_mouse_x()
    mouse_y = canvas.get_mouse_y()
    is_left_down = canvas.is_mouse_down(0)

    canvas.begin_drawing()
    canvas.clear_background()

    # 3. Draw the "Bar Mode" or "Window Mode" layout
    if console_state_view_mode == 0:
        # --- BAR MODE ---
        state bar_w: int = 580
        state bar_h: int = 54
        state bar_x: int = 350
        state bar_y: int = 646

        # Draw expanded output if needed
        if console_state_is_output_expanded:
            state out_h: int = 200
            state out_y: int = bar_y - 12 - out_h
            # Background
            canvas.draw_rectangle_rounded({bar_x, out_y, bar_w, out_h}, 0.1, 16, {253, 246, 238, 242})
            # Border
            canvas.draw_rectangle_rounded_lines({bar_x, out_y, bar_w, out_h}, 0.1, 16, 1.0, {120, 80, 40, 33})

            # Header line
            canvas.draw_text("OUTPUT", bar_x + 16, out_y + 10, 10, {160, 120, 80, 255})
            canvas.draw_text("5 entries", bar_x + bar_w - 60, out_y + 10, 10, {192, 168, 128, 255})

        # Draw the bottom bar
        # Background
        canvas.draw_rectangle_rounded({bar_x, bar_y, bar_w, bar_h}, 0.5, 32, {253, 246, 238, 234})
        # Border
        canvas.draw_rectangle_rounded_lines({bar_x, bar_y, bar_w, bar_h}, 0.5, 32, 1.0, {120, 80, 40, 38})

        # Monogram
        canvas.draw_circle(bar_x + 32, bar_y + 27, 16.0, {200, 50, 50, 25})
        canvas.draw_text("N", bar_x + 27, bar_y + 20, 16, {200, 50, 50, 255})

        # Prompt ~
        canvas.draw_text("~", bar_x + 60, bar_y + 20, 14, {192, 168, 128, 255})

        # Input Text
        if input_len == 0:
            canvas.draw_text("enter command...", bar_x + 80, bar_y + 18, 16, {160, 120, 80, 255})
        else:
            canvas.draw_text(console_state_input_buffer, bar_x + 80, bar_y + 18, 16, {44, 26, 14, 255})

    else:
        # --- WINDOW MODE ---
        state win_w: int = 740
        state win_h: int = 540
        state win_x: int = 270
        state win_y: int = 90

        # Window Background
        canvas.draw_rect(win_x, win_y, win_w, win_h, {253, 246, 238, 247})
        # Window Border
        canvas.draw_rectangle_lines_ex({win_x, win_y, win_w, win_h}, 1.0, {120, 80, 40, 35})

        # Title Bar Area (mocking standard height)
        canvas.draw_text("Console", win_x + 20, win_y + 15, 14, {44, 26, 14, 255})

        # Messages Area
        canvas.draw_text("... Message History ...", win_x + 24, win_y + 60, 14, {192, 168, 128, 255})

        # Status bar (above input)
        state status_h: int = 28
        state input_h: int = 50
        state status_y: int = win_y + win_h - input_h - status_h

        canvas.draw_rect(win_x, status_y, win_w, status_h, {240, 232, 220, 153})
        canvas.draw_rectangle_lines_ex({win_x, status_y, win_w, 1.0}, 1.0, {120, 80, 40, 25})
        canvas.draw_text("0 entries", win_x + 20, status_y + 8, 12, {192, 168, 128, 255})
        canvas.draw_text("console ~", win_x + 100, status_y + 8, 12, {192, 168, 128, 255})

        # Input Area (Bottom of window)
        state input_y: int = win_y + win_h - input_h
        canvas.draw_rect(win_x, input_y, win_w, input_h, {247, 239, 228, 178})
        canvas.draw_rectangle_lines_ex({win_x, input_y, win_w, 1.0}, 1.0, {120, 80, 40, 25})

        # Prompt and Input Text
        canvas.draw_text("console ~", win_x + 20, input_y + 16, 14, {200, 50, 50, 255})
        if input_len == 0:
            canvas.draw_text("enter command...", win_x + 100, input_y + 16, 16, {160, 120, 80, 255})
        else:
            canvas.draw_text(console_state_input_buffer, win_x + 100, input_y + 16, 16, {44, 26, 14, 255})

    canvas.end_drawing()

canvas.close_window()
