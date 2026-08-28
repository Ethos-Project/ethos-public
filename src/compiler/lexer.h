#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

namespace Axiom {

enum class TokenType {
    // Standard Types & Identifiers
    IDENTIFIER,
    LITERAL_INT, LITERAL_FLOAT, LITERAL_STRING, LITERAL_BOOL,

    // Axiom Unique Primitives
    TYPE_STRUCT,
    TYPE_PRISMATIC_ARRAY,

    // Spatial Execution Frames (from Pythonic Indentation)
    FRAME_ENTER, // Replaces '{' or visual bounding box entrance
    FRAME_EXIT,  // Replaces '}' or visual bounding box exit

    // Routing Operators
    WIRE_OP,       // "->" Routes data from left to right
    ASYNC_FORK,    // "||>"
    SYNC_BARRIER,  // "==>"

    // Standard Control Flow
    KW_IF, KW_ELSE, KW_FOR, KW_WHILE, KW_NODE, KW_STATE,

    // Engine Directives (Ingestion Tags)
    TAG_C_NATIVE,   // @C_Native
    TAG_SQL,        // @SQL
    TAG_STYLUS,     // @Stylus
    TAG_HTML,       // @HTML
    TAG_ZERO_INIT,  // @Zero_Init
    TAG_COMPUTE,    // @Compute

    // Intrinsics
    TOKEN_STD_MATH,
    TOKEN_STD_PG,
    TOKEN_STD_CHRONOS,

    PUNCTUATION, // brackets, colons, etc.
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int indentation_level; // Maps text depth to visual GUI depth
};

class Lexer {
public:
    Lexer(const std::string& source) : sourceCode(source) {}
    std::vector<Token> tokenize();

private:
    std::string sourceCode;
    int current_indent = 0;
};

} // namespace Axiom

#endif // LEXER_H
