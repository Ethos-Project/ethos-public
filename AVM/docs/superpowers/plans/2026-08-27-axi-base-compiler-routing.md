# Axi Base Compiler Routing Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace the false-success compiler shell with a relocatable base-Axi coordinator that truthfully builds and verifies `.axi`, C, C++, C#, and Python fixtures.

**Architecture:** Keep the existing native coordinator as the bootstrap boundary, but make every input extension select an explicit dedicated pipeline. Author conformance fixtures and the canonical route runner in `.axi`; use generated C as inspectable output. Reuse bundled GCC, G++, and .NET, and isolate the generic Python frontend from cognition-specific Cortex lowering.

**Tech Stack:** Axi `.axi`, Windows C11, MinGW GCC/G++ 16.2, .NET SDK 8.0.204, Python 3.11 AST bootstrap, Windows process/filesystem APIs.

**Spec:** `docs/superpowers/specs/2026-08-27-Axi-psyche-two-distribution-design.md`

**Execution status (2026-08-27):** Tasks 1-6 are implemented and verified. Task
7's canonical `.axi` strict build plus durable round-trip driver is implemented,
the live focused DVCS v1 is promoted, and the supplemental behavior suite passes
11/11. The full port of all behavioral cases into `.axi`, failure injection,
concurrency, and the expanded corrupt-object matrix remain open and are recorded
in `network/tmp/Axi-wrap-sdd/task-2-report.md`. The checkboxes below preserve
the original execution plan rather than overstating those extended requirements.

## Global Constraints

- Base Axi coordinates dedicated `.axi`, C, C++, C#, and Python pipelines.
- Psyche shared-IR/DAG normalization is additional and must not be simulated by a metadata-only sidecar.
- No developer-machine absolute paths may remain in active routing code.
- A route succeeds only when its child process exits successfully and its requested artifact exists.
- Build and test programs are authored in `.axi`; generated C is inspectable build output.
- No protected Eros cognition, identity, ethics, kernel, routing, or provider behavior is modified.
- No live workspace DVCS store is initialized.
- This directory is not conventional Git authority, so verification evidence replaces commit steps.

---

### Task 1: Five-language red contract

**Files:**
- Create: `lang/tests/compiler/fixtures/hello_graph.axi`
- Create: `lang/tests/compiler/fixtures/hello.c`
- Create: `lang/tests/compiler/fixtures/hello.cpp`
- Create: `lang/tests/compiler/fixtures/hello.cs`
- Create: `lang/tests/compiler/fixtures/hello.py`
- Create: `lang/tests/compiler/compiler_routes.axi`

**Interfaces:**
- Consumes: `Axi_compiler.exe <input> <output>`.
- Produces: five marker executables and an `.axi` route runner that requires `AXI_OK`, `C_OK`, `CPP_OK`, `CS_OK`, and `PY_OK`.

- [ ] Write minimal fixtures with one deterministic marker each.
- [ ] Write `compiler_routes.axi` using `@C_Native` only for Windows process and file assertions; it invokes the compiler, requires exit `0`, requires the requested artifact, runs it, and compares its marker.
- [ ] Run the checked-in coordinator against all fixtures and record the existing red result: exit `0`, requested artifact absent.

### Task 2: Relocatable coordinator and native C/C++ routes

**Files:**
- Modify: `lang/Axi_compiler.c`
- Create: `lang/tests/compiler/fixtures/invalid.c`

**Interfaces:**
- Consumes: input path and output path.
- Produces: `int run_native_backend(const wchar_t *tool, const wchar_t *input, const wchar_t *output, const wchar_t *standard)` returning nonzero on launch failure, child failure, or absent output.

- [ ] Add a failing invalid-C fixture and prove the current coordinator returns false success.
- [ ] Resolve the Axi installation from the running executable and locate bundled `gcc.exe` and `g++.exe` relative to it.
- [ ] Launch tools without `system()` string interpretation, wait for completion, propagate the child exit code, and verify the requested output is a regular file.
- [ ] Route `.c` through GCC with `-std=c11 -Wall -Wextra -Werror -pedantic`.
- [ ] Route `.cpp` through G++ with `-std=c++20 -Wall -Wextra -Werror -pedantic`.
- [ ] Rebuild an isolated coordinator candidate and prove valid C/C++ fixtures pass while invalid C fails without an output.

### Task 3: Truthful `.axi` bootstrap route

**Files:**
- Modify: `lang/Axi_compiler.c`
- Test: `lang/tests/compiler/fixtures/hello_graph.axi`

**Interfaces:**
- Consumes: graph/node `.axi` source supported by the present bootstrap parser.
- Produces: generated `<output>.c` plus the requested native executable.

- [ ] Generate C beside the requested output rather than the process working directory.
- [ ] Emit `g_argc`, `g_argv`, and a main return path needed by `.axi`-authored tool drivers.
- [ ] Compile generated C through the same verified GCC backend.
- [ ] Propagate parse, generation, backend, and artifact-verification failures.
- [ ] Run the `.axi` fixture and require `AXI_OK`.

### Task 4: Dedicated C# route

**Files:**
- Modify: `lang/Axi_compiler.c`
- Test: `lang/tests/compiler/fixtures/hello.cs`

**Interfaces:**
- Consumes: one `.cs` source file and requested `.exe` path.
- Produces: a .NET 8 application host and required companion artifacts in the requested output directory.

- [ ] Create an isolated temporary SDK project with `EnableDefaultCompileItems=false`, an explicit source include, and an assembly name derived from the requested output.
- [ ] Locate bundled `dotnet.exe` relative to Axi and run `dotnet build` with a requested output directory.
- [ ] Require the requested `.exe`, `.dll`, `.deps.json`, and `.runtimeconfig.json`.
- [ ] Remove only the validated temporary project directory.
- [ ] Run the output and require `CS_OK`.

### Task 5: Dedicated generic Python route

**Files:**
- Create: `lang/src/pipelines/python_to_c.py`
- Modify: `lang/Axi_compiler.c`
- Test: `lang/tests/compiler/fixtures/hello.py`

**Interfaces:**
- Consumes: `python_to_c.py <input.py> <output.c>`.
- Produces: strict C11 for a documented initial subset; unsupported AST nodes exit nonzero with a source location.

- [ ] Implement a generic AST emitter for module-level `print` calls with string, integer, float, and boolean constants.
- [ ] Reject imports, calls other than `print`, dynamic expressions, classes, async constructs, filesystem actions, and cognition-specific interceptors.
- [ ] Route `.py` through the generic emitter and then the verified GCC backend.
- [ ] Prove the valid fixture runs with `PY_OK` and an unsupported fixture fails without an executable.
- [ ] Keep `cortex_transpiler.py` unchanged as Psyche/Cortex evidence; do not import it into the base route.

### Task 6: Native `.axi` conformance runner and promotion evidence

**Files:**
- Modify: `lang/tests/compiler/compiler_routes.axi`
- Create: `lang/tests/compiler/README.md`
- Modify: `docs/architecture/Axi_compiler_pipeline_inventory.md`

**Interfaces:**
- Consumes: isolated coordinator candidate and five fixtures.
- Produces: one exit code and a five-route pass/fail summary.

- [ ] Compile `compiler_routes.axi` through the isolated candidate.
- [ ] Run the generated native route runner from the workspace root.
- [ ] Require all five valid markers and the invalid C/Python negative cases.
- [ ] Scan active coordinator and pipeline source for `C:\Antigravity` and require no matches.
- [ ] Hash the candidate and record exact compiler, route-runner, and fixture results.
- [ ] Preserve the old compiler executable in `network/tmp/Axi-wrap-sdd` before any live replacement.
- [ ] Replace the live compiler only when the candidate and `.axi` runner both pass.

### Task 7: Resume the DVCS fix through Axi

**Files:**
- Create: `network/Axi_dvcs/tests/compiler_dvcs_contract.axi`
- Modify: `network/Axi_dvcs/build.ps1` only to mark it superseded; do not extend it
- Modify: `network/Axi_dvcs/tests/run_behavior_tests.ps1` only to mark it superseded; do not extend it
- Modify: `network/tmp/Axi-wrap-sdd/task-2-report.md`

**Interfaces:**
- Consumes: verified base-Community AVM and existing focused DVCS C modules.
- Produces: `.axi`-authored build/verification entrypoint and truthful Task 2 evidence.

- [ ] Author an `.axi` driver that invokes the verified C backend with the DVCS module list and `bcrypt` link requirement.
- [ ] Port the eleven existing behavioral cases into native `.axi` assertions using isolated temporary roots.
- [ ] Run strict compilation, native identity/path verification, and all behavioral cases.
- [ ] Record uncovered failure-injection and concurrency requirements explicitly rather than claiming the full 23-case matrix.
- [ ] Run `Axi wrap` only against an isolated initialized fixture until a real workspace store is separately authorized.


