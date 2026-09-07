# lib/node/graphics/canvas.flux
# Decoupled Graphics Architecture - Raylib Backend

node init_window(width: int, height: int):
    @C_Native
    """
    InitWindow(width, height, "FLUX IDE");
    """

node begin_drawing():
    @C_Native
    """
    BeginDrawing();
    """

node clear_background():
    @C_Native
    """
    ClearBackground((Color){ 30, 30, 40, 255 });
    """

node end_drawing():
    @C_Native
    """
    EndDrawing();
    """

node close_window():
    @C_Native
    """
    CloseWindow();
    """

struct Vector2:
    x: float
    y: float

struct Rectangle:
    x: float
    y: float
    width: float
    height: float

struct Color:
    r: int8
    g: int8
    b: int8
    a: int8

node get_mouse_x() -> int:
    @C_Native
    """
    return GetMouseX();
    """

node get_mouse_y() -> int:
    @C_Native
    """
    return GetMouseY();
    """

node is_mouse_down(button: int) -> bool:
    @C_Native
    """
    return IsMouseButtonDown(button);
    """

node draw_rect(x: int, y: int, width: int, height: int, color: Color):
    @C_Native
    """
    DrawRectangle(x, y, width, height, color);
    """

node draw_text(text: string, x: int, y: int, fontSize: int, color: Color):
    @C_Native
    """
    DrawText(text, x, y, fontSize, color);
    """

node draw_bezier_wire(start: Vector2, end: Vector2, thick: float, color: Color):
    @C_Native
    """
    DrawLineBezier(start, end, thick, color);
    """

node draw_rectangle_rec(rec: Rectangle, color: Color):
    @C_Native
    """
    DrawRectangleRec(rec, color);
    """

node draw_rectangle_lines_ex(rec: Rectangle, lineThick: float, color: Color):
    @C_Native
    """
    DrawRectangleLinesEx(rec, lineThick, color);
    """

node check_collision_point_rec(point: Vector2, rec: Rectangle) -> bool:
    @C_Native
    """
    return CheckCollisionPointRec(point, rec);
    """

