pub struct GraphemicCandidate {
    pub observed_variant: str,
    pub correction_hypothesis: str,
    pub disposition: EvidenceDisposition,
}

@query
pub fn assemble_graphemic_candidates(signal_id: str, raw_text: str, span: TokenSpan) -> [CanonicalEvidenceCell] {
    // Assembles typographic hypotheses while preserving noisy surface inputs intact.
}
