#include "parser.h"

namespace axiom {
namespace compiler {

PrattParser::PrattParser(const std::vector<Token>& tokens_in) : pos(0) {
    for (const auto& t : tokens_in) {
        tokens.push_back(t);
    }
}

const Token* PrattParser::current() const {
    if (pos < tokens.size()) return &tokens[pos];
    return nullptr;
}

const Token* PrattParser::bump() {
    if (pos < tokens.size()) return &tokens[pos++];
    return nullptr;
}

void PrattParser::expect(TokenType type) {
    if (!current() || current()->type != type) {
        throw std::runtime_error("Unexpected token! Expected type " + std::to_string(static_cast<int>(type)) + 
            (current() ? " but got '" + current()->lexeme + "' of type " + std::to_string(static_cast<int>(current()->type)) : " but got EOF"));
    }
    bump();
}

int PrattParser::prefixBindingPower(TokenType type) {
    if (type == TokenType::PUNCTUATION && (current()->lexeme == "-" || current()->lexeme == "+")) return 13;
    return -1;
}

std::pair<int, int> PrattParser::infixBindingPower(TokenType type) {
    if (type == TokenType::PUNCTUATION) {
        if (current()->lexeme == "=") return {2, 1};
        if (current()->lexeme == "<" || current()->lexeme == ">") return {7, 8}; // Comparison
        if (current()->lexeme == "+" || current()->lexeme == "-") return {9, 10};
        if (current()->lexeme == "*" || current()->lexeme == "/") return {11, 12};
        if (current()->lexeme == "." || current()->lexeme == "::") return {15, 16}; // Highest binding power for member access
        if (current()->lexeme == "[") return {17, 1}; // High precedence for array indexing
    }
    return {-1, -1};
}

std::shared_ptr<ExprNode> PrattParser::parseExpression(int min_bp) {
    auto token = bump();
    if (!token) throw std::runtime_error("Unexpected EOF");

    std::shared_ptr<ExprNode> lhs;

    if (token->type == TokenType::PUNCTUATION && token->lexeme == "(") {
        lhs = parseExpression();
        auto next = bump();
        if (!next || next->type != TokenType::PUNCTUATION || next->lexeme != ")") {
            throw std::runtime_error("Expected ')' after expression");
        }
    } else if (token->type == TokenType::LITERAL_INT || token->type == TokenType::LITERAL_FLOAT || token->type == TokenType::LITERAL_STRING) {
        lhs = std::make_shared<LiteralNode>(token->lexeme);
        if (token->type == TokenType::LITERAL_STRING) {
            std::dynamic_pointer_cast<LiteralNode>(lhs)->resolved_type = "str";
        }
    } else if (token->type == TokenType::IDENTIFIER) {
        // Look ahead for struct initialization e.g., Vector(x=1, y=2) or fn call e.g., print(x)
        if (current() && current()->type == TokenType::PUNCTUATION && current()->lexeme == "(") {
            bool is_struct_init = false;
            // Hacky lookahead to distinguish: if next is IDENTIFIER followed by '=' it's struct init
            if (pos + 1 < tokens.size() && tokens[pos+1].type == TokenType::PUNCTUATION && tokens[pos+1].lexeme == "=") {
                is_struct_init = true;
            }
            
            if (is_struct_init) {
                auto struct_init = std::make_shared<StructInitNode>(token->lexeme);
                bump(); // consume '('
                while (current() && !(current()->type == TokenType::PUNCTUATION && current()->lexeme == ")")) {
                    auto field_name = bump();
                    expect(TokenType::PUNCTUATION); // '='
                    auto field_val = parseExpression();
                    struct_init->fields.push_back({field_name->lexeme, field_val});
                    if (current() && current()->type == TokenType::PUNCTUATION && current()->lexeme == ",") bump();
                }
                expect(TokenType::PUNCTUATION); // ')'
                lhs = struct_init;
            } else {
                // Function call
                auto fn_call = std::make_shared<FnCallNode>(token->lexeme);
                bump(); // consume '('
                while (current() && !(current()->type == TokenType::PUNCTUATION && current()->lexeme == ")")) {
                    fn_call->args.push_back(parseExpression());
                    if (current() && current()->type == TokenType::PUNCTUATION && current()->lexeme == ",") bump();
                }
                expect(TokenType::PUNCTUATION); // ')'
                lhs = fn_call;
            }
        } else {
            lhs = std::make_shared<IdentNode>(token->lexeme);
        }
    } else {
        throw std::runtime_error("Unexpected token in expression: " + token->lexeme);
    }

    while (true) {
        auto op_token = current();
        if (!op_token) break;
        
        auto bp = infixBindingPower(op_token->type);
        if (bp.first == -1) break;

        if (bp.first < min_bp) break;

        bump(); // consume operator
        if (op_token->lexeme == "[") {
            auto index_expr = parseExpression();
            auto next = bump();
            if (!next || next->type != TokenType::PUNCTUATION || next->lexeme != "]") {
                throw std::runtime_error("Expected ']' after array index");
            }
            lhs = std::make_shared<IndexNode>(lhs, index_expr);
        } else if (op_token->lexeme == "." || op_token->lexeme == "::") {
            auto member_tok = bump(); // Should be IDENTIFIER
            lhs = std::make_shared<MemberAccessNode>(lhs, member_tok->lexeme);
        } else {
            auto rhs = parseExpression(bp.second);
            lhs = std::make_shared<BinaryNode>(op_token->lexeme, lhs, rhs);
        }
    }

    return lhs;
}

std::shared_ptr<StmtNode> PrattParser::parseStatement() {
    auto token = current();
    if (!token) return nullptr;

    if (token->type == TokenType::KW_HOW_TO) {
        bump();
        auto name_tok = bump();
        auto decl = std::make_shared<FnDecl>(name_tok->lexeme);
        expect(TokenType::PUNCTUATION); // (
        expect(TokenType::PUNCTUATION); // )
        expect(TokenType::PUNCTUATION); // :
        decl->body = parseBlock();
        return decl;
    } else if (token->type == TokenType::KW_PUT) {
        bump();
        auto expr = parseExpression();
        expect(TokenType::KW_IN);
        auto target_expr = parseExpression();
        return std::make_shared<AssignStmt>(target_expr, expr);
    } else if (token->type == TokenType::KW_WRITE) {
        bump();
        auto expr = parseExpression();
        expect(TokenType::KW_TO_SCREEN);
        return std::make_shared<WriteStmt>(expr);
    } else if (token->type == TokenType::KW_WHILE) {
        bump();
        auto cond = parseExpression();
        expect(TokenType::PUNCTUATION); // :
        auto while_stmt = std::make_shared<WhileStmt>(cond);
        while_stmt->body = parseBlock();
        return while_stmt;
    } else if (token->type == TokenType::KW_FOR) {
        bump();
        auto iter_tok = bump();
        expect(TokenType::KW_IN);
        auto range = parseExpression();
        expect(TokenType::PUNCTUATION); // :
        auto for_stmt = std::make_shared<ForStmt>(iter_tok->lexeme, range);
        for_stmt->body = parseBlock();
        return for_stmt;
    } else if (token->type == TokenType::KW_STRUCT) {
        bump();
        auto name_tok = bump();
        expect(TokenType::PUNCTUATION); // :
        auto struct_decl = std::make_shared<StructDeclNode>(name_tok->lexeme);
        
        // Parse block of fields
        if (current() && current()->type == TokenType::FRAME_ENTER) {
            bump();
            while (current() && current()->type != TokenType::FRAME_EXIT && current()->type != TokenType::END_OF_FILE) {
                auto type_tok = bump(); // Type e.g., i32
                auto field_tok = bump(); // Field name
                struct_decl->fields.push_back({field_tok->lexeme, type_tok->lexeme});
            }
            if (current() && current()->type == TokenType::FRAME_EXIT) {
                bump();
            }
        }
        return struct_decl;
    } else if (token->type == TokenType::TAG_C_NATIVE) {
        bump();
        if (current() && current()->type == TokenType::KW_PUB) bump(); // optional pub
        expect(TokenType::KW_FN);
        auto name_tok = bump();
        auto native_decl = std::make_shared<NativeFnDeclNode>(name_tok->lexeme);
        
        expect(TokenType::PUNCTUATION); // (
        while (current() && !(current()->type == TokenType::PUNCTUATION && current()->lexeme == ")")) {
            auto param_name = bump();
            expect(TokenType::PUNCTUATION); // :
            auto param_type = bump();
            native_decl->params.push_back({param_name->lexeme, param_type->lexeme});
            if (current() && current()->type == TokenType::PUNCTUATION && current()->lexeme == ",") bump();
        }
        expect(TokenType::PUNCTUATION); // )
        
        if (current() && current()->type == TokenType::PUNCTUATION && current()->lexeme == "-") {
            bump(); // -
            expect(TokenType::PUNCTUATION); // >
            native_decl->return_type = bump()->lexeme;
        } else {
            native_decl->return_type = "void";
        }
        
        return native_decl;
    } else if (token->type == TokenType::KW_IMPORT) {
        bump();
        std::string path;
        while (current() && current()->type == TokenType::IDENTIFIER) {
            path += current()->lexeme;
            bump();
            if (current() && current()->type == TokenType::PUNCTUATION && current()->lexeme == "::") {
                bump(); // ::
                path += "::";
            } else {
                break;
            }
        }
        return std::make_shared<ImportNode>(path);
    }
    
    // Fallback: try parsing as an expression statement
    try {
        auto expr = parseExpression();
        return std::make_shared<ExprStmt>(expr);
    } catch (const std::exception& e) {
        throw std::runtime_error("Unknown statement (" + token->lexeme + "): " + e.what());
    }
}

std::shared_ptr<BlockStmt> PrattParser::parseBlock() {
    auto block = std::make_shared<BlockStmt>();
    if (current() && current()->type == TokenType::FRAME_ENTER) {
        bump(); // consume FRAME_ENTER
        while (current() && current()->type != TokenType::FRAME_EXIT && current()->type != TokenType::END_OF_FILE) {
            block->statements.push_back(parseStatement());
        }
        if (current() && current()->type == TokenType::FRAME_EXIT) {
            bump(); // consume FRAME_EXIT
        }
    } else {
        // Single statement block
        block->statements.push_back(parseStatement());
    }
    return block;
}

std::shared_ptr<ModuleNode> PrattParser::parseModule() {
    auto mod = std::make_shared<ModuleNode>();
    while (current() && current()->type != TokenType::END_OF_FILE) {
        mod->items.push_back(parseStatement());
    }
    return mod;
}

}
}
