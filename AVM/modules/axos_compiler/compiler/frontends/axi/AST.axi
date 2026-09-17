// =====================================================================
// MODULE: frontends::axi::AST
// CLASSIFICATION: TIER 3 (RESTRICTED / INTERNAL ONLY)
// DESCRIPTION: Axi Abstract Syntax Tree Definitions.
// Explicitly enforces LOW <-> HIGH systems-level compliance.
// =====================================================================

@C_Include("<stdint.h>")
@C_Include("<stdbool.h>")

node axi_ast_definitions {
`
    // Enums defining the strict boundary between High and Low levels
    typedef enum {
        AXI_NODE_HIGH_SEMANTIC, // ABC/Python style abstract logic (HOW TO, PUT)
        AXI_NODE_LOW_BAREMETAL, // Raw C kernel boundary logic (node { `...` })
        AXI_NODE_BOUNDARY_BRIDGE // Memory handoff nodes crossing the boundary
    } AxiComplianceLevel;

    typedef enum {
        AST_HOW_TO_DECLARATION,
        AST_PUT_ASSIGNMENT,
        AST_C_INLINE_BLOCK,
        AST_SYSTEM_IO_HANDOFF
    } AxiNodeType;

    // The Universal AST Node Structure
    typedef struct AxiASTNode {
        AxiNodeType type;
        AxiComplianceLevel compliance; // Ensures HIGH-to-LOW memory safety
        char* identifier;
        char* payload;
        
        // Compliance Enforcement Flags
        bool requires_euler_pool_alloc; 
        bool is_unsafe_c_kernel_call;
        
        struct AxiASTNode* left_child;
        struct AxiASTNode* right_child;
    } AxiASTNode;

    // Boundary Validation Hook
    bool validate_compliance_handoff(AxiASTNode* high_node, AxiASTNode* low_node) {
        // Enforces that HIGH level logic cannot arbitrarily corrupt LOW level kernel state
        // without passing through an explicit AXI_NODE_BOUNDARY_BRIDGE
        if (high_node->compliance == AXI_NODE_HIGH_SEMANTIC && low_node->compliance == AXI_NODE_LOW_BAREMETAL) {
            return false; // Must be bridged!
        }
        return true;
    }
`
}

start -> axi_ast_definitions
