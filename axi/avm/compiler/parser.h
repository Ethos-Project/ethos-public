#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "AST.h"
namespace axi {
class Parser {
private:
    std::vector<Token> tokens;
    size_t current = 0;

    Token peek() const { return tokens[current]; }
    Token advance() { return tokens[current++]; }
    bool match(TokenType type) {
        if (peek().type == type) {
            advance();
            return true;
        }
        return false;
    }

    std::shared_ptr<Node> parse_node_declaration();
    std::shared_ptr<CognitiveNode> parse_cognitive_block();

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {}
    std::shared_ptr<DAG> parse();
};
}
#endif
