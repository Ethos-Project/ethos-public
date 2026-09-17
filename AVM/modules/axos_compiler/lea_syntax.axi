pub struct DependencyGraphArc {
    pub head_cell_id: str,
    pub head_surface: str,
    pub relation_label: str,
    pub dependent_cell_id: str,
    pub dependent_surface: str,
}

pub struct LinguisticWorkspace {
    pub signal_id: str,
    pub stack: [CanonicalEvidenceCell],
    pub buffer: [CanonicalEvidenceCell],
    pub arcs: [DependencyGraphArc],
}

@query
pub fn execute_shift(ws: &LinguisticWorkspace) -> LinguisticWorkspace {
    // Moves the top item from the input buffer queue onto the workspace stack.
}

@query
pub fn execute_left_arc(ws: &LinguisticWorkspace, edge_label: str) -> LinguisticWorkspace {
    // Creates an asymmetric dependency arrow: Stack[0] (Head) -> Stack[1] (Dependent).
}

@query
pub fn execute_right_arc(ws: &LinguisticWorkspace, edge_label: str) -> LinguisticWorkspace {
    // Creates an asymmetric dependency arrow: Stack[1] (Head) -> Stack[0] (Dependent).
}
