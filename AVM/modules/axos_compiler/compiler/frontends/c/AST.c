#include <stdint.h>
#include <stdbool.h>

typedef enum { AXI_NODE_HIGH_SEMANTIC = 0, AXI_NODE_LOW_BAREMETAL = 1, AXI_NODE_BOUNDARY_BRIDGE = 2 } AxiComplianceLevel;

typedef struct AxiASTNode {
    int type;
    AxiComplianceLevel compliance;
    char* identifier;
    char* payload;
    bool requires_euler_pool_alloc;
    bool is_unsafe_c_kernel_call;
    void* left_child;
    void* right_child;
} AxiASTNode;
