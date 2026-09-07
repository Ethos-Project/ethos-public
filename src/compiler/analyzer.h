#ifndef ANALYZER_H
#define ANALYZER_H

#include "parser.h"
#include <map>
#include <string>
#include <vector>
#include <stdexcept>

namespace axiom {
namespace compiler {

class SymbolTable {
public:
    void pushScope() {
        scopes.push_back({});
    }

    void popScope() {
        if (!scopes.empty()) {
            scopes.pop_back();
        }
    }

    void define(const std::string& name, const std::string& type) {
        if (scopes.empty()) pushScope();
        scopes.back()[name] = type;
    }

    std::string resolve(const std::string& name) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            if (it->find(name) != it->end()) {
                return (*it)[name];
            }
        }
        throw std::runtime_error("Undefined variable: " + name);
    }

private:
    std::vector<std::map<std::string, std::string>> scopes;
};

struct StructDef {
    std::string name;
    std::map<std::string, std::string> fields;
};

struct FnDef {
    std::string name;
    std::string return_type;
    std::vector<std::string> param_types;
};

class SemanticAnalyzer {
public:
    void analyze(std::shared_ptr<ModuleNode> module);
    std::map<std::string, StructDef> structRegistry;
    std::map<std::string, FnDef> fnRegistry;

private:
    SymbolTable symTable;

    void analyzeStatement(std::shared_ptr<StmtNode> stmt);
    void analyzeExpression(std::shared_ptr<ExprNode> expr);
};

}
}

#endif // ANALYZER_H
