#[repr(C)]
pub enum AxiComplianceLevel {
    HighSemantic = 0,
    LowBareMetal = 1,
    BoundaryBridge = 2,
}

#[repr(C)]
pub struct AxiASTNode {
    pub node_type: i32,
    pub compliance: AxiComplianceLevel,
    pub identifier: *mut libc::c_char,
    pub payload: *mut libc::c_char,
    pub requires_euler_pool_alloc: bool,
    pub is_unsafe_c_kernel_call: bool,
    pub left_child: *mut libc::c_void,
    pub right_child: *mut libc::c_void,
}
