#include <string>
#include <memory>

enum class AxiComplianceLevel { HighSemantic = 0, LowBareMetal = 1, BoundaryBridge = 2 };

struct AxiASTNode {
    int type;
    AxiComplianceLevel compliance;
    std::string identifier;
    std::string payload;
    bool requires_euler_pool_alloc;
    bool is_unsafe_c_kernel_call;
    std::shared_ptr<AxiASTNode> left_child;
    std::shared_ptr<AxiASTNode> right_child;
};
