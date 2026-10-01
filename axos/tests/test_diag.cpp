#include <iostream>
#include <cassert>
#include <string>
#include "allos_lexer.h"
#include "allos_parser.h"

using namespace axi::compiler;

int main() {
    std::cout << "[TEST] Verifying Deprecation Diagnostic for environments without stdlib..." << std::endl;
    std::string test_verbs[] = {
        "PRINT", "READ", "COPY", "MOVE", "ZIP", "REMOVE",
        "SEARCH", "SELECT", "FIND", "PASTE",
        "GRAPH", "MERGE",
        "RUN", "SERVE",
        "MAKE", "COMPILE", "TEST", "DEPLOY", "INSTALL", "UPDATE", "ANALYZE"
    };

    for (const auto& verb : test_verbs) {
        std::string src = verb + " \"arg\"\n";
        Lexer lex(src);
        auto toks = lex.tokenize();
        // Parse with is_stdlib_parse = true to simulate an environment WITHOUT stdlib loaded
        PrattParser parser(toks, true);
        try {
            parser.parseModule();
            std::cerr << "FAIL: Expected exception for verb " << verb << std::endl;
            return 1;
        } catch (const std::exception& e) {
            std::string msg = e.what();
            if (msg.find("[AXOS DEPRECATION]") == std::string::npos) {
                std::cerr << "FAIL: Did not get [AXOS DEPRECATION] for " << verb << ": " << msg << std::endl;
                return 1;
            }
            std::cout << "PASS: " << verb << " -> " << msg << std::endl;
        }
    }

    std::cout << "ALL 21 VERBS EMIT ACTIONABLE DEPRECATION DIAGNOSTIC!" << std::endl;
    return 0;
}
