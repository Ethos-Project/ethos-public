# axi_studio_state.axi
import node.graphics.canvas

struct SpatialNode:
    id: int
    label: string
    pos_x: int
    pos_y: int
    width: int
    height: int
    is_dragging: bool
    is_active: bool
    is_sandboxed: bool

struct Camera2D:
    offset_x: float
    offset_y: float
    target_x: float
    target_y: float
    rotation: float
    zoom: float

state canvas_nodes: array[SpatialNode, 256] = @Zero_Init

state sandbox_bounds: canvas.Rectangle = { 500.0, 100.0, 800.0, 600.0 }

state camera: Camera2D = {640.0, 360.0, 0.0, 0.0, 0.0, 1.0}

state is_context_menu_open: bool = false
state context_menu_x: int = 0
state context_menu_y: int = 0

state wire_drag_source_id: int = -1
state wire_drag_x: int = 0
state wire_drag_y: int = 0
