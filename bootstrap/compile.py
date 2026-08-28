import sys
import os
from typing import List

# Add current dir to path to import local modules
sys.path.insert(0, os.path.dirname(__file__))

from lexer import AxiLexer, SyntaxKind
from c_emitter import StructDef, StructField, FnDef, EnumDef, EnumVariant, emit_c_abi
from py_emitter import emit_py_bindings


def parse_schema(tokens):
    structs = []
    functions = []
    enums = []
    pos = 0
    while pos < len(tokens):
        # ... function parsing
        is_query = False
        start_pos = pos

        while start_pos < len(tokens) and tokens[start_pos].kind in (
            SyntaxKind.Whitespace,
            SyntaxKind.Indent,
            SyntaxKind.Dedent,
            SyntaxKind.Comment,
        ):
            start_pos += 1

        if start_pos < len(tokens) and tokens[start_pos].kind == SyntaxKind.At:
            if start_pos + 1 < len(tokens) and tokens[start_pos + 1].text == "query":
                is_query = True
                start_pos += 2
                while start_pos < len(tokens) and tokens[start_pos].kind in (
                    SyntaxKind.Whitespace,
                    SyntaxKind.Indent,
                    SyntaxKind.Dedent,
                    SyntaxKind.Comment,
                ):
                    start_pos += 1

        # Match 'pub fn ident'
        if (
            start_pos + 2 < len(tokens)
            and tokens[start_pos].kind == SyntaxKind.KwPub
            and tokens[start_pos + 1].kind == SyntaxKind.KwFn
        ):
            fn_name = tokens[start_pos + 2].text
            pos = start_pos + 3
            args = []

            while pos < len(tokens) and tokens[pos].kind != SyntaxKind.LParen:
                pos += 1
            pos += 1  # skip '('

            while pos < len(tokens) and tokens[pos].kind != SyntaxKind.RParent:
                if tokens[pos].kind == SyntaxKind.Ident:
                    arg_name = tokens[pos].text
                    pos += 1
                    if tokens[pos].kind == SyntaxKind.Colon:
                        pos += 1
                        type_name = ""
                        bracket_depth = 0
                        while pos < len(tokens):
                            if tokens[pos].kind in (
                                SyntaxKind.LBracket,
                                SyntaxKind.LBrace,
                            ):
                                bracket_depth += 1
                            elif tokens[pos].kind in (
                                SyntaxKind.RBracket,
                                SyntaxKind.RBrace,
                            ):
                                bracket_depth -= 1
                            if bracket_depth <= 0 and tokens[pos].kind in (
                                SyntaxKind.Comma,
                                SyntaxKind.RParent,
                            ):
                                break
                            type_name += tokens[pos].text
                            pos += 1
                        args.append(StructField(arg_name, type_name.strip()))
                else:
                    pos += 1

            ret_type = "void"
            while pos < len(tokens) and tokens[pos].kind != SyntaxKind.LBrace:
                if tokens[pos].kind == SyntaxKind.Arrow:
                    pos += 1
                    ret_type = ""
                    while pos < len(tokens) and tokens[pos].kind != SyntaxKind.LBrace:
                        ret_type += tokens[pos].text
                        pos += 1
                    ret_type = ret_type.strip()
                    break
                pos += 1

            functions.append(FnDef(fn_name, is_query, args, ret_type))

            brace_depth = 1
            pos += 1
            while pos < len(tokens) and brace_depth > 0:
                if tokens[pos].kind == SyntaxKind.LBrace:
                    brace_depth += 1
                elif tokens[pos].kind == SyntaxKind.RBrace:
                    brace_depth -= 1
                pos += 1
            continue

        # Look for 'pub enum Ident'
        if (
            tokens[pos].kind == SyntaxKind.KwPub
            and pos + 2 < len(tokens)
            and tokens[pos + 1].kind == SyntaxKind.KwEnum
        ):
            enum_name = tokens[pos + 2].text
            pos += 3

            base_type = "int"
            if tokens[pos].kind == SyntaxKind.Colon:
                pos += 1
                base_type = ""
                while pos < len(tokens) and tokens[pos].kind != SyntaxKind.LBrace:
                    base_type += tokens[pos].text
                    pos += 1
                base_type = base_type.strip()

            variants = []
            while pos < len(tokens) and tokens[pos].kind != SyntaxKind.LBrace:
                pos += 1
            pos += 1  # skip '{'

            while pos < len(tokens) and tokens[pos].kind != SyntaxKind.RBrace:
                if tokens[pos].kind == SyntaxKind.Ident:
                    v_name = tokens[pos].text
                    pos += 1
                    v_value = None
                    v_fields = []
                    is_struct = False

                    if tokens[pos].kind == SyntaxKind.Eq:
                        pos += 1
                        v_value = ""
                        while pos < len(tokens) and tokens[pos].kind not in (
                            SyntaxKind.Comma,
                            SyntaxKind.Indent,
                            SyntaxKind.Dedent,
                            SyntaxKind.RBrace,
                            SyntaxKind.Ident,
                        ):
                            v_value += tokens[pos].text
                            pos += 1
                        v_value = v_value.strip()
                    elif tokens[pos].kind == SyntaxKind.LParen:
                        is_struct = False
                        pos += 1
                        while (
                            pos < len(tokens) and tokens[pos].kind != SyntaxKind.RParent
                        ):
                            type_name = ""
                            bracket_depth = 0
                            while pos < len(tokens):
                                if tokens[pos].kind in (
                                    SyntaxKind.LBracket,
                                    SyntaxKind.LBrace,
                                    SyntaxKind.LParen,
                                ):
                                    bracket_depth += 1
                                elif tokens[pos].kind in (
                                    SyntaxKind.RBracket,
                                    SyntaxKind.RBrace,
                                    SyntaxKind.RParent,
                                ):
                                    bracket_depth -= 1
                                if bracket_depth <= 0 and tokens[pos].kind in (
                                    SyntaxKind.Comma,
                                    SyntaxKind.RParent,
                                ):
                                    break
                                type_name += tokens[pos].text
                                pos += 1
                            if type_name.strip():
                                v_fields.append(
                                    StructField(f"_{len(v_fields)}", type_name.strip())
                                )
                            if tokens[pos].kind == SyntaxKind.Comma:
                                pos += 1
                        pos += 1  # skip ')'
                    elif tokens[pos].kind == SyntaxKind.LBrace:
                        is_struct = True
                        pos += 1
                        while (
                            pos < len(tokens) and tokens[pos].kind != SyntaxKind.RBrace
                        ):
                            if tokens[pos].kind == SyntaxKind.Ident:
                                f_name = tokens[pos].text
                                pos += 1
                                if tokens[pos].kind == SyntaxKind.Colon:
                                    pos += 1
                                    type_name = ""
                                    bracket_depth = 0
                                    while pos < len(tokens):
                                        if tokens[pos].kind in (
                                            SyntaxKind.LBracket,
                                            SyntaxKind.LBrace,
                                            SyntaxKind.LParen,
                                        ):
                                            bracket_depth += 1
                                        elif tokens[pos].kind in (
                                            SyntaxKind.RBracket,
                                            SyntaxKind.RBrace,
                                            SyntaxKind.RParent,
                                        ):
                                            bracket_depth -= 1
                                        if bracket_depth <= 0 and tokens[pos].kind in (
                                            SyntaxKind.Comma,
                                            SyntaxKind.RBrace,
                                        ):
                                            break
                                        type_name += tokens[pos].text
                                        pos += 1
                                    v_fields.append(
                                        StructField(f_name, type_name.strip())
                                    )
                            if tokens[pos].kind == SyntaxKind.Comma:
                                pos += 1
                            elif (
                                tokens[pos].kind != SyntaxKind.RBrace
                                and tokens[pos].kind != SyntaxKind.Ident
                            ):
                                pos += 1
                        pos += 1  # skip '}'

                    variants.append(EnumVariant(v_name, v_value, v_fields, is_struct))

                    if pos < len(tokens) and tokens[pos].kind == SyntaxKind.Comma:
                        pos += 1
                else:
                    pos += 1

            enums.append(EnumDef(enum_name, base_type, variants))
            pos += 1
            continue

        # Look for 'pub struct Ident {'
        if (
            tokens[pos].kind == SyntaxKind.KwPub
            and pos + 2 < len(tokens)
            and tokens[pos + 1].kind == SyntaxKind.KwStruct
        ):
            struct_name = tokens[pos + 2].text
            fields = []
            pos += 3

            while pos < len(tokens) and tokens[pos].kind != SyntaxKind.LBrace:
                pos += 1
            pos += 1  # skip '{'

            while pos < len(tokens) and tokens[pos].kind != SyntaxKind.RBrace:
                if tokens[pos].kind == SyntaxKind.KwPub:
                    pos += 1
                    field_name = tokens[pos].text
                    pos += 1  # skip ident

                    if tokens[pos].kind == SyntaxKind.Colon:
                        pos += 1

                        type_name = ""
                        bracket_depth = 0
                        while pos < len(tokens):
                            if tokens[pos].kind in (
                                SyntaxKind.LBracket,
                                SyntaxKind.LBrace,
                            ):
                                bracket_depth += 1
                            elif tokens[pos].kind in (
                                SyntaxKind.RBracket,
                                SyntaxKind.RBrace,
                            ):
                                bracket_depth -= 1
                            if bracket_depth <= 0 and tokens[pos].kind in (
                                SyntaxKind.Comma,
                                SyntaxKind.Indent,
                                SyntaxKind.Dedent,
                                SyntaxKind.KwPub,
                                SyntaxKind.RBrace,
                            ):
                                if tokens[pos].kind == SyntaxKind.RBrace:
                                    bracket_depth += 1
                                    break
                                break
                            type_name += tokens[pos].text
                            pos += 1

                        fields.append(StructField(field_name, type_name.strip()))
                pos += 1

            structs.append(StructDef(struct_name, fields))
        else:
            pos += 1

    return structs, functions, enums


def compile_file(input_file: str, output_file: str):
    print(f"Reading {input_file}...")
    with open(input_file, "r") as f:
        source = f.read()

    print("Lexing tokens...")
    lexer = AxiLexer(source)
    tokens = lexer.lex()

    print("Parsing schema blocks...")
    structs, functions, enums = parse_schema(tokens)
    print(
        f"Found {len(structs)} structs, {len(functions)} functions, {len(enums)} enums."
    )

    print("Emitting C ABI Headers...")
    c_header = emit_c_abi(structs, functions, enums)

    print("Emitting Python Bindings...")
    py_bindings = emit_py_bindings(structs, functions, enums)

    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    with open(output_file, "w") as f:
        f.write(c_header)

    py_out = output_file.replace(".h", ".py")
    with open(py_out, "w") as f:
        f.write(py_bindings)

    print(f"\n[SUCCESS] Wrote C ABI definition to {output_file}")
    print(f"[SUCCESS] Wrote Python Bindings to {py_out}")
    print("-" * 80)
    print("... Headers generated successfully ...")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python compile.py <input.axi> <output.h>")
        sys.exit(1)
    compile_file(sys.argv[1], sys.argv[2])

