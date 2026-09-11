#include <iostream>
#include "AST.cpp"

std::shared_ptr<AxiASTNode> parse_cpp_to_axi_dag(const std::string& source) {
    std::cout << "[CPP-FRONTEND] Parsing C++ AST to Axi DAG...\n";
    return std::make_shared<AxiASTNode>();
}
