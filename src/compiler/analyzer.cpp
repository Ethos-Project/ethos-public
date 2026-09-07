#include "analyzer.h"
#include <iostream>

namespace axiom {
namespace compiler {

void SemanticAnalyzer::analyze(std::shared_ptr<ModuleNode> module) {
    symTable.pushScope();
    for (auto& stmt : module->items) {
        analyzeStatement(stmt);
    }
    symTable.popScope();
}

void SemanticAnalyzer::analyzeStatement(std::shared_ptr<StmtNode> stmt) {
    if (auto fn = std::dynamic_pointer_cast<FnDecl>(stmt)) {
        symTable.pushScope();
        for (auto& s : fn->body->statements) {
            analyzeStatement(s);
        }
        symTable.popScope();
    } 
    else if (auto assign = std::dynamic_pointer_cast<AssignStmt>(stmt)) {
        analyzeExpression(assign->expr);
        
        if (auto ident = std::dynamic_pointer_cast<IdentNode>(assign->target)) {
            try {
                symTable.resolve(ident->name);
                std::cout << "[Analyzer] Re-assigning '" << ident->name << "' with type '" << assign->expr->resolved_type << "'\n";
            } catch (...) {
                symTable.define(ident->name, assign->expr->resolved_type);
                std::cout << "[Analyzer] Defined new variable '" << ident->name << "' with inferred type '" << assign->expr->resolved_type << "'\n";
            }
        } else {
            // It's a member access or something else, just analyze it to ensure it's valid
            analyzeExpression(assign->target);
            std::cout << "[Analyzer] Mutating memory at member access.\n";
        }
    }
    else if (auto write = std::dynamic_pointer_cast<WriteStmt>(stmt)) {
        analyzeExpression(write->expr);
    }
    else if (auto w = std::dynamic_pointer_cast<WhileStmt>(stmt)) {
        analyzeExpression(w->condition);
        symTable.pushScope();
        for (auto& s : w->body->statements) {
            analyzeStatement(s);
        }
        symTable.popScope();
    }
    else if (auto f = std::dynamic_pointer_cast<ForStmt>(stmt)) {
        analyzeExpression(f->range);
        symTable.pushScope();
        symTable.define(f->iterator_name, "i32"); // Assume range iterator is i32 for now
        for (auto& s : f->body->statements) {
            analyzeStatement(s);
        }
        symTable.popScope();
    }
    else if (auto sd = std::dynamic_pointer_cast<StructDeclNode>(stmt)) {
        StructDef def;
        def.name = sd->name;
        for (const auto& field : sd->fields) {
            def.fields[field.first] = field.second;
        }
        structRegistry[def.name] = def;
        std::cout << "[Analyzer] Registered Struct '" << def.name << "' with " << def.fields.size() << " fields.\n";
    }
    else if (auto nd = std::dynamic_pointer_cast<NativeFnDeclNode>(stmt)) {
        FnDef def;
        def.name = nd->name;
        def.return_type = nd->return_type;
        for (const auto& param : nd->params) {
            def.param_types.push_back(param.second);
        }
        fnRegistry[def.name] = def;
        std::cout << "[Analyzer] Registered @C_Native Function '" << def.name << "' returning '" << def.return_type << "'.\n";
    }
}

void SemanticAnalyzer::analyzeExpression(std::shared_ptr<ExprNode> expr) {
    if (auto lit = std::dynamic_pointer_cast<LiteralNode>(expr)) {
        // If it was already resolved as str by parser, keep it
        if (lit->resolved_type != "str") {
            if (lit->value.find('.') != std::string::npos) {
                lit->resolved_type = "f32";
            } else {
                lit->resolved_type = "i32";
            }
        }
    }
    else if (auto ident = std::dynamic_pointer_cast<IdentNode>(expr)) {
        ident->resolved_type = symTable.resolve(ident->name);
    }
    else if (auto bin = std::dynamic_pointer_cast<BinaryNode>(expr)) {
        analyzeExpression(bin->lhs);
        analyzeExpression(bin->rhs);
        // Standard arithmetic type promotion logic
        if (bin->lhs->resolved_type == "f32" || bin->rhs->resolved_type == "f32") {
            bin->resolved_type = "f32";
        } else {
            bin->resolved_type = bin->lhs->resolved_type;
        }
    }
    else if (auto sinit = std::dynamic_pointer_cast<StructInitNode>(expr)) {
        if (structRegistry.find(sinit->struct_name) == structRegistry.end()) {
            throw std::runtime_error("Unknown struct type: " + sinit->struct_name);
        }
        for (auto& field : sinit->fields) {
            analyzeExpression(field.second);
            // In a real compiler, we'd verify field.second->resolved_type matches structRegistry[sinit->struct_name].fields[field.first]
        }
        sinit->resolved_type = sinit->struct_name;
    }
    else if (auto idx = std::dynamic_pointer_cast<IndexNode>(expr)) {
        analyzeExpression(idx->object);
        analyzeExpression(idx->index);
        idx->resolved_type = "unknown";
    }
    else if (auto macc = std::dynamic_pointer_cast<MemberAccessNode>(expr)) {
        analyzeExpression(macc->object);
        std::string obj_type = macc->object->resolved_type;
        if (structRegistry.find(obj_type) == structRegistry.end()) {
            throw std::runtime_error("Attempted member access on non-struct type: " + obj_type);
        }
        auto& sdef = structRegistry[obj_type];
        if (sdef.fields.find(macc->member) == sdef.fields.end()) {
            throw std::runtime_error("Struct " + obj_type + " has no member '" + macc->member + "'");
        }
        macc->resolved_type = sdef.fields[macc->member];
    }
    else if (auto fncall = std::dynamic_pointer_cast<FnCallNode>(expr)) {
        // Disabled for FOSS external imports check
        // if (global_functions.find(fncall->name) == global_functions.end()) {
        //     throw std::runtime_error("Call to unknown function: " + fncall->name);
        // }
        for (auto& arg : fncall->args) {
            analyzeExpression(arg);
        }
        fncall->resolved_type = fnRegistry[fncall->name].return_type;
    }
}

}
}
