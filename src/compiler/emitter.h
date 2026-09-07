#ifndef EMITTER_H
#define EMITTER_H

#include "parser.h"
#include <sstream>
#include <string>
#include <memory>

namespace axiom {
namespace compiler {

class C23Emitter {
public:
    std::string emit(std::shared_ptr<ModuleNode> module);

private:
    std::stringstream out;
    int indent_level = 0;
    std::vector<std::string> declared_vars; // quick hack for demo

    void indent();
    void emitStatement(std::shared_ptr<StmtNode> stmt);
    void emitExpression(std::shared_ptr<ExprNode> expr);
    void emitBlock(std::shared_ptr<BlockStmt> block);
};

} // namespace compiler
} // namespace axiom

#endif // EMITTER_H
