# Task 1 — Establish the red baseline

Read first: `docs/superpowers/specs/2026-08-24-axi-wrap-durable-wip-design.md` is the binding design.

## Goal

Add a native black-box regression test that proves the checked-in print-only `lang/axi_dvcs/axi.exe` falsely exits successfully without creating a durable WIP object and session ref.

## Constraints

- Do not edit production source or replace `axi.exe`.
- Do not run Git or any Python file/interpreter.
- Do not touch `lang/axi_dvcs/.axi`.
- Test only in a unique temporary root.
- Use a native C test executable. PowerShell may only compile and invoke the native test and clean its isolated temporary output.
- Resolve the compiler relative to the workspace at `lang/bootstrap/python_to_c_compiler/bin/mingw64/bin/gcc.exe`; the directory name is historical, but only `gcc.exe` may be executed.
- Compile with strict warnings. Build artifacts go under the workspace `tmp` directory, not beside the checked-in executable.

## Required behavior

1. The C test creates a synthetic version 1 root with `.axi/FORMAT`, `objects`, `refs/wip`, `refs/previous`, `tmp`, a valid `wrap-manifest.json`, and one regular payload file.
2. It launches the real checked-in `axi.exe wrap --root <synthetic-root>` with the synthetic root as its working directory.
3. It asserts that exit code zero is insufficient: a lowercase 64-hex session ref and its addressed object must both exist. The current executable must make the test fail for the expected missing-ref/object reason.
4. It never treats console text as proof of durability.
5. It cleans its synthetic root after the assertion while leaving a clear failure message.

## Files

- `lang/axi_dvcs/tests/test_current_wrap_contract.c`
- `lang/axi_dvcs/tests/run_red_baseline.ps1`
- Report: `tmp/axi-wrap-sdd/task-1-report.md`

## Report contract

Record files changed, exact compile/run commands, process exit codes, and the expected red failure. Return only status, one-line test summary, and concerns.
