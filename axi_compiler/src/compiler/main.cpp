#include <iostream>
#include <string>
#include "lexer.h"
#include "parser.h"

using namespace Axi::compiler;

class ASTPrinter : public ASTVisitor {
public:
    int indent = 0;

    void printIndent() {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
    }

    void visit(IntLiteralExpr& node) override {
        printIndent();
        std::cout << "Literal(" << node.value << ")\n";
    }

    void visit(IdentExpr& node) override {
        printIndent();
        std::cout << "Ident(" << node.name << ")\n";
    }

    void visit(UnaryExpr& node) override {
        printIndent();
        std::cout << "Unary(" << static_cast<int>(node.op) << ")\n";
        indent++;
        if (node.right) node.right->accept(*this);
        indent--;
    }

    void visit(BinaryExpr& node) override {
        printIndent();
        std::cout << "Binary(" << static_cast<int>(node.op) << ")\n";
        indent++;
        if (node.left) node.left->accept(*this);
        if (node.right) node.right->accept(*this);
        indent--;
    }

    void visit(LetStmt& node) override {
        printIndent();
        std::cout << "LetStmt(name=" << node.name << ")\n";
        indent++;
        if (node.initializer) node.initializer->accept(*this);
        indent--;
    }

    void visit(ExprStmt& node) override {
        printIndent();
        std::cout << "ExprStmt\n";
        indent++;
        if (node.expr) node.expr->accept(*this);
        indent--;
    }

    void visit(FnDecl& node) override {
        printIndent();
        std::cout << "FnDecl(name=" << node.name << ")\n";
        indent++;
        for (auto& stmt : node.body) {
            if (stmt) stmt->accept(*this);
        }
        indent--;
    }

    void visit(StructDecl& node) override {
        printIndent();
        std::cout << "StructDecl(name=" << node.name << ")\n";
    }

    void visit(ModuleNode& node) override {
        printIndent();
        std::cout << "ModuleNode\n";
        indent++;
        for (auto& item : node.items) {
            if (item) item->accept(*this);
        }
        indent--;
    }
};

int main() {
    std::string source =
        "pub struct AppState { }\n"
        "pub fn main() {\n"
        "    let x = 42;\n"
        "    let velocity = distance / time + acceleration * time;\n"
        "}\n";
    std::cout << "Parsing Axi Code:\n" << source << "\n";

    AxiLexer lexer(source);
    auto tokens = lexer.lex();

    PrattParser parser(tokens);
    try {
        auto module = parser.parseModule();
        std::cout << "\nRed-Green CST Output:\n";

        ASTPrinter printer;
        if (module) {
            module->accept(printer);
        }
    } catch (const std::exception& e) {
        std::cerr << "Parse error: " << e.what() << "\n";
    }

    return 0;
}
