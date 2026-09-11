# console_ui.axi
import node.graphics.canvas

pub enum ViewMode {
    Bar,
    Window,
}

pub enum MessageType {
    User,
    AI,
    System,
    Error,
}

pub struct Message {
    pub id: u32,
    pub msg_type: MessageType,
    pub timestamp: u64,
    pub content_ptr: u32,
}

pub struct Session {
    pub id: u32,
    pub label: str,
    pub message_count: u32,
    pub messages_head: u32,
}

pub struct ConsoleState {
    pub view_mode: ViewMode,
    pub active_session_id: u32,
    pub sessions_count: u32,
    pub is_output_expanded: bool,
    pub input_buffer: str,
    pub history_idx: i32,
}

# The Native Event Loop
# This is a stub for the main render loop to be integrated with the compiler.
# while sys.window_should_close() == false:
#    ...
