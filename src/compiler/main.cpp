#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "lexer.h"
#include "parser.h"
#include "analyzer.h"
#include "emitter.h"
#ifdef AXIOM_INTERNAL_BUILD
#include "dvcs.h"
#endif
#include <filesystem>

using namespace axiom::compiler;

void compileFile(const std::string& filepath, const std::string& exe_path) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filepath << "\n";
        return;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    
    std::cout << "Compiling " << filepath << "...\n";
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    PrattParser parser(tokens);
    auto module = parser.parseModule();
    
    SemanticAnalyzer analyzer;
    analyzer.analyze(module);
    
    C23Emitter emitter;
    std::string c_code = emitter.emit(module);
    
    std::string out_c_file = "build_output.c";
    std::ofstream out_file(out_c_file);
    out_file << c_code;
    out_file.close();
    
    std::cout << "[Orchestrator] Emitted " << out_c_file << ".\n";
    
    // Get executable path to find adjacent lib/ folder
    std::string base_dir = "";
    size_t last_slash = exe_path.find_last_of("\\/");
    if (last_slash != std::string::npos) {
        base_dir = exe_path.substr(0, last_slash) + "/";
    }
    
    // Compile using GCC, linking Raylib, Llama.cpp, and the core DAG C implementation
    std::string lib_dir = base_dir + "lib";
    std::string dag_native_src = lib_dir + "/core/dag_native.c";
    std::string gcc_path = "C:\\Ethos\\ethos-logos\\LANG\\src\\compiler\\bootstrap\\python_to_c_compiler\\bin\\mingw64\\bin\\g++.exe";
    std::string raylib_lib = lib_dir + "/raylib-5.0_win64_mingw-w64/lib";
    std::string raylib_inc = lib_dir + "/raylib-5.0_win64_mingw-w64/include";
    std::string gcc_cmd = gcc_path + " " + out_c_file + " \"" + lib_dir + "/core/dag_native.c\" \"" + lib_dir + "/core/terminal_native.c\" \"" + lib_dir + "/core/tui_native.c\" -o app.exe -L \"" + raylib_lib + "\" -L \"" + lib_dir + "/core\" -lraylib -lgdi32 -lwinmm -I \"" + lib_dir + "\" -I \"" + raylib_inc + "\"";
    std::cout << "[Orchestrator] Running: " << gcc_cmd << "\n";
    
    int result = std::system(gcc_cmd.c_str());
    if (result == 0) {
        std::cout << "[SUCCESS] Build completed: app.exe\n";
    } else {
        std::cerr << "[ERROR] GCC Compilation failed with code " << result << "\n";
    }
}



int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Axiom Compiler Engine (Internal Orchestrator)\n";
        std::cout << "Usage: axiomc <command> [args]\n";
        std::cout << "Commands:\n";
        std::cout << "  build <file.axi>   Compile Axi code to C23 binary\n";
        std::cout << "  init               Initialize TOON ledger\n";
        std::cout << "  track <path>       Track directory in DAG\n";
        std::cout << "  wrap               Snapshot current state\n";
        std::cout << "  ship               Commit and release to production\n";
        return 1;
    }
    
    std::string command = argv[1];
    std::string current_dir = std::filesystem::current_path().string();
    std::string exe_path = argv[0];
    
    if (command == "build") {
        if (argc < 3) {
            std::cerr << "Usage: axiomc build <file.axi>\n";
            return 1;
        }
        try {
            compileFile(argv[2], exe_path);
        } catch (const std::exception& e) {
            std::cerr << "Compilation Error: " << e.what() << "\n";
            return 1;
        }
    } else if (command == "init") {
#ifdef AXIOM_INTERNAL_BUILD
        axiom::dvcs::init(current_dir);
#else
        std::cerr << "Command 'init' is only available in the internal Ethos build.\n";
        return 1;
#endif
    } else if (command == "track") {
        if (argc < 3) {
            std::cerr << "Usage: axiomc track <path>\n";
            return 1;
        }
#ifdef AXIOM_INTERNAL_BUILD
        axiom::dvcs::track(current_dir, argv[2]);
#else
        std::cerr << "Command 'track' is only available in the internal Ethos build.\n";
        return 1;
#endif
    } else if (command == "wrap") {
#ifdef AXIOM_INTERNAL_BUILD
        axiom::dvcs::wrap(current_dir);
#else
        std::cerr << "Command 'wrap' is only available in the internal Ethos build.\n";
        return 1;
#endif
    } else if (command == "ship") {
#ifdef AXIOM_INTERNAL_BUILD
        axiom::dvcs::ship(current_dir);
#else
        std::cerr << "Command 'ship' is only available in the internal Ethos build.\n";
        return 1;
#endif
    } else {
        std::cerr << "Unknown command: " << command << "\n";
        return 1;
    }
    
    return 0;
}
