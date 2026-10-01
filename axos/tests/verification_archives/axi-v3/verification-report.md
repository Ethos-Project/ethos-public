# Axi Infrastructure v3 — cross-agent verification

Date: 2026-09-07

Work item: [WRK-15](https://app.notion.com/p/3d4b679aa7008167b710c236017b8769)

## Disposition

The FOSS/Internal architectural decision remains completed and maintainer-confirmed. Technical acceptance does not pass this bounded verification: the shipped binaries reproduce material failures. Native C round-trip and checkout-link execution remain environment-blocked, not independently demonstrated failures.

The founder explicitly authorized this verification and routine automatic verification of cross-agent work. This does not expand authority for protected implementation changes.

## Scope and reproducibility

Executed the actual designated public and internal binaries from a new scratch workspace, using synthetic fixtures only. Product source was read without modification. No production ship operation was run: the inspected internal ship command writes into its current directory, and all tests used isolated stores beneath the run directory.

- Harness: `C:/Ethos/verification/axi-v3/verify.py`
- Focused follow-up: `C:/Ethos/verification/axi-v3/followup.py`
- Full commands, exit codes, stdout/stderr, source/binary SHA-256 manifest, and observations: `C:/Ethos/verification/axi-v3/run-20260907-132104-43313a/results.json`
- Reproduce: run `python verify.py`, then `python followup.py` from this evidence directory. Each primary run creates a new scratch directory; the follow-up uses `latest-run.txt`.

## Confirmed behavior

1. Internal `init` creates a local ledger; `track tracked` writes the scratch directory into its configuration.
2. Public `init` exits 1 and creates no ledger, supporting command separation for the tested entry point. This is not a full distribution or proprietary-content audit.
3. Public `build main.axi` emits C for a minimal integer-print function. A native executable was not produced because `g++` is unavailable.

## Reproduced failures

1. **Snapshot and release payloads are missing.** A tracked file contained a unique marker. Internal `wrap` and `ship` returned 0 but their complete objects contained only `[Snapshot Data Placeholder]` and `[Release Payload Placeholder]`; neither contained the marker or a file manifest. Source `LANG/src/compiler/dvcs.cpp` confirms those literal payloads and time/PRNG identifiers instead of content hashing.
2. **Failed object writes advance refs.** In a separate scratch store, `.axi/objects` was deliberately a regular file, making object creation impossible. Internal `ship` returned 0, printed its production-success message, and replaced the main ref's `BASELINE_SENTINEL` with an identifier despite having written no object. The source never checks successful object persistence before updating the ref.
3. **Errors report successful process exits.** Internal `wrap` without a store and `build absent.axi` both returned 0 while reporting an error. The public minimal build also returned 0 despite the child C++ compiler failing and no `app.exe` being produced.
4. **Documented/public DAG inputs fail parsing.** The README hello-world example failed at `start`. The repository's `LANG/release/test_dag.axi` failed at `import`. Direct compilation of `FOSS/axi_compiler/lib/core/dag.axi` failed at `/`. These are exact observed parser failures, not a claim that every possible Axi program fails.

## Source observations and environment limits

- Public `dag_native.c` still returns simulated read metadata and copies JSON input unchanged. The checkout symbol referenced by `dag.axi` is absent from the inspected C/header pair.
- Compiling native probes with installed Clang 22.1.8 first failed for missing `stdio.h`. Supplying the installed Windows SDK UCRT include directory reached a missing `vcruntime.h`. No toolchain was installed or product source altered to work around this. Native round-trip and checkout-link behavior therefore remain unexecuted.
- Public build output requests `g++`, Raylib, llama, and a `lib/core/dag_native.c` path adjacent to its executable. The inspected executable directory contains only the executable. Full package isolation and reproducible release builds remain unverified.
- The supplied release receipt exists and contains a placeholder; this verification does not demonstrate a signed release or deployment.

## Preservation and wrap

Before/after SHA-256 values match for all 11 inspected source, binary, handoff, live receipt, and live release-ref files. This is a bounded preservation check, not a full-workspace non-mutation assertion.

Required wrap ran in the isolated ledger and created `wip-18d319a2f2e9c16400000000fb6006729bba2e86`. Its placeholder contents are preserved in the results. It proves command execution, not capture of this report or Notion content.

## Next review gate

Update WRK-15 with these findings and retain Needs Review. A bounded implementation task should address durable payload persistence, failed-write ref protection, error exit propagation, and public DAG compiler compatibility. The missing native toolchain must be resolved to execute the remaining C tests. Implementation choices and protected repairs require their existing approval; the verification itself is already authorized.
