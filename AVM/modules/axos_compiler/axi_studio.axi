@C_Include("<raylib.h>")
@C_Include("<stdbool.h>")
@C_Include("<math.h>")
@C_Include("<stdio.h>")
@C_Include("<string.h>")
@C_Include("<dirent.h>")

@StaticArena
state is_running: bool = true

# ---------------------------------------------------------
# SPATIAL CANVAS STATE (inline — matches axi_studio_state)
# ---------------------------------------------------------
@StaticArena
state node_count: int = 0

@StaticArena
state node_ids: int[64] = {0}
@StaticArena
state node_labels: char[64*64] = {0}
@StaticArena
state node_x: float[64] = {0}
@StaticArena
state node_y: float[64] = {0}
@StaticArena
state node_w: float[64] = {0}
@StaticArena
state node_h: float[64] = {0}
@StaticArena
state node_active: bool[64] = {0}

# Camera pan state
@StaticArena
state cam_offset_x: float = 0.0
@StaticArena
state cam_offset_y: float = 0.0
@StaticArena
state cam_zoom: float = 1.0

# Drag state
@StaticArena
state drag_node_id: int = -1
@StaticArena
state drag_start_mouse_x: float = 0.0
@StaticArena
state drag_start_mouse_y: float = 0.0
@StaticArena
state drag_start_node_x: float = 0.0
@StaticArena
state drag_start_node_y: float = 0.0

# Wire drag
@StaticArena
state wire_src_id: int = -1

# Explorer sidebar
@StaticArena
state issue_names: char[120*80] = {0}
@StaticArena
state issue_count: int = 0
@StaticArena
state selected_issue: int = -1

# Terminal line
@StaticArena
state terminal_line: char[256] = {0}

# ---------------------------------------------------------
# INIT
# ---------------------------------------------------------
node init_studio() -> int:
    @C_Native
    """
    InitWindow(1280, 720, "axi Spatial Studio IDE v2");
    SetTargetFPS(60);

    // Seed a few spatial nodes to represent the live DAG
    node_count = 4;

    node_ids[0] = 0; strcpy(&node_labels[0*64], "domus_genesis.axi");
    node_x[0] = 320.0f; node_y[0] = 180.0f; node_w[0] = 200.0f; node_h[0] = 60.0f;

    node_ids[1] = 1; strcpy(&node_labels[1*64], "axi_studio.axi");
    node_x[1] = 620.0f; node_y[1] = 180.0f; node_w[1] = 200.0f; node_h[1] = 60.0f; node_active[1] = true;

    node_ids[2] = 2; strcpy(&node_labels[2*64], "ethos_server.axi");
    node_x[2] = 320.0f; node_y[2] = 320.0f; node_w[2] = 200.0f; node_h[2] = 60.0f;

    node_ids[3] = 3; strcpy(&node_labels[3*64], "compiler.axi");
    node_x[3] = 620.0f; node_y[3] = 320.0f; node_w[3] = 200.0f; node_h[3] = 60.0f;

    // Load real issue filenames from .axi/issues/
    issue_count = 0;
    DIR* dir = opendir("C:\\Antigravity\\cogni-core\\.axi\\issues");
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL && issue_count < 120) {
            char* dname = (*entry).d_name;
            if (dname[0] == '.') continue;
            strncpy(&issue_names[issue_count * 80], dname, 79);
            issue_count++;
        }
        closedir(dir);
    }

    strcpy(terminal_line, "axi_studio v2.0 loaded. DAG wired.");
    """

# ---------------------------------------------------------
# INPUT HANDLING
# ---------------------------------------------------------
node handle_input() -> int:
    @C_Native
    """
    if (WindowShouldClose()) { is_running = false; return; }

    Vector2 mouse = GetMousePosition();
    float world_mouse_x = (mouse.x - cam_offset_x) / cam_zoom;
    float world_mouse_y = (mouse.y - cam_offset_y) / cam_zoom;

    // --- Node drag ---
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        drag_node_id = -1;
        for (int i = node_count - 1; i >= 0; i--) {
            if (world_mouse_x >= node_x[i] && world_mouse_x <= node_x[i] + node_w[i] &&
                world_mouse_y >= node_y[i] && world_mouse_y <= node_y[i] + node_h[i]) {
                drag_node_id = i;
                drag_start_mouse_x = world_mouse_x;
                drag_start_mouse_y = world_mouse_y;
                drag_start_node_x = node_x[i];
                drag_start_node_y = node_y[i];
                // Mark active
                for (int j = 0; j < node_count; j++) node_active[j] = false;
                node_active[i] = true;
                break;
            }
        }
        // Explorer click
        if (mouse.x < 250) {
            int clicked = (int)((mouse.y - 60) / 22);
            if (clicked >= 0 && clicked < issue_count) {
                selected_issue = clicked;
                snprintf(terminal_line, 255, "> open %s", &issue_names[selected_issue * 80]);
            }
        }
    }

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && drag_node_id >= 0) {
        node_x[drag_node_id] = drag_start_node_x + (world_mouse_x - drag_start_mouse_x);
        node_y[drag_node_id] = drag_start_node_y + (world_mouse_y - drag_start_mouse_y);
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        drag_node_id = -1;
    }

    // --- Camera pan (right mouse drag) ---
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
        Vector2 delta = GetMouseDelta();
        cam_offset_x += delta.x;
        cam_offset_y += delta.y;
    }

    // --- Zoom (scroll wheel) ---
    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f) {
        float old_zoom = cam_zoom;
        cam_zoom += wheel * 0.05f;
        if (cam_zoom < 0.2f) cam_zoom = 0.2f;
        if (cam_zoom > 3.0f) cam_zoom = 3.0f;
        // Zoom toward mouse
        cam_offset_x = mouse.x - (mouse.x - cam_offset_x) * (cam_zoom / old_zoom);
        cam_offset_y = mouse.y - (mouse.y - cam_offset_y) * (cam_zoom / old_zoom);
    }

    // --- Keyboard: Arrow keys pan ---
    if (IsKeyDown(KEY_LEFT))  cam_offset_x += 4.0f;
    if (IsKeyDown(KEY_RIGHT)) cam_offset_x -= 4.0f;
    if (IsKeyDown(KEY_UP))    cam_offset_y += 4.0f;
    if (IsKeyDown(KEY_DOWN))  cam_offset_y -= 4.0f;
    """

# ---------------------------------------------------------
# DRAW SIDEBAR (Explorer — real issue filenames)
# ---------------------------------------------------------
node draw_sidebar() -> int:
    @C_Native
    """
    // Background
    DrawRectangle(0, 0, 250, 720, (Color){22, 22, 28, 255});
    DrawText("EXPLORER", 15, 12, 14, (Color){180, 180, 180, 255});
    DrawRectangle(0, 34, 250, 1, (Color){55, 55, 65, 255});

    // Section: DAG nodes
    DrawText("DAG NODES", 12, 42, 11, (Color){100, 180, 255, 255});
    for (int i = 0; i < node_count && i < 8; i++) {
        Color col = node_active[i] ? (Color){150, 255, 150, 255} : (Color){140, 140, 140, 255};
        DrawText(&node_labels[i * 64], 18, 58 + i * 18, 12, col);
    }

    DrawRectangle(0, 206, 250, 1, (Color){55, 55, 65, 255});

    // Section: Issues
    DrawText("ISSUES (DAG)", 12, 212, 11, (Color){100, 200, 255, 255});
    int max_visible = (720 - 230) / 22;
    for (int i = 0; i < issue_count && i < max_visible; i++) {
        bool sel = (i == selected_issue);
        if (sel) DrawRectangle(0, 228 + i * 22, 250, 21, (Color){40, 60, 90, 255});
        Color tc = sel ? (Color){255, 220, 80, 255} : (Color){130, 160, 200, 255};
        // Truncate long names to fit sidebar
        char disp[32] = {0};
        strncpy(disp, &issue_names[i * 80], 28);
        DrawText(disp, 14, 230 + i * 22, 11, tc);
    }

    // Sidebar right border
    DrawRectangle(250, 0, 1, 720, (Color){50, 50, 60, 255});
    """

# ---------------------------------------------------------
# DRAW GRAPH VIEWPORT (Live spatial canvas)
# ---------------------------------------------------------
node draw_viewport() -> int:
    @C_Native
    """
    // Viewport background
    DrawRectangle(251, 0, 1029, 565, (Color){15, 15, 20, 255});

    // Apply camera transform via scissor + manual offset
    BeginScissorMode(251, 0, 1029, 565);

    // Draw grid
    Color grid_col = (Color){30, 30, 38, 255};
    int grid_step = (int)(60.0f * cam_zoom);
    if (grid_step < 10) grid_step = 10;
    int ox = (int)fmodf(cam_offset_x + 251, grid_step);
    int oy = (int)fmodf(cam_offset_y, grid_step);
    for (int gx = ox + 251; gx < 1280; gx += grid_step)
        DrawRectangle(gx, 0, 1, 565, grid_col);
    for (int gy = oy; gy < 565; gy += grid_step)
        DrawRectangle(251, gy, 1029, 1, grid_col);

    // Draw wires between nodes (pairs: 0 to 1, 2 to 3, 0 to 2)
    int wire_pairs[][2] = {{0,1},{2,3},{0,2}};
    for (int w = 0; w < 3; w++) {
        int a = wire_pairs[w][0], b = wire_pairs[w][1];
        float ax = (node_x[a] + node_w[a] * 0.5f) * cam_zoom + cam_offset_x + 251;
        float ay = (node_y[a] + node_h[a] * 0.5f) * cam_zoom + cam_offset_y;
        float bx = (node_x[b] + node_w[b] * 0.5f) * cam_zoom + cam_offset_x + 251;
        float by = (node_y[b] + node_h[b] * 0.5f) * cam_zoom + cam_offset_y;
        DrawLineEx((Vector2){ax, ay}, (Vector2){bx, by}, 1.5f, (Color){80, 120, 180, 180});
        // Arrowhead
        float angle = atan2f(by - ay, bx - ax);
        float ah = 10.0f;
        DrawTriangle(
            (Vector2){bx, by},
            (Vector2){bx - ah * cosf(angle - 0.4f), by - ah * sinf(angle - 0.4f)},
            (Vector2){bx - ah * cosf(angle + 0.4f), by - ah * sinf(angle + 0.4f)},
            (Color){100, 160, 220, 200}
        );
    }

    // Draw nodes
    for (int i = 0; i < node_count; i++) {
        float sx = node_x[i] * cam_zoom + cam_offset_x + 251;
        float sy = node_y[i] * cam_zoom + cam_offset_y;
        float sw = node_w[i] * cam_zoom;
        float sh = node_h[i] * cam_zoom;

        // Shadow
        DrawRectangleRounded((Rectangle){sx + 3, sy + 3, sw, sh}, 0.2f, 8, (Color){0,0,0,80});

        // Body
        Color body = node_active[i]
            ? (Color){30, 55, 90, 255}
            : (Color){28, 32, 42, 255};
        DrawRectangleRounded((Rectangle){sx, sy, sw, sh}, 0.2f, 8, body);

        // Border
        Color border = node_active[i]
            ? (Color){80, 160, 255, 255}
            : (Color){55, 65, 85, 255};
        DrawRectangleRoundedLines((Rectangle){sx, sy, sw, sh}, 0.2f, 8, 1.5f, border);

        // Top accent bar
        DrawRectangleRounded((Rectangle){sx, sy, sw, 4}, 0.5f, 4,
            node_active[i] ? (Color){80,160,255,255} : (Color){60,90,130,180});

        // Label
        float font_size = 13.0f * cam_zoom;
        if (font_size < 8.0f) font_size = 8.0f;
        if (font_size > 20.0f) font_size = 20.0f;
        DrawText(&node_labels[i * 64], (int)(sx + 10), (int)(sy + 20), (int)font_size,
            (Color){200, 210, 230, 255});

        // Port dots
        DrawCircle((int)(sx), (int)(sy + sh * 0.5f), 5.0f * cam_zoom, (Color){80,160,255,255});
        DrawCircle((int)(sx + sw), (int)(sy + sh * 0.5f), 5.0f * cam_zoom, (Color){80,160,255,255});
    }

    EndScissorMode();

    // Viewport border
    DrawRectangleLines(251, 0, 1029, 565, (Color){50, 50, 60, 255});

    // HUD
    DrawText("[ axi DAG SPATIAL VIEWPORT ]", 270, 8, 11, (Color){55, 65, 80, 255});
    DrawText(TextFormat("Zoom: %.2fx  Pan: (%.0f, %.0f)  Nodes: %d",
        cam_zoom, cam_offset_x, cam_offset_y, node_count),
        270, 548, 11, (Color){80, 100, 130, 255});
    """

# ---------------------------------------------------------
# DRAW TERMINAL (Bottom panel)
# ---------------------------------------------------------
node draw_terminal() -> int:
    @C_Native
    """
    // Panel background
    DrawRectangle(250, 566, 1030, 154, (Color){14, 14, 18, 255});
    DrawRectangle(250, 566, 1030, 1, (Color){50, 50, 60, 255});

    // Header
    DrawRectangle(250, 566, 1030, 24, (Color){20, 20, 26, 255});
    DrawText("TERMINAL", 265, 572, 13, (Color){180, 180, 180, 255});
    DrawText("OUTPUT", 360, 572, 13, (Color){80, 80, 100, 255});

    DrawRectangle(250, 590, 1030, 1, (Color){40, 40, 50, 255});

    // Kernel status line
    DrawText("axi Kernel v2.0  |  domus_genesis.exe [RUNNING]  |  llama.cpp [LINKED]",
        265, 600, 12, (Color){80, 200, 100, 255});

    // Active terminal line
    DrawText(terminal_line, 265, 622, 12, (Color){200, 200, 200, 255});

    // Prompt
    DrawText("C:\\Antigravity\\ethos-products\\axi-lang> _",
        265, 645, 12, (Color){160, 160, 170, 255});

    // Controls hint
    DrawText("[RMB] Pan  [Scroll] Zoom  [Arrow Keys] Pan  [LMB] Select/Drag Node",
        265, 668, 11, (Color){60, 70, 90, 255});
    """

# ---------------------------------------------------------
# MAIN LOOP
# ---------------------------------------------------------
node main_loop() -> int:
    @C_Native
    """
    while (is_running) {
        handle_input();
        BeginDrawing();
        ClearBackground((Color){10, 10, 14, 255});
        draw_sidebar();
        draw_viewport();
        draw_terminal();
        EndDrawing();
    }
    CloseWindow();
    """

init_studio -> main_loop
main_loop -> handle_input
main_loop -> draw_sidebar
main_loop -> draw_viewport
main_loop -> draw_terminal
