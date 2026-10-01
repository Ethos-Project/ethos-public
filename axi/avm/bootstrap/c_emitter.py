import sys
from typing import List, Dict
from dataclasses import dataclass


@dataclass
class StructField:
    name: str
    type_name: str


@dataclass
class StructDef:
    name: str
    fields: List[StructField]


@dataclass
class FnDef:
    name: str
    is_query: bool
    args: List[StructField]
    ret_type: str


@dataclass
class EnumVariant:
    name: str
    value: str
    fields: List[StructField]
    is_struct: bool


@dataclass
class EnumDef:
    name: str
    base_type: str
    variants: List[EnumVariant]


def map_type(axi_type: str) -> str:
    # Map Axi types to C types
    mapping = {
        "u8": "uint8_t",
        "u16": "uint16_t",
        "u32": "uint32_t",
        "u64": "uint64_t",
        "i32": "int32_t",
        "f32": "float",
        "f64": "double",
        "bool": "bool",
        "str": "char",  # special handling
        "any": "uint64_t",  # opaque handle
        "void": "void",
    }

    if axi_type.startswith("&[") and axi_type.endswith("]"):
        inner = axi_type[2:-1]
        return f"const {map_type(inner)}*"
    if axi_type.startswith("&"):
        inner = axi_type[1:]
        return f"const {map_type(inner)}*"

    if axi_type.startswith("[") and axi_type.endswith("]"):
        inner = axi_type[1:-1]
        if ";" in inner:
            parts = inner.split(";")
            return f"{map_type(parts[0].strip())}*"

    return mapping.get(axi_type, axi_type)


def emit_c_abi(
    structs: List[StructDef], functions: List[FnDef] = None, enums: List[EnumDef] = None
) -> str:
    if functions is None:
        functions = []
    if enums is None:
        enums = []

    out = []
    out.append(
        "/* ===================================================================="
    )
    out.append(" * Axi GENERATED ABI BOUNDARY")
    out.append(" * STRICT COMPLIANCE: EULER_POOL STATIC ALLOCATION & UINT64 WRAPPING")
    out.append(" * MEMORY FLATTENING: Arrays and Maps are flattened into static limits")
    out.append(
        " * ==================================================================== */\n"
    )
    out.append("#ifndef Axi_GENERATED_ABI_H")
    out.append("#define Axi_GENERATED_ABI_H\n")
    out.append("#include <stdint.h>")
    out.append("#include <stdbool.h>\n")

    # 1. Emit Enum Definitions
    for e in enums:
        is_adt = any(v.fields for v in e.variants)
        if is_adt:
            out.append(f"typedef enum {{")
            for v in e.variants:
                out.append(f"    {e.name}Tag_{v.name},")
            out.append(f"}} {e.name}Tag;")
            out.append(f"")
            out.append(f"typedef struct {{")
            out.append(f"    {e.name}Tag tag;")
            out.append(f"    union {{")
            for v in e.variants:
                if v.fields:
                    out.append(f"        struct {{")
                    for f in v.fields:
                        if f.type_name.startswith("map["):
                            inner = f.type_name[4:-1]
                            parts = [p.strip() for p in inner.split(",")]
                            k_type = map_type(parts[0])
                            v_type = map_type(parts[1])
                            k_decl = (
                                f"char key[128];"
                                if k_type == "char"
                                else f"{k_type} key;"
                            )
                            v_decl = (
                                f"char value[128];"
                                if v_type == "char"
                                else f"{v_type} value;"
                            )
                            out.append(
                                f"            struct {{ {k_decl} {v_decl} }} {f.name}[16];"
                            )
                            out.append(f"            uint32_t {f.name}_count;")
                        elif f.type_name.startswith("[") and f.type_name.endswith("]"):
                            inner = f.type_name[1:-1]
                            elem_type = map_type(inner)
                            if elem_type == "char":
                                out.append(f"            char {f.name}[32][256];")
                            else:
                                out.append(f"            {elem_type} {f.name}[32];")
                            out.append(f"            uint32_t {f.name}_count;")
                        elif f.type_name == "str":
                            out.append(f"            char {f.name}[256];")
                        else:
                            c_type = map_type(f.type_name)
                            out.append(f"            {c_type} {f.name};")
                    out.append(f"        }} {v.name};")
            out.append(f"    }} data;")
            out.append(f"}} {e.name};\n")
        else:
            c_base = map_type(e.base_type)
            if not c_base.strip():
                c_base = "int"
            out.append(f"typedef {c_base} {e.name};")
            out.append(f"enum {e.name}_Variants {{")
            for v in e.variants:
                if v.value:
                    out.append(f"    {e.name}_{v.name} = {v.value},")
                else:
                    out.append(f"    {e.name}_{v.name},")
            out.append(f"}};\n")

    # 2. Emit Struct Definitions
    for s in structs:
        out.append(f"typedef struct {{")
        for f in s.fields:
            if f.type_name.startswith("map["):
                inner = f.type_name[4:-1]
                parts = [p.strip() for p in inner.split(",")]
                k_type = map_type(parts[0])
                v_type = map_type(parts[1])

                k_decl = f"char key[128];" if k_type == "char" else f"{k_type} key;"
                v_decl = f"char value[128];" if v_type == "char" else f"{v_type} value;"

                out.append(f"    struct {{ {k_decl} {v_decl} }} {f.name}[16];")
                out.append(f"    uint32_t {f.name}_count;")
            elif f.type_name.startswith("[") and f.type_name.endswith("]"):
                inner = f.type_name[1:-1]
                elem_type = map_type(inner)
                if elem_type == "char":
                    out.append(f"    char {f.name}[32][256];")
                else:
                    out.append(f"    {elem_type} {f.name}[32];")
                out.append(f"    uint32_t {f.name}_count;")
            elif f.type_name == "str":
                out.append(f"    char {f.name}[256];")
            else:
                c_type = map_type(f.type_name)
                out.append(f"    {c_type} {f.name};")
        out.append(f"}} {s.name};\n")

    # 3. Emit IPC Queue Mechanism for each struct
    for s in structs:
        out.append(f"/* Lock-Free IPC Ring Buffer for {s.name} */")
        out.append(f"typedef struct {{")
        out.append(f"    uint64_t producer_sequence;")
        out.append(f"    uint64_t consumer_sequence;")
        out.append(f"    uint32_t capacity;")
        out.append(f"    {s.name}* euler_pool_buffer; // Must map to static memory")
        out.append(f"}} {s.name}Queue;\n")

        out.append(
            f"static inline bool {s.name}Queue_push({s.name}Queue* q, {s.name} item) {{"
        )
        out.append(
            f"    uint64_t distance = (uint64_t)(q->producer_sequence - q->consumer_sequence);"
        )
        out.append(f"    if (distance >= q->capacity) return false; // Queue Full")
        out.append(
            f"    q->euler_pool_buffer[q->producer_sequence % q->capacity] = item;"
        )
        out.append(
            f"    q->producer_sequence++; // Unsigned wrapping prevents overflow"
        )
        out.append(f"    return true;")
        out.append(f"}}\n")

    # 4. Emit Function Stubs
    if functions:
        out.append(
            "/* ===================================================================="
        )
        out.append(" * QUERY & COMPUTE KERNEL EXPORTS")
        out.append(
            " * ==================================================================== */\n"
        )
        out.append('#ifdef __cplusplus\nextern "C" {\n#endif\n')
        for fn in functions:
            if fn.is_query:
                out.append(f"// @query (Memoized incrementally)")

            c_args = []
            for a in fn.args:
                c_args.append(f"{map_type(a.type_name)} {a.name}")

            c_ret = map_type(fn.ret_type)
            args_str = ", ".join(c_args) if c_args else "void"

            out.append(f"extern {c_ret} {fn.name}({args_str});\n")

        out.append("#ifdef __cplusplus\n}\n#endif\n")

    out.append("#endif // Axi_GENERATED_ABI_H")
    return "\n".join(out)


if __name__ == "__main__":
    pass

