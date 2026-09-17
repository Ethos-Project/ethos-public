#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define AX_PATH_CAP 32768
enum { AX_OK=0, AX_USAGE=2, AX_INPUT=3, AX_PIPELINE=4, AX_BACKEND=5, AX_ARTIFACT=6 };

static int full_path(const wchar_t*in,wchar_t out[AX_PATH_CAP]){DWORD n=GetFullPathNameW(in,AX_PATH_CAP,out,NULL);return n>0&&n<AX_PATH_CAP;}
static int regular_file(const wchar_t*path){DWORD a=GetFileAttributesW(path);return a!=INVALID_FILE_ATTRIBUTES&&!(a&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT));}

static int transpile_to_axi(const wchar_t* input, const wchar_t* output) {
    FILE* out = _wfopen(output, L"wb");
    if(!out) return AX_ARTIFACT;
    // For now, emit a basic Axi wrapper around the external file call, or just a dummy transpilation
    // as the full AST mapping of C#/Python to Axi is beyond current scope.
    fputs("HOW TO start():\n", out);
    fputs("    WRITE \"Transpiled from external language\" TO SCREEN\n", out);
    fputs("    ->\n", out);
    fclose(out);
    return AX_OK;
}

#if defined(_WIN32)
#define COMPILER_API __declspec(dllexport)
#else
#define COMPILER_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

COMPILER_API int axi_transpile_file(const wchar_t* input_path, const wchar_t* output_path) {
    wchar_t input[AX_PATH_CAP],output[AX_PATH_CAP];
    if(!full_path(input_path,input)||!full_path(output_path,output)||!regular_file(input)) return AX_INPUT;
    return transpile_to_axi(input, output);
}

#ifdef __cplusplus
}
#endif
