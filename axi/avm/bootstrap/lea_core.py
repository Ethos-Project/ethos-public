# ruff: noqa: F821
"""
Axi GENERATED PYTHON BINDINGS
STRICT COMPLIANCE: IMMUTABLE FROZEN DATACLASSES
"""

from dataclasses import dataclass
from typing import List, Dict, Any, Union

from enum import IntEnum


class CommitmentStatus(IntEnum):
    Observed = 1
    InferredNotAsserted = 2
    Rejected = 3


class AgentState:
    pass


@dataclass(frozen=True)
class AgentState_Idle(AgentState):
    pass


@dataclass(frozen=True)
class AgentState_Running(AgentState):
    _0: int


@dataclass(frozen=True)
class AgentState_Errored(AgentState):
    reason: str
    code: int


@dataclass(frozen=True)
class TokenSpan:
    start_char: int
    end_char: int


@dataclass(frozen=True)
class EvidenceLineage:
    raw_span: TokenSpan
    assembler_module_id: str
    module_version: str
    rules_triggered: List[str]
    upstream_dependencies: List[str]
    is_inferred: bool


@dataclass(frozen=True)
class EvidenceDisposition:
    hypothesis_confidence: float
    validation_confidence: float
    commitment_status: CommitmentStatus


@dataclass(frozen=True)
class CanonicalEvidenceCell:
    signal_id: str
    module_id: str
    module_version: str
    evidence_kind: str
    span: TokenSpan
    payload: Dict[str, Any]
    disposition: EvidenceDisposition
    lineage: EvidenceLineage


@dataclass(frozen=True)
class LinguisticEvidenceEnvelope:
    signal_id: str
    assembler_version: str
    graphemic_evidence: List[CanonicalEvidenceCell]
    morphological_evidence: List[CanonicalEvidenceCell]
    syntactic_evidence: List[CanonicalEvidenceCell]
    semantic_evidence: List[CanonicalEvidenceCell]
    inferred_evidence: List[CanonicalEvidenceCell]
    tamper_evident_digest: str

