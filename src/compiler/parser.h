#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "AST.h"
namespace Axiom {
class Parser {
public:
    Parser(const std::vector<Token>& tokens) {}
    void parse() {}
};
}
#endif
