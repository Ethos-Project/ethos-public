#include "lexer.h"
#include <cctype>
#include <iostream>

namespace axiom {
namespace compiler {

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    int pos = 0;
    
    // Simplistic line processing for indentations (Pythonic)
    std::vector<int> indent_stack;
    indent_stack.push_back(0);
    
    while (pos < sourceCode.length()) {
        char c = sourceCode[pos];
        
        if (isspace(c)) {
            // Check for newlines and indents
            if (c == '\n') {
                pos++;
                int spaces = 0;
                while (pos < sourceCode.length() && sourceCode[pos] == ' ') {
                    spaces++;
                    pos++;
                }
                
                if (pos < sourceCode.length() && sourceCode[pos] != '\n' && sourceCode[pos] != '\r') {
                    if (spaces > indent_stack.back()) {
                        indent_stack.push_back(spaces);
                        tokens.push_back({TokenType::FRAME_ENTER, "", 0, spaces});
                    } else if (spaces < indent_stack.back()) {
                        while (indent_stack.size() > 1 && spaces < indent_stack.back()) {
                            indent_stack.pop_back();
                            tokens.push_back({TokenType::FRAME_EXIT, "", 0, spaces});
                        }
                    }
                }
                continue;
            }
            pos++;
            continue;
        }
        
        // Engine Directives
        if (c == '@') {
            std::string text = "@";
            pos++;
            while (pos < sourceCode.length() && (isalnum(sourceCode[pos]) || sourceCode[pos] == '_')) {
                text += sourceCode[pos++];
            }
            if (text == "@C_Native") tokens.push_back({TokenType::TAG_C_NATIVE, text, 0, 0});
            else tokens.push_back({TokenType::IDENTIFIER, text, 0, 0});
            continue;
        }
        
        // ABC Keywords & Identifiers
        if (isalpha(c) || c == '_') {
            std::string text;
            while (pos < sourceCode.length() && (isalnum(sourceCode[pos]) || sourceCode[pos] == '_')) {
                text += sourceCode[pos++];
            }
            
            // Lookahead for multi-word keywords
            if (text == "HOW") {
                if (pos + 3 < sourceCode.length() && sourceCode.substr(pos, 3) == " TO") {
                    pos += 3;
                    tokens.push_back({TokenType::KW_HOW_TO, "HOW TO", 0, 0});
                    continue;
                }
            }
            
            if (text == "TO") {
                if (pos + 7 <= sourceCode.length() && sourceCode.substr(pos, 7) == " SCREEN") {
                    pos += 7;
                    tokens.push_back({TokenType::KW_TO_SCREEN, "TO SCREEN", 0, 0});
                    continue;
                }
            }
            
            TokenType type = TokenType::IDENTIFIER;
            if (text == "PUT") type = TokenType::KW_PUT;
            else if (text == "IN") type = TokenType::KW_IN;
            else if (text == "WRITE") type = TokenType::KW_WRITE;
            else if (text == "IF") type = TokenType::KW_IF;
            else if (text == "ELSE") type = TokenType::KW_ELSE;
            else if (text == "WHILE") type = TokenType::KW_WHILE;
            else if (text == "FOR") type = TokenType::KW_FOR;
            else if (text == "let") type = TokenType::KW_LET;
            else if (text == "pub") type = TokenType::KW_PUB;
            else if (text == "fn") type = TokenType::KW_FN;
            else if (text == "struct") type = TokenType::KW_STRUCT;
            else if (text == "import") type = TokenType::KW_IMPORT;
            
            tokens.push_back({type, text, 0, 0});
            continue;
        }
        
        // Numbers
        if (isdigit(c)) {
            std::string text;
            while (pos < sourceCode.length() && isdigit(sourceCode[pos])) {
                text += sourceCode[pos++];
            }
            tokens.push_back({TokenType::LITERAL_INT, text, 0, 0});
            continue;
        }
        
        // Strings
        if (c == '"') {
            std::string text;
            pos++;
            while (pos < sourceCode.length() && sourceCode[pos] != '"') {
                text += sourceCode[pos++];
            }
            pos++; // skip closing quote
            tokens.push_back({TokenType::LITERAL_STRING, text, 0, 0});
            continue;
        }
        
        // Operators
        if (c == '+') { tokens.push_back({TokenType::PUNCTUATION, "+", 0, 0}); pos++; continue; }
        if (c == '-') { tokens.push_back({TokenType::PUNCTUATION, "-", 0, 0}); pos++; continue; }
        if (c == '*') { tokens.push_back({TokenType::PUNCTUATION, "*", 0, 0}); pos++; continue; }
        if (c == '/') { tokens.push_back({TokenType::PUNCTUATION, "/", 0, 0}); pos++; continue; }
        if (c == '=') {
            if (pos + 1 < sourceCode.length() && sourceCode[pos+1] == '=') {
                tokens.push_back({TokenType::PUNCTUATION, "==", 0, 0});
                pos += 2;
            } else {
                tokens.push_back({TokenType::PUNCTUATION, "=", 0, 0});
                pos++;
            }
            continue;
        }
        
        // Brackets / Punctuation
        if (c == '(') { tokens.push_back({TokenType::PUNCTUATION, "(", 0, 0}); pos++; continue; }
        if (c == ')') { tokens.push_back({TokenType::PUNCTUATION, ")", 0, 0}); pos++; continue; }
        if (c == '{') { tokens.push_back({TokenType::PUNCTUATION, "{", 0, 0}); pos++; continue; }
        if (c == '}') { tokens.push_back({TokenType::PUNCTUATION, "}", 0, 0}); pos++; continue; }
        if (c == ':') {
            if (pos + 1 < sourceCode.length() && sourceCode[pos+1] == ':') {
                tokens.push_back({TokenType::PUNCTUATION, "::", 0, 0});
                pos += 2;
            } else {
                tokens.push_back({TokenType::PUNCTUATION, ":", 0, 0});
                pos++;
            }
            continue;
        }
        if (c == ';') { tokens.push_back({TokenType::PUNCTUATION, ";", 0, 0}); pos++; continue; }
        if (c == '.') { tokens.push_back({TokenType::PUNCTUATION, ".", 0, 0}); pos++; continue; }
        if (c == '<') { tokens.push_back({TokenType::PUNCTUATION, "<", 0, 0}); pos++; continue; }
        if (c == '>') { tokens.push_back({TokenType::PUNCTUATION, ">", 0, 0}); pos++; continue; }
        if (c == ',') { tokens.push_back({TokenType::PUNCTUATION, ",", 0, 0}); pos++; continue; }
        if (c == '[') { tokens.push_back({TokenType::PUNCTUATION, "[", 0, 0}); pos++; continue; }
        if (c == ']') { tokens.push_back({TokenType::PUNCTUATION, "]", 0, 0}); pos++; continue; }
        
        // Fallback
        tokens.push_back({TokenType::END_OF_FILE, std::string(1, c), 0, 0});
        pos++;
    }
    
    // Pop remaining indents
    while (indent_stack.size() > 1) {
        indent_stack.pop_back();
        tokens.push_back({TokenType::FRAME_EXIT, "", 0, 0});
    }
    
    tokens.push_back({TokenType::END_OF_FILE, "", 0, 0});
    return tokens;
}

} // namespace compiler
} // namespace Axiom
