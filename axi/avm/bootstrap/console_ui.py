# ruff: noqa: F821
"""
Axi GENERATED PYTHON BINDINGS
STRICT COMPLIANCE: IMMUTABLE FROZEN DATACLASSES
"""

from dataclasses import dataclass
from typing import List, Dict, Any, Union

from enum import IntEnum


class ViewMode(IntEnum):
    Bar = 0
    Window = 1


class MessageType(IntEnum):
    User = 0
    AI = 1
    System = 2
    Error = 3


@dataclass(frozen=True)
class Message:
    id: int
    msg_type: MessageType
    timestamp: int
    content_ptr: int


@dataclass(frozen=True)
class Session:
    id: int
    label: str
    message_count: int
    messages_head: int


@dataclass(frozen=True)
class ConsoleState:
    view_mode: ViewMode
    active_session_id: int
    sessions_count: int
    is_output_expanded: bool
    input_buffer: str
    history_idx: int

