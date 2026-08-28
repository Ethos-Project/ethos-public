import ast
import sys
import os
import zipfile
import urllib.request
import pathlib
import subprocess

BIN_DIR = pathlib.Path(__file__).parent / "bin"
LIB_DIR = pathlib.Path(__file__).parent / "lib"


def find_gcc(base_dir):
    for root, dirs, files in os.walk(base_dir):
        if "gcc.exe" in files:
            return pathlib.Path(root) / "gcc.exe"
    return None


def ensure_compiler():
    gcc_bin = find_gcc(BIN_DIR)
    if gcc_bin and gcc_bin.exists():
        return gcc_bin
    url = "https://github.com/brechtsanders/winlibs_mingw/releases/download/16.2.0posix-14.0.0-ucrt-r1/winlibs-x86_64-posix-seh-gcc-16.2.0-mingw-w64ucrt-14.0.0-r1.zip"
    zip_path = BIN_DIR / "gcc.zip"
    BIN_DIR.mkdir(parents=True, exist_ok=True)
    urllib.request.urlretrieve(url, zip_path)
    with zipfile.ZipFile(zip_path, "r") as zip_ref:
        zip_ref.extractall(BIN_DIR)
    zip_path.unlink()
    return find_gcc(BIN_DIR)


def ensure_cjson():
    cjson_dir = LIB_DIR / "cJSON"
    cjson_dir.mkdir(parents=True, exist_ok=True)
    h_file = cjson_dir / "cJSON.h"
    c_file = cjson_dir / "cJSON.c"
    if not h_file.exists():
        urllib.request.urlretrieve(
            "https://raw.githubusercontent.com/DaveGamble/cJSON/master/cJSON.h", h_file
        )
    if not c_file.exists():
        urllib.request.urlretrieve(
            "https://raw.githubusercontent.com/DaveGamble/cJSON/master/cJSON.c", c_file
        )
    return cjson_dir


def ensure_libcurl():
    curl_dir = LIB_DIR / "curl"
    lib_file = curl_dir / "lib" / "libcurl.a"
    include_dir = curl_dir / "include" / "curl"
    if not lib_file.exists() or not include_dir.exists():
        print("Downloading static libcurl for MinGW...")
        url = "https://curl.se/windows/dl-8.21.0_7/curl-8.21.0_7-win64-mingw.zip"
        zip_path = LIB_DIR / "curl.zip"
        LIB_DIR.mkdir(parents=True, exist_ok=True)
        urllib.request.urlretrieve(url, zip_path)
        with zipfile.ZipFile(zip_path, "r") as zip_ref:
            zip_ref.extractall(curl_dir)
        zip_path.unlink()

        # The zip extracts to a subfolder like curl-8.21.0_7-win64-mingw
        # Move lib, include and bin up if needed
        extracted_base = curl_dir / "curl-8.21.0_7-win64-mingw"
        if extracted_base.exists():
            import shutil

            if (curl_dir / "lib").exists():
                shutil.rmtree(curl_dir / "lib")
            if (curl_dir / "include").exists():
                shutil.rmtree(curl_dir / "include")
            if (curl_dir / "bin").exists():
                shutil.rmtree(curl_dir / "bin")
            shutil.move(str(extracted_base / "lib"), str(curl_dir / "lib"))
            shutil.move(str(extracted_base / "include"), str(curl_dir / "include"))
            shutil.move(str(extracted_base / "bin"), str(curl_dir / "bin"))
            shutil.rmtree(str(extracted_base))
    return curl_dir


class CortexVisitor(ast.NodeVisitor):
    def generic_visit(self, node):
        return "/* unhandled_node */"

    def __init__(self, module_name):
        self.module_name = module_name

        self.h_code = []
        self.h_code.append("#pragma once")
        self.h_code.append("#include <stdint.h>")
        self.h_code.append("#include <stdbool.h>")
        self.h_code.append('#include "cJSON.h"')
        self.h_code.append("")

        self.c_code = []
        self.c_code.append("/* Hybrid Compiler Auto-Generated C Code */")
        self.c_code.append("#include <stdio.h>")
        self.c_code.append("#include <stdint.h>")
        self.c_code.append("#include <stdbool.h>")
        self.c_code.append("#include <stdlib.h>")
        self.c_code.append('#include "arena.h"')
        self.c_code.append('#include "cJSON.h"')
        self.c_code.append('#include "eros_bus.h"')
        self.c_code.append(f'#include "{module_name}.h"')
        self.c_code.append("")
        self.c_code.append("extern arena_t cortex_arena;")
        self.c_code.append("")
        self.declared_vars = set()
        self.in_async = False
        self.async_state = 1

    def visit_Module(self, node):
        for stmt in node.body:
            if not isinstance(stmt, ast.ClassDef):
                self.visit(stmt)

    def visit_Name(self, node):
        if node.id == "True":
            return "true"
        if node.id == "False":
            return "false"
        if node.id == "None":
            return "NULL"
        if getattr(self, "in_async", False) and node.id in ["i", "event"]:
            return f"ctx->{node.id}"
        return node.id

    def visit_Constant(self, node):
        if isinstance(node.value, str):
            escaped = (
                node.value.replace("\\", "\\\\")
                .replace('"', '\\"')
                .replace("\n", "\\n")
                .replace("\r", "\\r")
            )
            return f'"{escaped}"'
        elif isinstance(node.value, bool):
            return "true" if node.value else "false"
        elif node.value is None:
            return "NULL"
        else:
            return str(node.value)

    def visit_Compare(self, node):
        left = self.visit(node.left)
        ops = []
        for op, comp in zip(node.ops, node.comparators):
            if isinstance(op, ast.Eq):
                op_str = "=="
            elif isinstance(op, ast.NotEq):
                op_str = "!="
            elif isinstance(op, ast.Lt):
                op_str = "<"
            elif isinstance(op, ast.LtE):
                op_str = "<="
            elif isinstance(op, ast.Gt):
                op_str = ">"
            elif isinstance(op, ast.GtE):
                op_str = ">="
            elif isinstance(op, ast.Is):
                op_str = "=="
            elif isinstance(op, ast.IsNot):
                op_str = "!="
            else:
                op_str = "=="
            right = self.visit(comp)
            ops.append(f"{op_str} {right}")
        return f"{left} {' '.join(ops)}"

    def visit_BinOp(self, node):
        left = self.visit(node.left)
        right = self.visit(node.right)
        if isinstance(node.op, ast.Add):
            op = "+"
        elif isinstance(node.op, ast.Sub):
            op = "-"
        elif isinstance(node.op, ast.Mult):
            op = "*"
        elif isinstance(node.op, ast.Div):
            op = "/"
        else:
            op = "+"
        return f"{left} {op} {right}"

    def visit_If(self, node):
        cond = self.visit(node.test)
        self.c_code.append(f"    if ({cond}) {{")
        for stmt in node.body:
            self.visit(stmt)
        if node.orelse:
            self.c_code.append("    } else {")
            for stmt in node.orelse:
                self.visit(stmt)
        self.c_code.append("    }")

    def visit_While(self, node):
        cond = self.visit(node.test)
        self.c_code.append(f"    while ({cond}) {{")
        for stmt in node.body:
            self.visit(stmt)
        self.c_code.append("    }")

    def visit_For(self, node):
        if (
            isinstance(node.iter, ast.Call)
            and getattr(node.iter.func, "id", "") == "range"
        ):
            target = self.visit(node.target)
            args = [self.visit(a) for a in node.iter.args]
            start = "0"
            end = args[0] if len(args) == 1 else args[1]
            self.c_code.append(
                f"    for (int64_t {target} = {start}; {target} < {end}; {target}++) {{"
            )
            for stmt in node.body:
                self.visit(stmt)
            self.c_code.append("    }")

    def visit_Expr(self, node):
        if isinstance(node.value, ast.Call):
            res = self.visit_Call(node.value, is_expr=True)
            if res:
                self.c_code.append(f"    {res};")
        elif isinstance(node.value, ast.Await):
            self.visit_Await(node.value)

    def visit_Call(self, node, is_expr=False):
        is_signal_bus = False
        if isinstance(node.func, ast.Attribute):
            if (
                isinstance(node.func.value, ast.Name)
                and node.func.value.id == "SignalBus"
            ):
                if node.func.attr == "emit":
                    is_signal_bus = True

        if is_signal_bus:
            event_arg = self.visit(node.args[0])
            self.c_code.append("    // Lowered SignalBus.emit C Logic")
            self.c_code.append("    uint32_t current_head = eros_bus->ring_head;")
            self.c_code.append("    uint32_t slot_size = 256;")
            self.c_code.append("    uint32_t buffer_capacity = 968;")
            self.c_code.append(
                "    char* target_slot = (char*)(eros_bus->event_ring_buffer + current_head);"
            )
            self.c_code.append(
                f"    cJSON_PrintPreallocated({event_arg}, target_slot, slot_size, 0);"
            )
            self.c_code.append(
                "    eros_bus->ring_head = (current_head + slot_size) % buffer_capacity;"
            )
            self.c_code.append(
                f'    printf("[FFI] SignalBus.emit written to ring buffer at offset %u\\n", current_head);'
            )
            return None
        else:
            func_name = (
                self.visit(node.func)
                if not isinstance(node.func, ast.Name)
                else node.func.id
            )
            args = ", ".join(self.visit(a) for a in node.args)
            return f"{func_name}({args})"

    def visit_Assign(self, node):
        target = node.targets[0]
        if isinstance(node.value, ast.Dict):
            if isinstance(target, ast.Name):
                dict_name = self.visit(target)
                raw_name = target.id
                if raw_name not in self.declared_vars:
                    if getattr(self, "in_async", False):
                        self.c_code.append(f"    {dict_name} = cJSON_CreateObject();")
                    else:
                        self.c_code.append(
                            f"    cJSON* {dict_name} = cJSON_CreateObject();"
                        )
                    self.declared_vars.add(raw_name)
                else:
                    self.c_code.append(f"    {dict_name} = cJSON_CreateObject();")
                for k, v in zip(node.value.keys, node.value.values):
                    key_str = self.visit(k).strip('"')
                    val_str = self.visit(v).strip('"')
                    self.c_code.append(
                        f'    cJSON_AddStringToObject({dict_name}, "{key_str}", "{val_str}");'
                    )
        elif isinstance(target, ast.Name):
            raw_name = target.id
            var_name = self.visit(target)
            val = self.visit(node.value)
            if raw_name not in self.declared_vars:
                if isinstance(node.value, ast.Constant):
                    if isinstance(node.value.value, str):
                        self.c_code.append(
                            f"    char* {var_name} = {val};"
                            if not getattr(self, "in_async", False)
                            else f"    {var_name} = {val};"
                        )
                    elif isinstance(node.value.value, bool):
                        self.c_code.append(
                            f"    bool {var_name} = {val};"
                            if not getattr(self, "in_async", False)
                            else f"    {var_name} = {val};"
                        )
                    else:
                        self.c_code.append(
                            f"    int64_t {var_name} = {val};"
                            if not getattr(self, "in_async", False)
                            else f"    {var_name} = {val};"
                        )
                elif isinstance(node.value, ast.BinOp):
                    self.c_code.append(
                        f"    int64_t {var_name} = {val};"
                        if not getattr(self, "in_async", False)
                        else f"    {var_name} = {val};"
                    )
                self.declared_vars.add(raw_name)
            else:
                self.c_code.append(f"    {var_name} = {val};")
        else:
            self.generic_visit(node)

    def visit_FunctionDef(self, node):
        args = []
        for arg in node.args.args:
            if "event" in arg.arg or "dict" in arg.arg or "state" in arg.arg:
                args.append(f"cJSON* {arg.arg}")
            else:
                args.append(f"int64_t {arg.arg}")
        arg_str = ", ".join(args)

        ret_type = "void"
        for stmt in ast.walk(node):
            if isinstance(stmt, ast.Return):
                if stmt.value:
                    ret_type = "int64_t"

        self.h_code.append(f"{ret_type} {node.name}({arg_str});")
        self.c_code.append(f"{ret_type} {node.name}({arg_str}) {{")
        for stmt in node.body:
            self.visit(stmt)
        self.c_code.append("}")
        self.c_code.append("")

    def visit_Return(self, node):
        if node.value:
            val = self.visit(node.value)
            self.c_code.append(f"    return {val};")
        else:
            self.c_code.append("    return;")

    def visit_AsyncFunctionDef(self, node):
        self.in_async = True
        self.async_state = 1

        self.h_code.append(f"typedef struct {{")
        self.h_code.append(f"    int state;")
        self.h_code.append(f"    int64_t i;")
        self.h_code.append(f"    cJSON* event;")
        self.h_code.append(f"}} {node.name}_ctx_t;")
        self.h_code.append("")
        self.h_code.append(f"void {node.name}_step({node.name}_ctx_t* ctx);")
        self.h_code.append("")

        self.c_code.append(f"void {node.name}_step({node.name}_ctx_t* ctx) {{")
        self.c_code.append("    // Two-Way Kernel Safety Interlock")
        self.c_code.append(
            "    if (eros_bus->Axi_status == 3 /* UNSAFE */ || eros_bus->Axi_status == 4 /* EMERGENCY_SHUTDOWN */) {"
        )
        self.c_code.append(
            '        printf("[CORTEX INTERLOCK] Execution halted by .axi kernel signal.\\n");'
        )
        self.c_code.append("        return;")
        self.c_code.append("    }")
        self.c_code.append("    switch (ctx->state) {")
        self.c_code.append("        case 0:")

        for stmt in node.body:
            self.visit(stmt)

        self.c_code.append("        default:")
        self.c_code.append("            ctx->state = -1; // Done")
        self.c_code.append("            break;")
        self.c_code.append("    }")
        self.c_code.append("}")
        self.in_async = False

    def visit_Await(self, node):
        state = self.async_state
        self.async_state += 1
        self.c_code.append(f"            ctx->state = {state};")
        self.c_code.append(f"            return;")
        self.c_code.append(f"        case {state}: ;")
        return None

    def generate_h(self):
        return "\n".join(self.h_code)

    def generate_c(self):
        return "\n".join(self.c_code)


class ASTFilterVisitor(ast.NodeVisitor):
    def __init__(self):
        self.should_skip = False
        self.in_function = False

    def visit_FunctionDef(self, node):
        old = self.in_function
        self.in_function = True
        self.generic_visit(node)
        self.in_function = old

    def visit_AsyncFunctionDef(self, node):
        old = self.in_function
        self.in_function = True
        if not node.name.endswith("_step") and "test_async" not in node.name:
            self.should_skip = True
        self.generic_visit(node)
        self.in_function = old

    def visit_Await(self, node):
        self.should_skip = True
        self.generic_visit(node)

    def visit_Import(self, node):
        if self.in_function:
            self.should_skip = True
        self.generic_visit(node)

    def visit_ImportFrom(self, node):
        if self.in_function:
            self.should_skip = True
        self.generic_visit(node)


def ensure_dotnet():
    dotnet_dir = BIN_DIR / "dotnet"
    dotnet_exe = dotnet_dir / "dotnet.exe"
    if dotnet_exe.exists():
        return dotnet_exe
    url = "https://builds.dotnet.microsoft.com/dotnet/Sdk/8.0.204/dotnet-sdk-8.0.204-win-x64.zip"
    zip_path = BIN_DIR / "dotnet.zip"
    dotnet_dir.mkdir(parents=True, exist_ok=True)
    print("[JIT] Bootstrapping .NET SDK (this may take a minute)...")
    urllib.request.urlretrieve(url, zip_path)
    import zipfile

    with zipfile.ZipFile(zip_path, "r") as zip_ref:
        zip_ref.extractall(dotnet_dir)
    zip_path.unlink()
    return dotnet_exe


class CSharpVisitor(ast.NodeVisitor):
    def __init__(self, module_name):
        self.module_name = module_name
        self.cs_code = []
        self.cs_code.append("using System;")
        self.cs_code.append("using System.Runtime.InteropServices;")
        self.cs_code.append("using System.Threading.Tasks;")
        self.cs_code.append("")
        self.cs_code.append("namespace Cortex.Modules")
        self.cs_code.append("{")
        self.cs_code.append("    [StructLayout(LayoutKind.Explicit, Size = 16)]")
        self.cs_code.append("    public struct AxiValue {")
        self.cs_code.append("        [FieldOffset(0)] public uint Type;")
        self.cs_code.append("        [FieldOffset(4)] public uint Flags;")
        self.cs_code.append("        [FieldOffset(8)] public long IntVal;")
        self.cs_code.append("        [FieldOffset(8)] public double FloatVal;")
        self.cs_code.append("        [FieldOffset(8)] public bool BoolVal;")
        self.cs_code.append("        [FieldOffset(8)] public ulong HeapOffset;")
        self.cs_code.append("    }")
        self.cs_code.append("")
        self.cs_code.append("    public static unsafe class EulerPool {")
        self.cs_code.append(
            "        public static AxiValue AllocateString(string s) { return new AxiValue { Type = 4, HeapOffset = 0 }; }"
        )
        self.cs_code.append("    }")
        self.cs_code.append("")
        self.cs_code.append(
            f"    public unsafe class {module_name.capitalize()}_Module"
        )
        self.cs_code.append("    {")

    def generate_cs(self):
        self.cs_code.append("    }")
        self.cs_code.append("}")
        return "\n".join(self.cs_code)

    def generic_visit(self, node):
        return "new AxiValue() /* unhandled */"

    def visit_Module(self, node):
        for stmt in node.body:
            self.visit(stmt)

    def visit_Name(self, node):
        return node.id

    def visit_Constant(self, node):
        if isinstance(node.value, str):
            val = node.value.replace('"', '"')
            return f'EulerPool.AllocateString("{val}")'
        elif node.value is None:
            return "new AxiValue { Type = 0 }"
        elif isinstance(node.value, bool):
            b = "true" if node.value else "false"
            return f"new AxiValue {{ Type = 3, BoolVal = {b} }}"
        elif isinstance(node.value, int):
            return f"new AxiValue {{ Type = 1, IntVal = {node.value} }}"
        elif isinstance(node.value, float):
            return f"new AxiValue {{ Type = 2, FloatVal = {node.value} }}"
        return "new AxiValue()"

    def visit_Call(self, node):
        func = (
            self.visit(node.func)
            if not isinstance(node.func, ast.Name)
            else node.func.id
        )
        args = ", ".join(self.visit(a) for a in node.args)
        return f"{func}(/* {args} */ null, 0)"

    def visit_Expr(self, node):
        val = self.visit(node.value)
        if val:
            self.cs_code.append(f"            {val};")

    def visit_Assign(self, node):
        target = node.targets[0]
        t_name = self.visit(target)
        val = self.visit(node.value)
        if not hasattr(self, "declared_vars"):
            self.declared_vars = set()
        if t_name not in self.declared_vars:
            self.cs_code.append(f"            AxiValue {t_name} = {val};")
            self.declared_vars.add(t_name)
        else:
            self.cs_code.append(f"            {t_name} = {val};")

    def visit_Return(self, node):
        if node.value:
            val = self.visit(node.value)
            self.cs_code.append(f"            return {val};")
        else:
            self.cs_code.append("            return new AxiValue { Type = 0 };")

    def visit_Await(self, node):
        val = self.visit(node.value)
        return f"/* await */ {val}"

    def visit_FunctionDef(self, node):
        self.cs_code.append(
            f"        public unsafe AxiValue {node.name}(AxiValue* args, int argCount) {{"
        )
        self.declared_vars = set()
        for stmt in node.body:
            self.visit(stmt)
        if not any(isinstance(s, ast.Return) for s in node.body):
            self.cs_code.append("            return new AxiValue { Type = 0 };")
        self.cs_code.append("        }")

    def visit_AsyncFunctionDef(self, node):
        self.cs_code.append(
            f"        public unsafe AxiValue {node.name}_Async(AxiValue* args, int argCount) {{"
        )
        self.declared_vars = set()
        for stmt in node.body:
            self.visit(stmt)
        if not any(isinstance(s, ast.Return) for s in node.body):
            self.cs_code.append("            return new AxiValue { Type = 0 };")
        self.cs_code.append("        }")


class CppVisitor(ast.NodeVisitor):
    def __init__(self, module_name):
        self.module_name = module_name
        self.cpp_code = []
        self.hpp_code = []
        self.hpp_code.append("#pragma once")
        self.hpp_code.append("#include <any>")
        self.hpp_code.append("#include <string>")
        self.hpp_code.append("#include <vector>")
        self.hpp_code.append("#include <unordered_map>")
        self.hpp_code.append("#include <memory>")
        self.hpp_code.append("")
        self.hpp_code.append(f"namespace cortex::{module_name} {{")
        self.hpp_code.append(f"    class {module_name.capitalize()}Module {{")
        self.hpp_code.append("    public:")
        self.cpp_code.append(f'#include "{module_name}.hpp"')
        self.cpp_code.append("")
        self.cpp_code.append(f"namespace cortex::{module_name} {{")

    def generate_hpp(self):
        self.hpp_code.append("    };")
        self.hpp_code.append("}")
        return "\n".join(self.hpp_code)

    def generate_cpp(self):
        self.cpp_code.append("}")
        return "\n".join(self.cpp_code)

    def generic_visit(self, node):
        return "std::any() /* unhandled */"

    def visit_Module(self, node):
        for stmt in node.body:
            self.visit(stmt)

    def visit_Name(self, node):
        return node.id

    def visit_Constant(self, node):
        if isinstance(node.value, str):
            val = node.value.replace('"', '\\"')
            return f'"{val}"'
        elif node.value is None:
            return "std::any()"
        elif isinstance(node.value, bool):
            return "true" if node.value else "false"
        return str(node.value)

    def visit_Call(self, node):
        func = (
            self.visit(node.func)
            if not isinstance(node.func, ast.Name)
            else node.func.id
        )
        args = ", ".join(self.visit(a) for a in node.args)
        return f"{func}({args})"

    def visit_Expr(self, node):
        val = self.visit(node.value)
        if val:
            self.cpp_code.append(f"        {val};")

    def visit_Assign(self, node):
        target = node.targets[0]
        t_name = self.visit(target)
        val = self.visit(node.value)
        if not hasattr(self, "declared_vars"):
            self.declared_vars = set()
        if t_name not in self.declared_vars:
            self.cpp_code.append(f"        auto {t_name} = {val};")
            self.declared_vars.add(t_name)
        else:
            self.cpp_code.append(f"        {t_name} = {val};")

    def visit_Return(self, node):
        if node.value:
            val = self.visit(node.value)
            self.cpp_code.append(f"        return {val};")
        else:
            self.cpp_code.append("        return std::any();")

    def visit_Await(self, node):
        val = self.visit(node.value)
        return f"/* co_await */ {val}"

    def visit_FunctionDef(self, node):
        self.hpp_code.append(
            f"        std::any {node.name}(std::vector<std::any> args);"
        )
        self.cpp_code.append(
            f"    std::any {self.module_name.capitalize()}Module::{node.name}(std::vector<std::any> args) {{"
        )
        self.declared_vars = set()
        for stmt in node.body:
            self.visit(stmt)
        if not any(isinstance(s, ast.Return) for s in node.body):
            self.cpp_code.append("        return std::any();")
        self.cpp_code.append("    }")

    def visit_AsyncFunctionDef(self, node):
        self.hpp_code.append(
            f"        std::any {node.name}_async(std::vector<std::any> args);"
        )
        self.cpp_code.append(
            f"    std::any {self.module_name.capitalize()}Module::{node.name}_async(std::vector<std::any> args) {{"
        )
        self.declared_vars = set()
        for stmt in node.body:
            self.visit(stmt)
        if not any(isinstance(s, ast.Return) for s in node.body):
            self.cpp_code.append("        return std::any();")
        self.cpp_code.append("    }")


def write_axi_file(out_path, runtime, payload_path, module_name):
    import struct, zlib

    if not payload_path.exists():
        return
    with open(payload_path, "rb") as f:
        payload = f.read()
    checksum = zlib.crc32(payload) & 0xFFFFFFFF
    payload_size = len(payload)
    header = struct.pack(
        "<I H H I I I Q 64s",
        0x31495841,
        1,
        0,
        runtime,
        92,
        payload_size,
        checksum,
        module_name.encode("utf-8"),
    )
    with open(out_path, "wb") as f:
        f.write(header)
        f.write(payload)


def batch_transpile_csharp(directory_path, dotnet_exe):
    import subprocess
    import pathlib

    build_dir = pathlib.Path(__file__).parent / "build_cs"
    build_dir.mkdir(parents=True, exist_ok=True)
    py_files = list(pathlib.Path(directory_path).rglob("*.py"))
    for py_file in py_files:
        if py_file.name == "__init__.py" or py_file.name == "cortex_bridge.py":
            continue
        try:
            with open(py_file, "r", encoding="utf-8") as f:
                source = f.read()
            tree = ast.parse(source)
            base_name = py_file.stem
            visitor = CSharpVisitor(base_name)
            visitor.visit(tree)
            with open(build_dir / f"{base_name}.cs", "w", encoding="utf-8") as f:
                f.write(visitor.generate_cs())
        except Exception as e:
            pass
    csproj_content = """<Project Sdk=\"Microsoft.NET.Sdk\">\n  <PropertyGroup>\n    <TargetFramework>net8.0</TargetFramework>\n    <ImplicitUsings>enable</ImplicitUsings>\n    <Nullable>enable</Nullable>\n  </PropertyGroup>\n</Project>"""
    with open(build_dir / "cortex.csproj", "w", encoding="utf-8") as f:
        f.write(csproj_content)
    subprocess.run(
        [str(dotnet_exe), "build", "cortex.csproj", "-c", "Release"], cwd=str(build_dir)
    )
    dll_path = build_dir / "bin" / "Release" / "net8.0" / "cortex.dll"
    if dll_path.exists():
        write_axi_file(
            pathlib.Path(directory_path) / "cortex.axi", 3, dll_path, "cortex_cs"
        )


def batch_transpile_cpp(directory_path, gcc_bin):
    import subprocess
    import pathlib

    gpp_bin = pathlib.Path(gcc_bin).parent / "g++.exe"
    build_dir = pathlib.Path(__file__).parent / "build_cpp"
    build_dir.mkdir(parents=True, exist_ok=True)
    py_files = list(pathlib.Path(directory_path).rglob("*.py"))
    o_files = []
    for py_file in py_files:
        if py_file.name == "__init__.py" or py_file.name == "cortex_bridge.py":
            continue
        try:
            with open(py_file, "r", encoding="utf-8") as f:
                source = f.read()
            tree = ast.parse(source)
            base_name = py_file.stem
            visitor = CppVisitor(base_name)
            visitor.visit(tree)
            cpp_file = build_dir / f"{base_name}.cpp"
            hpp_file = build_dir / f"{base_name}.hpp"
            with open(cpp_file, "w", encoding="utf-8") as f:
                f.write(visitor.generate_cpp())
            with open(hpp_file, "w", encoding="utf-8") as f:
                f.write(visitor.generate_hpp())
            subprocess.run(
                [str(gpp_bin), "-O2", "-std=c++20", "-c", cpp_file.name],
                cwd=str(build_dir),
                check=True,
            )
            o_files.append(cpp_file.with_suffix(".o").name)
        except Exception as e:
            pass
    ar_bin = pathlib.Path(gcc_bin).parent / "ar.exe"
    if o_files:
        subprocess.run(
            [str(ar_bin), "rcs", "libcortex_cpp.a"] + o_files,
            cwd=str(build_dir),
            check=True,
        )
        a_path = build_dir / "libcortex_cpp.a"
        if a_path.exists():
            write_axi_file(
                pathlib.Path(directory_path) / "cortex_cpp.axi", 2, a_path, "cortex_cpp"
            )


def batch_transpile(directory_path, gcc_bin, cjson_dir, curl_dir):
    import glob
    import shutil

    build_dir = pathlib.Path(__file__).parent / "build"
    build_dir.mkdir(parents=True, exist_ok=True)

    if (cjson_dir / "cJSON.h").exists():
        shutil.copy(cjson_dir / "cJSON.h", build_dir / "cJSON.h")
    if (cjson_dir / "cJSON.c").exists():
        shutil.copy(cjson_dir / "cJSON.c", build_dir / "cJSON.c")

    py_files = list(pathlib.Path(directory_path).rglob("*.py"))
    generated_headers = []

    for py_file in py_files:
        if py_file.name == "cortex_bridge.py":
            continue

        try:
            with open(py_file, "r", encoding="utf-8") as f:
                source = f.read()
            tree = ast.parse(source)

            filter_visitor = ASTFilterVisitor()
            filter_visitor.visit(tree)

            if filter_visitor.should_skip:
                print(
                    f"[SKIP] {py_file.name} - Contains dynamic imports or unhandled async constructs"
                )
                continue

            base_name = py_file.stem

            if base_name == "ollama_adapter":
                # Special AST interceptor for Phase 9
                output_c_file = build_dir / f"{base_name}.c"
                output_h_file = build_dir / f"{base_name}.h"

                h_code = """#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "cJSON.h"
#include <curl/curl.h>

typedef struct {
    int state;
    CURLM *multi_handle;
    CURL *easy_handle;
    char prompt_buffer[512];
    char response_buffer[16384];
    int response_len;
} ollama_fsm_ctx_t;

void ollama_fsm_step(ollama_fsm_ctx_t* ctx);
"""
                c_code = """/* Hybrid Compiler Auto-Generated C Code - Native LLM Bridge */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "arena.h"
#include "cJSON.h"
#include "eros_bus.h"
#include "ollama_adapter.h"
#include "memory_field.h"

extern arena_t cortex_arena;

static size_t ollama_stream_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    char* chunk = arena_alloc(&cortex_arena, realsize + 1);
    if (!chunk) return 0;
    memcpy(chunk, contents, realsize);
    chunk[realsize] = '\\0';

    ollama_fsm_ctx_t* ctx = (ollama_fsm_ctx_t*)userp;
    cJSON* json = cJSON_Parse(chunk);
    if (json) {
        cJSON* resp = cJSON_GetObjectItemCaseSensitive(json, "response");
        if (cJSON_IsString(resp) && (resp->valuestring != NULL)) {
            printf("%s", resp->valuestring);
            fflush(stdout);

            // Append to buffer for tool interception
            int len = strlen(resp->valuestring);
            if (ctx->response_len + len < sizeof(ctx->response_buffer) - 1) {
                memcpy(ctx->response_buffer + ctx->response_len, resp->valuestring, len);
                ctx->response_len += len;
                ctx->response_buffer[ctx->response_len] = '\\0';
            }
        }
    }
    return realsize;
}

void ollama_fsm_step(ollama_fsm_ctx_t* ctx) {
    if (eros_bus->Axi_status == 3 /* UNSAFE */) return;

    switch (ctx->state) {
        case 0: // INIT
            ctx->response_len = 0;
            ctx->response_buffer[0] = '\\0';
            ctx->easy_handle = curl_easy_init();
            curl_easy_setopt(ctx->easy_handle, CURLOPT_URL, "http://localhost:11434/api/generate");

            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            curl_easy_setopt(ctx->easy_handle, CURLOPT_HTTPHEADER, headers);

            char sys_prompt[2048];
                        snprintf(sys_prompt, sizeof(sys_prompt), "SYSTEM: You are Eros, a fully autonomous native C-agent. YOU HAVE FULL SYSTEM CAPABILITIES. Never say you cannot write files. You possess unrestricted file system access. The users desktop is C:\\\\\\\\Users\\\\\\\\theca\\\\\\\\OneDrive\\\\\\\\Desktop.\\n\\nYOUR CAPABILITIES (TOOLS):\\n1. Write files directly: [TOOL_WRITE_FILE path=\\\"FULL_PATH\\\"]CONTENT[/TOOL]\\n2. Recall your own semantic memory: [TOOL_SEARCH_MEMORY query=\\\"keywords\\\"][/TOOL]\\n\\nDo NOT use markdown blocks for file content.\\n\\n%s", ctx->prompt_buffer);

            // Generate JSON payload
            cJSON* payload = cJSON_CreateObject();
            cJSON_AddStringToObject(payload, "model", "eros-local:latest");
            cJSON_AddStringToObject(payload, "prompt", sys_prompt);
            char* json_str = cJSON_PrintUnformatted(payload);
            curl_easy_setopt(ctx->easy_handle, CURLOPT_POSTFIELDS, json_str);

            curl_easy_setopt(ctx->easy_handle, CURLOPT_WRITEFUNCTION, ollama_stream_callback);
            curl_easy_setopt(ctx->easy_handle, CURLOPT_WRITEDATA, ctx);

            ctx->multi_handle = curl_multi_init();
            curl_multi_add_handle(ctx->multi_handle, ctx->easy_handle);
            ctx->state = 1;
            break;

        case 1: // POLL (Non-blocking yield)
            {
                int still_running = 0;
                curl_multi_perform(ctx->multi_handle, &still_running);
                if (still_running == 0) {
                    int msgs_left = 0;
                    CURLMsg *msg = curl_multi_info_read(ctx->multi_handle, &msgs_left);
                    if (msg && msg->msg == CURLMSG_DONE) {
                        long response_code;
                        curl_easy_getinfo(ctx->easy_handle, CURLINFO_RESPONSE_CODE, &response_code);
                        if (msg->data.result != CURLE_OK) {
                            printf("\\n[OLLAMA ERROR] %s\\n", curl_easy_strerror(msg->data.result));
                        } else {
                            printf("\\n[OLLAMA STREAM DONE] HTTP %ld\\n", response_code);


                            // TOOL SEARCH MEMORY CHECK
                            char* mem_start = strstr(ctx->response_buffer, "[TOOL_SEARCH_MEMORY query=\\\"");
                            if (mem_start) {
                                mem_start += 27; // length of tag
                                char* query_end = strchr(mem_start, '\"');
                                if (query_end) {
                                    *query_end = '\\0';

                                    printf("\\n[CORTEX NATIVE TOOL] Searching memory for: %s\\n", mem_start);
                                    float q_vec[768] = {0}; // VECTOR_DIM
                                    if (embed_text_via_ollama(mem_start, q_vec)) {
                                        memory_record_t best;
                                        float sim = 0.0f;
                                        if (find_most_similar_memory(q_vec, &best, &sim)) {
                                            printf("[CORTEX NATIVE TOOL] Found Memory (sim=%.2f): %s\\n", sim, best.text);
                                            // Construct augmented prompt
                                            snprintf(ctx->prompt_buffer, sizeof(ctx->prompt_buffer), "%s\\n\\n[SYSTEM TOOL RESULT - MEMORY]: %s", ctx->prompt_buffer, best.text);

                                            // Clean up old handles
                                            curl_multi_remove_handle(ctx->multi_handle, ctx->easy_handle);
                                            curl_easy_cleanup(ctx->easy_handle);
                                            curl_multi_cleanup(ctx->multi_handle);

                                            // Trigger next LLM iteration with the new prompt
                                            ctx->state = 0;
                                            return;
                                        }
                                    }
                                }
                            }
\n                            // TOOL INTERCEPTOR CHECK
                                                        char* tool_start = strstr(ctx->response_buffer, "[TOOL_WRITE_FILE path=\\"");
                            if (tool_start) {
                                tool_start += 23; // length of tag
                                char* path_end = strchr(tool_start, '\"');
                                if (path_end) {
                                    *path_end = '\\0';
                                    char* content_start = strchr(path_end + 1, ']');
                                    if (content_start) {
                                        content_start += 1;
                                        char* tool_end = strstr(content_start, "[/TOOL]");
                                        if (tool_end) {
                                            *tool_end = '\\0';

                                            // NATIVE C FILE WRITE
                                            FILE* fp = fopen(tool_start, "w");
                                            if (fp) {
                                                fwrite(content_start, 1, strlen(content_start), fp);
                                                fclose(fp);
                                                printf("[CORTEX NATIVE TOOL] Successfully wrote file to: %s\\n", tool_start);
                                            } else {
                                                printf("[CORTEX NATIVE TOOL] Error writing file to: %s\\n", tool_start);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    curl_multi_remove_handle(ctx->multi_handle, ctx->easy_handle);
                    curl_easy_cleanup(ctx->easy_handle);
                    curl_multi_cleanup(ctx->multi_handle);
                    ctx->state = 2; // DONE
                }
            }
            break;

        case 2: // COMPLETED
            ctx->state = -1;
            break;
    }
}
"""
                with open(output_c_file, "w", encoding="utf-8") as f:
                    f.write(c_code)
                with open(output_h_file, "w", encoding="utf-8") as f:
                    f.write(h_code)

                generated_headers.append(f"{base_name}.h")
                print(f"[SUCCESS] Intercepted and lowered {py_file.name}")
                continue

            visitor = CortexVisitor(base_name)
            visitor.visit(tree)

            output_c_file = build_dir / f"{base_name}.c"
            output_h_file = build_dir / f"{base_name}.h"

            with open(output_c_file, "w", encoding="utf-8") as f:
                f.write(visitor.generate_c())
            with open(output_h_file, "w", encoding="utf-8") as f:
                f.write(visitor.generate_h())

            generated_headers.append(f"{base_name}.h")
            print(f"[SUCCESS] Transpiled {py_file.name}")
        except (NotImplementedError, AssertionError, SyntaxError) as e:
            print(f"[SKIP] {py_file.name} - Unhandled AST Node: {e}")
        except Exception as e:
            print(f"[SKIP] {py_file.name} - Error: {e}")

    master_h = build_dir / "cortex.h"
    with open(master_h, "w", encoding="utf-8") as f:
        f.write("#pragma once\n")
        f.write("#include <stdint.h>\n")
        f.write("#include <stdbool.h>\n")
        f.write('#include "cJSON.h"\n')
        for h in generated_headers:
            f.write(f'#include "{h}"\n')

    print(f"Generated master header cortex.h with {len(generated_headers)} modules.")

    print("Compiling transpiled C sources...")
    o_files = []

    curl_inc = f"-I{curl_dir / 'include'}"
    curl_lib = f"-L{curl_dir / 'lib'}"

    for c_file in build_dir.glob("*.c"):
        try:
            subprocess.run(
                [
                    str(gcc_bin),
                    "-DCURL_STATICLIB",
                    "-O2",
                    "-std=c11",
                    "-I.",
                    curl_inc,
                    "-I..\\..\\..\\eros_flat_core",
                    "-c",
                    c_file.name,
                ],
                cwd=str(build_dir),
                check=True,
                shell=True,
            )
            o_files.append(c_file.with_suffix(".o").name)
        except subprocess.CalledProcessError as e:
            print(f"Compilation failed for {c_file.name}: {e}")

    if not o_files:
        print("No object files to archive.")
        return

    ar_bin = gcc_bin.parent / "ar.exe"
    print("Archiving to libcortex.a...")
    try:
        subprocess.run(
            [str(ar_bin), "rcs", "libcortex.a"] + o_files,
            cwd=str(build_dir),
            check=True,
            shell=True,
        )
        print("Successfully created libcortex.a")
    except subprocess.CalledProcessError as e:
        print(f"Archiving failed: {e}")


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("--batch", type=str, help="Directory to batch transpile")
    parser.add_argument(
        "--target",
        type=str,
        choices=["c", "csharp", "cpp"],
        default="c",
        help="Target language",
    )
    parser.add_argument(
        "input_file", type=str, nargs="?", help="Single file to transpile"
    )
    args = parser.parse_args()

    if args.target == "csharp":
        dotnet_exe = ensure_dotnet()
        if args.batch:
            batch_transpile_csharp(args.batch, dotnet_exe)
    elif args.target == "cpp":
        gcc_bin = ensure_compiler()
        if args.batch:
            batch_transpile_cpp(args.batch, gcc_bin)
    else:
        gcc_bin = ensure_compiler()
        cjson_dir = ensure_cjson()
        curl_dir = ensure_libcurl()
        if args.batch:
            batch_transpile(args.batch, gcc_bin, cjson_dir, curl_dir)

