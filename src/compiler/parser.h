#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <memory>
#include <stdexcept>

namespace axiom {
namespace compiler {

// AST Nodes
class ExprNode {
public:
    std::string resolved_type = "unknown";
    virtual ~ExprNode() = default;
};

class LiteralNode : public ExprNode {
public:
    std::string value;
    LiteralNode(std::string v) : value(v) {}
};

class IdentNode : public ExprNode {
public:
    std::string name;
    IdentNode(std::string n) : name(n) {}
};

class BinaryNode : public ExprNode {
public:
    std::string op;
    std::shared_ptr<ExprNode> lhs;
    std::shared_ptr<ExprNode> rhs;
    BinaryNode(std::string o, std::shared_ptr<ExprNode> l, std::shared_ptr<ExprNode> r) : op(o), lhs(l), rhs(r) {}
};

class MemberAccessNode : public ExprNode {
public:
    std::shared_ptr<ExprNode> object;
    std::string member;
    MemberAccessNode(std::shared_ptr<ExprNode> obj, std::string mem) : object(obj), member(mem) {}
};

class IndexNode : public ExprNode {
public:
    std::shared_ptr<ExprNode> object;
    std::shared_ptr<ExprNode> index;
    IndexNode(std::shared_ptr<ExprNode> obj, std::shared_ptr<ExprNode> idx) : object(obj), index(idx) {}
};

class StructInitNode : public ExprNode {
public:
    std::string struct_name;
    std::vector<std::pair<std::string, std::shared_ptr<ExprNode>>> fields;
    StructInitNode(std::string name) : struct_name(name) {}
};

class FnCallNode : public ExprNode {
public:
    std::string name;
    std::vector<std::shared_ptr<ExprNode>> args;
    FnCallNode(std::string n) : name(n) {}
};

class StmtNode {
public:
    virtual ~StmtNode() = default;
};

class ImportNode : public StmtNode {
public:
    std::string module_path;
    ImportNode(std::string path) : module_path(path) {}
};

class ExprStmt : public StmtNode {
public:
    std::shared_ptr<ExprNode> expr;
    ExprStmt(std::shared_ptr<ExprNode> e) : expr(e) {}
};

class NativeFnDeclNode : public StmtNode {
public:
    std::string name;
    std::string return_type;
    std::vector<std::pair<std::string, std::string>> params; // name, type
    std::string c_native_target; 
    NativeFnDeclNode(std::string n) : name(n) {}
};

class StructDeclNode : public StmtNode {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> fields; // name, type
    StructDeclNode(std::string n) : name(n) {}
};

class AssignStmt : public StmtNode {
public:
    std::shared_ptr<ExprNode> target;
    std::shared_ptr<ExprNode> expr;
    AssignStmt(std::shared_ptr<ExprNode> t, std::shared_ptr<ExprNode> e) : target(t), expr(e) {}
};

class WriteStmt : public StmtNode {
public:
    std::shared_ptr<ExprNode> expr;
    WriteStmt(std::shared_ptr<ExprNode> e) : expr(e) {}
};

class BlockStmt : public StmtNode {
public:
    std::vector<std::shared_ptr<StmtNode>> statements;
};

class WhileStmt : public StmtNode {
public:
    std::shared_ptr<ExprNode> condition;
    std::shared_ptr<BlockStmt> body;
    WhileStmt(std::shared_ptr<ExprNode> cond) : condition(cond) {
        body = std::make_shared<BlockStmt>();
    }
};

class ForStmt : public StmtNode {
public:
    std::string iterator_name;
    std::shared_ptr<ExprNode> range;
    std::shared_ptr<BlockStmt> body;
    ForStmt(std::string name, std::shared_ptr<ExprNode> r) : iterator_name(name), range(r) {
        body = std::make_shared<BlockStmt>();
    }
};

class FnDecl : public StmtNode {
public:
    std::string name;
    std::shared_ptr<BlockStmt> body;
    FnDecl(std::string n) : name(n) {
        body = std::make_shared<BlockStmt>();
    }
};

class ModuleNode {
public:
    std::vector<std::shared_ptr<StmtNode>> items;
};

// Pratt Parser
class PrattParser {
public:
    PrattParser(const std::vector<Token>& tokens);
    std::shared_ptr<ModuleNode> parseModule();

private:
    std::vector<Token> tokens;
    size_t pos;

    const Token* current() const;
    const Token* bump();
    void expect(TokenType type);

    std::shared_ptr<ExprNode> parseExpression(int min_bp = 0);
    int prefixBindingPower(TokenType type);
    std::pair<int, int> infixBindingPower(TokenType type);
    
    std::shared_ptr<StmtNode> parseStatement();
    std::shared_ptr<BlockStmt> parseBlock();
};

}
}

#endif // PARSER_H
