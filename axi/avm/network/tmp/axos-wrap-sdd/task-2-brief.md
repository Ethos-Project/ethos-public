# Task 2 — Implement and verify a release candidate

Read first: `docs/superpowers/specs/2026-08-24-axi-wrap-durable-wip-design.md` is binding. Task 1's accepted red test is the proof that current behavior is broken.

## Goal

Implement the complete local-only v1 `init`, `wrap`, and safe `resume` contract as native Windows C, compile a release candidate outside the checked-in executable path, and prove it against isolated synthetic roots.

## Global constraints

- No Git/GitHub and no Python files or interpreter.
- Do not touch or migrate `lang/axi_dvcs/.axi`.
- Do not initialize a real workspace/product store.
- Do not capture or modify Eros cognition, memory, identity, ethics, kernel, routing, `eros_flat_core`, or protected axi mathematics.
- Do not contact Ethos Server. Successful wrap must report local durability and server-not-attempted separately.
- Use the workspace-relative MinGW GCC at `lang/bootstrap/python_to_c_compiler/bin/mingw64/bin/gcc.exe`; only the compiler executable is used.
- Use Windows CNG SHA-256 (`bcrypt`) and Windows durability APIs described by the specification. Do not invent a second database or object model.
- Never report success unless the object and final session ref were reopened and independently verified.
- Build candidates and test binaries under `tmp/axi-wrap-sdd`; do not replace `lang/axi_dvcs/axi.exe` in this task.
- Use `apply_patch` for source/test edits.
- Do not spawn subagents.

## TDD sequence

1. Extend native black-box/unit tests for the required CLI, store, object/ref, recovery, corruption, and failure behavior.
2. Run the new behavior tests against the current executable or absent APIs and record the expected failures before production implementation.
3. Implement the minimum focused modules needed to pass.
4. Run strict compilation and the entire native test suite.

## Authorized source boundaries

- `lang/axi_dvcs/axi.c` — command parsing/dispatch and truthful output.
- New focused `axi_*.c` / `axi_*.h` files directly under `lang/axi_dvcs` for store, manifest, object, wrap, and resume responsibilities.
- `lang/axi_dvcs/tests/**` — native C tests and PowerShell orchestration only.
- `lang/axi_dvcs/build.ps1` — relative release-candidate build only.
- Report: `tmp/axi-wrap-sdd/task-2-report.md`.

## Required implementation

- Root discovery and strict `FORMAT` validation exactly as specified.
- Fail-closed initialization with the exact v1 directory/manifest layout.
- Strict manifest parser: exact fields only; opaque validated IDs; sorted/deduplicated arrays; explicit regular root-relative files; no `.axi`, absolute/traversing/duplicate/glob/directory/reparse/escaping path.
- Stable file capture with handles held open, write/delete sharing denied, metadata checked before and after, and valid UTF-8 canonical paths.
- Canonical `AXSTATE1` state digest and `AXWIP001` object encoding exactly as specified, using big-endian fixed widths and SHA-256/CNG.
- Fan-out immutable object publication through exclusive same-store temp, full write checks, `FlushFileBuffers`, reopen/parse/hash verification, atomic same-volume move, existing-object verification, and final-path verification.
- Strict object parser: reject bad magic/version/parent flag, overflow, truncation, trailing bytes, invalid UTF-8/path order/duplicates, payload digest mismatch, state digest mismatch, and object-ID mismatch.
- Session refs at `refs/wip/<session>` and verified previous refs at `refs/previous/<session>`, exact 64-lowercase-hex plus newline content, write-through atomic transition, post-transition resolution, and verified rollback/absence on failure.
- Idempotent repeated wrap by state digest; changed state links to the prior object.
- `resume --session <id> --to <empty-directory>` fully verifies before output, rejects unsafe/nonempty/reparse destinations, writes via temp/flush/atomic promote, and verifies restored bytes.
- Exit codes and stdout/stderr truthfulness exactly match the design.
- Failure injection exists only under a compile-time test build and covers write, flush, promote, and ref-transition boundaries.

## Minimum native verification matrix

Cover every numbered test in specification section 15, adapting its obsolete Python-harness sentence to native C plus PowerShell orchestration. At minimum provide explicit evidence for:

- missing/unversioned/partial/malformed store failures without mutation;
- exact init layout;
- invalid manifests and unsafe paths;
- successful durable object/ref plus independent resolver verification;
- unchanged wrap deduplication and changed-state parent lineage;
- issue/context opaque lineage round trip;
- concurrent mutation detection;
- injected object/ref write, flush, promotion, verification, and rollback failures with no dangling ref;
- corrupt/truncated/trailing objects and malformed/dangling refs fail closed;
- incomplete temp files ignored;
- empty-destination recovery with exact path/byte comparison and unsafe recovery rejection;
- local-only versus server-not-attempted output;
- no fallback to `lang/axi-lang`;
- the live unversioned `lang/axi_dvcs/.axi` remains unchanged.

## Report contract

Record the red evidence, files changed, architecture decisions, exact compile/test commands and exits, test counts, any uncovered specification requirement, and SHA-256 of the candidate executable. Return status, one-line test summary, and concerns only.
