pub enum CommitmentStatus : u8 {
    Observed = 1,
    InferredNotAsserted = 2,
    Rejected = 3,
}

pub struct TokenSpan {
    pub start_char: u32,
    pub end_char: u32,
}

pub struct EvidenceLineage {
    pub raw_span: TokenSpan,
    pub assembler_module_id: str,
    pub module_version: str,
    pub rules_triggered: [str],
    pub upstream_dependencies: [str],
    pub is_inferred: bool,
}

pub struct EvidenceDisposition {
    pub hypothesis_confidence: f32,
    pub validation_confidence: f32,
    pub commitment_status: CommitmentStatus,
}

pub struct CanonicalEvidenceCell {
    pub signal_id: str,
    pub module_id: str,
    pub module_version: str,
    pub evidence_kind: str,
    pub span: TokenSpan,
    pub payload: map[str, any],
    pub disposition: EvidenceDisposition,
    pub lineage: EvidenceLineage,
}

pub struct LinguisticEvidenceEnvelope {
    pub signal_id: str,
    pub assembler_version: str,
    pub graphemic_evidence: [CanonicalEvidenceCell],
    pub morphological_evidence: [CanonicalEvidenceCell],
    pub syntactic_evidence: [CanonicalEvidenceCell],
    pub semantic_evidence: [CanonicalEvidenceCell],
    pub inferred_evidence: [CanonicalEvidenceCell],
    pub tamper_evident_digest: str,
}

pub enum AgentState {
    Idle,
    Running(u32),
    Errored { reason: str, code: u32 },
}
