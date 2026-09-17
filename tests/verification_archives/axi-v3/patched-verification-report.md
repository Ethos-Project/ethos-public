# Axi Infrastructure v3 — patch re-verification and Terminal plan review

Date: 2026-09-07

Work item: [WRK-15](https://app.notion.com/p/3d4b679aa7008167b710c236017b8769)

## Decision

Partial fixes verified; infrastructure technical acceptance is not satisfied. The founder's FOSS/Internal architecture decision remains completed. The founder clarified that the pure Axi/Raylib FOSS Terminal implementation plan was already approved and the native C program is retained as a separate consumer product. The earlier suggestion to reconcile competing architectures or seek renewed plan approval is withdrawn. The tested public package still has the implementation failures below.

## Current evidence

Exact commands, exit codes, logs, generated objects, and SHA-256 manifests are in `C:/Ethos/verification/axi-v3/run-20260907-134304-452059/results.json`.

Run `verify.py`, then `followup.py`, then `patched-reverify.py` from this evidence directory with Python. The first script creates a new isolated store; the following scripts use `latest-run.txt`. Product source is not edited. The original verification report remains historical evidence.

### Verified improvements

- The new receipt `18d31a951fb7b0c800000000f760c9d5317ef257` exists and matches the live release main ref. Its supplied listing matches exactly. There is no placeholder literal in this receipt.
- Internal object-open failure now leaves the isolated main ref at `BASELINE_SENTINEL`, with no success message. This closes the specific ref-advancement-on-failed-open reproduction. It does not prove atomicity, flush/close integrity, or crash recovery.
- The parser accepts `import core::dag`, and the emitter produces `#include "core/dag_native.h"`.
- The internal compiler builds an import-only integer-print program using its bundled GCC and configured Raylib path; executing the generated program exits 0 and prints `42`. The program does not exercise a Raylib window or DAG function.
- The native public C backend compiles with bundled GCC 16.2.0. The earlier Clang header blocker no longer prevents this verification because the bundled GCC was located and used without installation.

### Remaining failures and regression

1. **Public/Internal binary separation regressed.** The designated public and internal executables have identical bytes and SHA-256 `1AD61DA66EA1FC602E58E504BD6CC76D166033D19AD37BDB1BB18E32CFF3516C`. The public binary advertises the Internal Orchestrator commands and successfully creates a ledger on `init`. Its build command uses an absolute compiler path under proprietary `LANG`. This contradicts the locked isolated-public-engine boundary for these artifacts.
2. **Payloads contain names, not snapshots.** The new implementation uses `directory_iterator` over each tracked directory. A test snapshot contains `[tracked:fixture.txt|subdir]`, omitting changed file contents and `subdir/nested.txt`. No referenced file-content objects are created. The new production receipt is likewise an immediate directory listing. Removing placeholder text has not established restorable file history. IDs still derive from time/PRNG, not content hashing.
3. **Failure exit codes remain incorrect.** Missing-store wrap, missing-input build, failed object creation, and failed public native build still return 0.
4. **The repository DAG example still fails.** Both designated binaries now pass the import parse but fail on `Call to unknown function: query`. The README example still fails at `start`, and direct public DAG API compilation still fails at `/`.
5. **Public native packaging still fails.** A minimal program emits C, then GCC cannot locate `bin/windows-x64/lib/core/dag_native.c`; no public-built executable is produced. The process nevertheless exits 0.
6. **Native DAG round-trip fails.** Directly compiled C commit writes the supplied marker to an object, but query returns `{ "status": "loaded from YAML ledger" }`. The probe exits 2 because the committed payload was not returned.
7. **Native checkout is absent.** The GCC checkout probe fails to link with `undefined reference to __native_dag_checkout`. This is now an executed result, not an environment-blocked source inference.

## Terminal implementation-plan review

Reviewed `C:/Users/theca/.gemini/antigravity/brain/7e7090fa-939b-4c07-971b-c13f49bab2a2/implementation_plan.md`, titled **Axi Native Terminal — FOSS Edition Plan**.

The proposal creates `FOSS/axi_terminal/main.axi` and `FOSS/axi_compiler/lib/core/graphics.axi`, uses a pure Axi Raylib loop, and includes keyboard input, history, and text rendering. Its current automated verification is compilation; manual verification is window/input/prompt behavior.

The following are implementation review considerations within the approved direction, not a request to reopen plan approval:

- **Design authority — resolved by founder:** the pure Axi/Raylib plan governs the FOSS Terminal. The native C program is retained as a consumer product. The earlier C/.NET foundation document is not an architectural conflict requiring supersession for the FOSS Terminal.
- **Public prerequisite:** require a genuinely separated public binary and a reproducible build in a clean FOSS package without an absolute proprietary LANG dependency. Prove a minimal imported graphics call compiles, links, and runs before relying on it for terminal implementation.
- **Native binding deliverables:** name the adapter/header and exported-function contract. The inspected emitter maps `import core::graphics` to `core/graphics_native.h`, but the proposal names only `graphics.axi`. Define how declarations become known to analysis and map to Raylib symbols; the query failure demonstrates this integration cannot be assumed.
- **Command scope:** specify whether this milestone is a window/input/history shell or actually executes commands. Define the bounded grammar, process invocation, diagnostics, output limits, and startup/no-write behavior where applicable. Explicitly decide how the earlier plan's deferred DVCS commands carry forward.
- **Artifact contract:** reconcile the promised `axi_terminal.exe` with the tested compiler's fixed `app.exe` output and specify the Raylib runtime/package layout.
- **Acceptance:** add tests for imported FFI resolution, input/history bounds, error propagation, missing dependencies, and public-package independence. Treat memory safety and performance as targets requiring evidence, not established capabilities.

The founder's plan approval is recorded above. Compiler/DAG verification findings are implementation evidence, not grounds to reopen that architectural decision. This verification task made no plan or product implementation changes.

## Preservation and wrap

Before/after manifests match for the 11 original evidence paths and 8 additional paths, including the new receipt, parser/emitter, bundled compiler, release DAG files, example, and plan. The live release ref remained unchanged during testing.

Required wrap ran only in the isolated ledger. Latest WIP: `wip-18d31ae59383726c0000000044b0312e1861d79d`. It contains a filename listing and does not prove persistence of this report or Notion content.

