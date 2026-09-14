#include <iostream>
#include <windows.h>
#include <string>

// Function pointer signature matching the C-API in axi_compiler.dll
typedef int (*compile_func_t)(const wchar_t*, const wchar_t*);

int wmain(int argc, wchar_t* argv[]) {
    if (argc < 3) {
        std::wcout << L"======================================\n";
        std::wcout << L"    Axi Compiler Demo (FOSS Edition)  \n";
        std::wcout << L"======================================\n";
        std::wcout << L"Usage: axi_compiler_demo.exe <input_file.axi> <output.exe>\n";
        std::wcout << L"This demo dynamically loads axi_compiler.dll to process the AST.\n";
        return 1;
    }

    // Attempt to load the DLL relative to the execution directory
    HMODULE hCompiler = LoadLibraryW(L"..\\components\\axi_compiler.dll");
    if (!hCompiler) {
        // Fallback for running exactly where the DLL is
        hCompiler = LoadLibraryW(L"axi_compiler.dll");
    }

    if (!hCompiler) {
        std::wcout << L"[Error] Could not locate axi_compiler.dll. Ensure it is in the components/ directory.\n";
        return 1;
    }

    // Hook the exported C function
    compile_func_t axi_compile = (compile_func_t)GetProcAddress(hCompiler, "axi_compile_file");
    if (!axi_compile) {
        std::wcout << L"[Error] Found DLL, but could not map 'axi_compile_file' interface.\n";
        FreeLibrary(hCompiler);
        return 1;
    }

    std::wcout << L"[Demo] Invoking compiler engine on: " << argv[1] << L"\n";
    
    // Execute the DLL function
    int result = axi_compile(argv[1], argv[2]);
    
    if (result == 0) {
        std::wcout << L"[Demo] Compilation successful! Output generated: " << argv[2] << L"\n";
    } else {
        std::wcout << L"[Demo] Compilation failed with internal Axi code: " << result << L"\n";
    }

    FreeLibrary(hCompiler);
    return result;
}
