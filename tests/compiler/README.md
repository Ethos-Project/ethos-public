# Base Axiom Compiler Route Contract

`compiler_routes.axi` is the canonical native conformance driver for the first
truthful base-distribution routing slice. It accepts the compiler executable and
Axiom workspace root as arguments, builds all five fixtures, executes the
outputs, and requires these markers:

- `.axi`: `AXI_OK`
- C: `C_OK`
- C++: `CPP_OK`
- C#: `CS_OK`
- Python: `PY_OK`

Build the driver through the live compiler:

```text
lang\axiom_compiler.exe lang\tests\compiler\compiler_routes.axi network\tmp\axiom-wrap-sdd\compiler-routes\compiler-routes.exe
```

Run it with absolute paths when the native host changes the child working
directory. The C++ fixture must run in a normal native process environment;
Codex's managed filesystem sandbox blocks C++ standard-library initialization.

`invalid.c` and `unsupported.py` are negative fixtures. Each must return
nonzero and leave no requested executable. The Python route intentionally
supports only a small, generic module-level `print(constant)` subset today and
does not import internal proprietary logic.
