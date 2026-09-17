# Community AVM Pipeline Inventory

Date: 2026-08-27  
Status: Verified base-routing slice plus discovery evidence  
Authority: Not a language or IR specification

## Purpose

This inventory records the compiler and transpiler material currently present
under `C:\Ethos\ethos-products\Axi`. It does not declare any incomplete
implementation canonical. The maintainer-confirmed target architecture is:

- the base Axi distribution coordinates dedicated native pipelines for
  `.axi`, C, C++, C#, and Python;
- Psyche adds shared Axi IR/DAG normalization for every supported language
  before internal lowering.

The existing trees predate that realignment. Reusable components remain
evidence until conformance tests establish their ownership. The focused base
coordinator slice described below is now verified and promoted.

## Verified base coordinator status

On 2026-08-27, `lang\Axi_compiler.c` and `Axi_compiler.exe` were rebuilt as
the live, fail-closed coordinator. The native `.axi` contract builds and runs
five marker programs through dedicated routes:

| Input | Dedicated route | Verified marker |
| --- | --- | --- |
| `.axi` | graph/node bootstrap to inspectable C, then bundled GCC | `AXI_OK` |
| C | bundled GCC 16.2 | `C_OK` |
| C++ | bundled G++ 16.2 with self-contained MinGW runtimes | `CPP_OK` |
| C# | isolated bundled .NET SDK 8 project | `CS_OK` |
| Python | fail-closed generic AST subset to inspectable C, then GCC | `PY_OK` |

Invalid C and unsupported Python both return nonzero and leave no requested
executable. This is an initial truthful routing slice, not a claim that the
complete Axi grammar, Python language, or Psyche shared IR/DAG is finished.
The C++ executable requires execution outside Codex's managed filesystem
sandbox because that sandbox blocks C++ runtime initialization with
`STATUS_ACCESS_DENIED`; the same self-contained artifact executes normally in
the native host environment.

During the first isolated C# probe, the bundled .NET SDK performed its first-run
behavior and reported installing an ASP.NET Core HTTPS development certificate.
The coordinator now suppresses first-run certificate generation, telemetry,
banner output, and global-tool PATH mutation for future isolated builds. The
certificate already created by that SDK probe was not removed automatically.

## Historical executable baseline (superseded 2026-08-27)

The previous `lang\Axi_compiler.exe` was invoked in an isolated directory
against existing `.axi`, C, C++, C#, and Python files. Every invocation exited
with code `0`. No invocation produced its requested executable.

| Input | Current message | Durable output |
| --- | --- | --- |
| `.axi` | Reports parsed, built, and successful | `output.c` only |
| C | Reports native pass-through | None |
| C++ | Reports native pass-through | None |
| C# | Reports pending implementation | None |
| Python | Reports pending implementation | None |

The `.axi`, C, and C++ routes attempt a developer-machine absolute compiler
path under `C:\Antigravity`. The child-process failure is ignored. This is the
same false-success class already identified in the DVCS CLI.

Probe artifacts are isolated under
`network\tmp\Axi-wrap-sdd\compiler-probe`; only generated `output.c` exists.

## Candidate component map

### Coordination shell

| Path | Evidence | Current limitation | Target classification |
| --- | --- | --- | --- |
| `lang\Axi_compiler.c` | Live relocatable fail-closed routing by `.axi`, `.c`, `.cpp`, `.cs`, and `.py` | Initial language subsets; broader conformance remains | Verified base Axi coordinator slice |
| `lang\Axi_compiler.exe` | Native executable rebuilt from the live coordinator | Five-route native contract passes | Live base coordinator binary |
| `lang\compiler.axi` | Axi-authored compiler/bootstrap experiment | Uses the earlier node/wire and raw-C form; contains stale tool paths | Reusable source evidence; not canonized by this inventory |

### `.axi` pipeline material

| Path | Evidence | Current limitation | Target classification |
| --- | --- | --- | --- |
| `lang\Axi_syntax_specification.md` | Describes high-to-low systems syntax and C11/C23 mapping | Not fully matched by a verified compiler | Language contract candidate, subject to conformance reconciliation |
| `lang\src\syntax.axi` | Syntax kinds and green/red node structures | Partial vocabulary | Base Axi parser candidate |
| `lang\src\lexer.axi` | Native indentation tracking | Depends on surrounding types and pipeline | Base Axi parser candidate |
| `lang\src\pratt.axi` | Native expression binding and parsing | Depends on undefined surrounding parser contracts | Base Axi parser candidate |
| `lang\bootstrap\lexer.py` | Broader bootstrap lexer | Python-hosted bootstrap implementation | Bootstrap evidence |
| `lang\bootstrap\compile.py` | Parses exported schemas and emits C ABI/Python bindings | Schema-focused rather than a complete language compiler | Base interop-generator candidate |
| `lang\bootstrap\c_emitter.py` | C ABI emitter | Does not establish complete `.axi` lowering | Base emitter component candidate |
| `lang\bootstrap\py_emitter.py` | Python binding emitter | Binding generation is not the dedicated Python input pipeline | Base interop component candidate |

### C and C++ routes

The bundled native toolchain is present and executable:

- MinGW GCC `16.2.0` at
  `lang\bootstrap\python_to_c_compiler\bin\mingw64\bin\gcc.exe`;
- MinGW G++ `16.2.0` beside it.

The coordinator does not discover these paths relative to its installation.
It also routes both C and C++ through `gcc.exe`, so the checked-in router does
not establish a truthful dedicated C++ pipeline.

### C# route

The bundled .NET SDK is present and executable:

- .NET SDK `8.0.204`;
- .NET runtimes `8.0.4`;
- Windows Desktop runtime `8.0.4`.

`lang\bootstrap\python_to_c_compiler\cortex_transpiler.py` contains a
Python-to-C# visitor and a batch `.NET` build route. It does not implement the
coordinator's direct `.cs` input route, does not check the `.NET` subprocess
result, and can silently skip source exceptions.

### Python route

`lang\bootstrap\python_to_c_compiler\cortex_transpiler.py` uses Python's AST
to generate C, C++, or C#. This is substantial implementation evidence, but it
is not yet the canonical base Python pipeline:

- its command-line `input_file` argument is parsed but unused;
- its implemented routes operate on batch directories;
- several translation exceptions are silently ignored;
- its C build path imports `eros_flat_core`, coupling it to the internal
  cognition-enhanced tree;
- generated build directories contain stale `C:\Antigravity` provenance.

The generic Python frontend and the Psyche-enhanced Python lowering must be
separated without discarding reusable visitors.

### Psyche shared IR/DAG material

`lang\src\compiler\AST.h` defines an Axi DAG and directly includes
`eros_flat_core` bus and Euler-pool headers. `c_api.cpp` owns a process-global
DAG, while `Axi_ParseSource` is currently a stub returning success without
parsing. Other files in the same directory use incompatible parser/AST names.

This directory is evidence for the intended Psyche shared-IR/DAG overlay, not a
completed or canonical implementation. Its protected Eros dependencies must
not be pulled into the base Axi distribution.

### LLVM/Clang-MinGW route

No `clang.exe`, `clang-cl.exe`, `llc.exe`, or `ld.lld.exe` was found under the
inspected `lang` toolchain. LLVM-related headers and notices do not establish an
installed executable route. The intended route remains unresolved until its
tool location or recoverable source package is identified and verified.

## Ownership statuses

| Status | Meaning |
| --- | --- |
| Base Axi target | Intended for the dedicated public-language pipeline after conformance |
| Psyche target | Intended for shared-IR/DAG and cognition-enhanced internal integration |
| Bootstrap evidence | May help rebuild a target but does not define it |
| Generated artifact | Output or cache; never source authority |
| Unresolved | Requires source-backed comparison before ownership or movement |

No current candidate is promoted merely by its directory name. No source is
moved, deleted, published, or incorporated into protected cognition by this
inventory.

## Realignment tasks

1. Define one fail-closed coordinator result contract shared by every dedicated
   base pipeline: input identity, selected pipeline, selected backend, generated
   artifacts, diagnostics, subprocess exit, and verified final output.
2. Create executable conformance fixtures for `.axi`, C, C++, C#, and Python.
3. Reconcile the `.axi` syntax, lexer, parser, semantic, and emitter candidates
   against those fixtures.
4. Extract generic Python visitors from cognition-specific build assumptions.
5. Wire C to GCC, C++ to G++, and C# to the bundled .NET SDK using paths
   relative to the Axi installation.
6. Implement the base coordinator without mandatory shared-IR normalization.
7. Specify and test Psyche's additional shared-IR/DAG ingress for all five
   languages while preserving Eros authority boundaries.
8. Add LLVM/Clang-MinGW only after discovery identifies a real executable route.
9. Replace temporary PowerShell orchestration with `.axi` build and test
   programs once the first corrected compiler slice can compile them.


