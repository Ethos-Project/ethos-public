#include "parser.h"
#include "AST.h"

namespace Axi {

std::shared_ptr<DAG> Parser::parse() {
    auto dag = std::make_shared<DAG>();
    
    while (peek().type != TokenType::END_OF_FILE) {
        if (peek().type == TokenType::KW_NODE) {
            dag->nodes.push_back(parse_node_declaration());
        } else {
            advance(); // Skip unhandled globals for now
        }
    }
    return dag;
}

std::shared_ptr<Node> Parser::parse_node_declaration() {
    advance(); // Consume KW_NODE
    
    // Check if this node is tagged with a Cognitive/Inference directive
    if (match(TokenType::TAG_INFERENCE)) {
        return parse_cognitive_block();
    }
    
    // Otherwise, parse a standard execution node
    auto std_node = std::make_shared<Node>();
    std_node->is_c_native_block = match(TokenType::TAG_C_NATIVE);
    
    // Parse identifiers, pins, and body...
    if (peek().type == TokenType::IDENTIFIER) {
        std_node->nodeID = advance().lexeme;
    }
    
    return std_node;
}

std::shared_ptr<CognitiveNode> Parser::parse_cognitive_block() {
    auto cog_node = std::make_shared<CognitiveNode>();
    
    // The presence of @Inference automatically flags this for SIMD/GPU compilation
    cog_node->is_hardware_accelerated = true;
    
    if (peek().type == TokenType::IDENTIFIER) {
        cog_node->nodeID = advance().lexeme + "_inference_pass";
    }
    
    // Next, we check for Native AI primitives explicitly within this block
    if (match(TokenType::TYPE_TENSOR) || match(TokenType::TYPE_MATRIX)) {
        // Here, the AST inherently prepares memory layouts for SIMD execution
        cog_node->tensor_shape = "[Dynamic Quantum Matrix]"; // Placeholder for actual shape parsing
        cog_node->optimize_for_avx512();
    }
    
    return cog_node;
}

}
