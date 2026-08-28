import sys
from typing import List
from c_emitter import StructDef, StructField, FnDef, EnumDef, EnumVariant


def map_type_py(axi_type: str) -> str:
    mapping = {
        "u8": "int",
        "u16": "int",
        "u32": "int",
        "u64": "int",
        "i32": "int",
        "f32": "float",
        "f64": "float",
        "bool": "bool",
        "str": "str",
        "any": "Any",
        "void": "None",
    }

    if axi_type.startswith("&[") and axi_type.endswith("]"):
        inner = axi_type[2:-1]
        if inner == "u8":
            return "bytes"
        return f"List[{map_type_py(inner)}]"
    if axi_type.startswith("&"):
        inner = axi_type[1:]
        return map_type_py(inner)

    if axi_type in mapping:
        return mapping[axi_type]

    if axi_type.startswith("map["):
        inner = axi_type[4:-1]
        parts = [p.strip() for p in inner.split(",")]
        if len(parts) == 2:
            return f"Dict[{map_type_py(parts[0])}, {map_type_py(parts[1])}]"

    if axi_type.startswith("[") and axi_type.endswith("]"):
        inner = axi_type[1:-1]
        if ";" in inner:
            parts = inner.split(";")
            return f"List[{map_type_py(parts[0].strip())}]"
        return f"List[{map_type_py(inner)}]"

    return axi_type


def emit_py_bindings(
    structs: List[StructDef], functions: List[FnDef] = None, enums: List[EnumDef] = None
) -> str:
    if functions is None:
        functions = []
    if enums is None:
        enums = []

    out = []
    out.append("# ruff: noqa: F821")
    out.append('"""')
    out.append("Axi GENERATED PYTHON BINDINGS")
    out.append("STRICT COMPLIANCE: IMMUTABLE FROZEN DATACLASSES")
    out.append('"""\n')
    out.append("from dataclasses import dataclass")
    out.append("from typing import List, Dict, Any, Union\n")
    out.append("from enum import IntEnum\n")

    for e in enums:
        is_adt = any(v.fields for v in e.variants)
        if is_adt:
            out.append(f"class {e.name}:")
            out.append("    pass\n")
            for v in e.variants:
                out.append("@dataclass(frozen=True)")
                out.append(f"class {e.name}_{v.name}({e.name}):")
                if not v.fields:
                    out.append("    pass")
                for f in v.fields:
                    py_type = map_type_py(f.type_name)
                    out.append(f"    {f.name}: {py_type}")
                out.append("")
        else:
            out.append(f"class {e.name}(IntEnum):")
            for i, v in enumerate(e.variants):
                val = v.value if v.value else str(i)
                out.append(f"    {v.name} = {val}")
            out.append("")

    for s in structs:
        out.append("@dataclass(frozen=True)")
        out.append(f"class {s.name}:")
        if not s.fields:
            out.append("    pass\n")
            continue

        for f in s.fields:
            py_type = map_type_py(f.type_name)
            out.append(f"    {f.name}: {py_type}")
        out.append("")

    if functions:
        out.append(
            "# ===================================================================="
        )
        out.append("# QUERY & COMPUTE KERNEL IPC STUBS")
        out.append(
            "# ====================================================================\n"
        )

        for fn in functions:
            if fn.is_query:
                out.append(f"# @query (Memoized incrementally)")

            py_args = []
            for a in fn.args:
                py_args.append(f"{a.name}: {map_type_py(a.type_name)}")

            py_ret = map_type_py(fn.ret_type)
            args_str = ", ".join(py_args)

            out.append(f"def {fn.name}({args_str}) -> {py_ret}:")
            out.append(
                f'    raise NotImplementedError("Kernel IPC Boundary - call into libAxi.so")\n'
            )

    return "\n".join(out)


if __name__ == "__main__":
    pass

