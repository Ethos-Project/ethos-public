import re

with open('compiler.axi', 'r', encoding='utf-8') as f:
    code = f.read()

# 1. Update load_source_file
code = re.sub(
    r'const char\* target_file = "domus_genesis.axi";',
    r'if (g_argc < 3) { printf("Usage: axi_compiler <input.axi> <output.exe>\\n"); compile_status = false; return; }\n    const char* target_file = g_argv[1];',
    code
)

# 2. Update emit_c_code main() generation
code = code.replace(
    r'fprintf(f, "int main() {\n");',
    r'fprintf(f, "int g_argc; char** g_argv;\\nint main(int argc, char** argv) {\\n    g_argc = argc; g_argv = argv;\\n");'
)

# 3. Update compile_binary system call
code = re.sub(
    r'system\("C:\\\\\\\\Antigravity\\\\\\\\cogni-core\\\\\\\\tools.*?"\);',
    r'char cmd[2048];\n    sprintf(cmd, "C:\\\\\\\\Antigravity\\\\\\\\cogni-core\\\\\\\\tools\\\\\\\\python_to_c_compiler\\\\\\\\bin\\\\\\\\mingw64\\\\\\\\bin\\\\\\\\gcc.exe output.c -o %s -I.\\\\\\\\raylib-5.0_win64_mingw-w64\\\\\\\\include -L.\\\\\\\\raylib-5.0_win64_mingw-w64\\\\\\\\lib -I.\\\\\\\\lib\\\\\\\\llama.cpp\\\\\\\\include -L.\\\\\\\\lib\\\\\\\\llama.cpp\\\\\\\\build\\\\\\\\src -lraylib -lgdi32 -lwinmm -lllama -lws2_32", g_argv[2]);\n    system(cmd);',
    code
)

# 4. Remove all hardcoded domus_genesis print statements
code = code.replace('Parsed domus_genesis.axi and generated output.c', 'Parsed input file and generated output.c')
code = code.replace('Invoking MinGW GCC natively to build domus_genesis', 'Invoking MinGW GCC natively to build executable')
code = code.replace('FULL BOOTSTRAP COMPLETE! domus_genesis.exe generated.', 'FULL BOOTSTRAP COMPLETE! Executable generated.')

# 5. Provide externs for g_argc and g_argv in compiler.axi itself so the C compiler doesn't complain when compiling compiler.axi
# Wait, actually we can just put it in a C_Native at the top
code = '@C_Native("\\nextern int g_argc;\\nextern char** g_argv;\\n")\n' + code

with open('compiler.axi', 'w', encoding='utf-8') as f:
    f.write(code)
