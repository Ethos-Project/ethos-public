# ruff: noqa: F821
"""
Axi GENERATED PYTHON BINDINGS
STRICT COMPLIANCE: IMMUTABLE FROZEN DATACLASSES
"""

from dataclasses import dataclass
from typing import List, Dict, Any, Union

from enum import IntEnum


@dataclass(frozen=True)
class QueryDb:
    file_id: int
    revision: int


@dataclass(frozen=True)
class ItemSignature:
    symbol_hash: int
    param_count: int
    is_pure: bool
    has_contracts: bool


# ====================================================================
# QUERY & COMPUTE KERNEL IPC STUBS
# ====================================================================


# @query (Memoized incrementally)
def parse_cst(db: QueryDb, source_bytes: bytes) -> GreenNode:
    raise NotImplementedError("Kernel IPC Boundary - call into libAxi.so")


# @query (Memoized incrementally)
def extract_signatures(db: QueryDb, root: GreenNode) -> List[ItemSignature]:
    raise NotImplementedError("Kernel IPC Boundary - call into libAxi.so")


# @query (Memoized incrementally)
def verify_item_contracts(db: QueryDb, item_hash: int) -> bool:
    raise NotImplementedError("Kernel IPC Boundary - call into libAxi.so")

