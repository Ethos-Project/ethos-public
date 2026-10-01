# Task 2 — Native axi DVCS implementation report

Date: 2026-08-27  
Status: Focused local v1 implementation verified and promoted; extended matrix remains open

## Outcome

The print-only `network\axi_dvcs\axi.c` and `axi.exe` were replaced with
the verified native implementation after preserving the previous files under
this ledger directory. The live CLI now implements truthful local-only `init`,
durable content-addressed `wrap`, and verified empty-destination `resume`.

The canonical build/smoke-verification entrypoint is now axi source:
`network\axi_dvcs\build_and_verify.axi`. It strictly compiles the focused C
modules with the bundled GCC and `bcrypt`, creates only a unique temporary v1
store, wraps known bytes, verifies the session ref, resumes into an empty
directory, compares the restored bytes, and removes the fixture.

No DVCS store was initialized under the real axi workspace. A live
`wrap --root C:\Ethos\ethos-products\axi` returned exit `3`, reported no
valid v1 store, and left the root `.axi` path absent.

## Live files

- `network\axi_dvcs\axi.c`
- `network\axi_dvcs\axi_store.c`
- `network\axi_dvcs\axi_manifest.c`
- `network\axi_dvcs\axi_object.c`
- `network\axi_dvcs\axi_wrap.c`
- `network\axi_dvcs\axi_resume.c`
- `network\axi_dvcs\axi_internal.h`
- `network\axi_dvcs\axi.exe`
- `network\axi_dvcs\build_and_verify.axi`

## Preserved print-only baseline

- source: `axi_dvcs_print_only_bdfe6696.c`
  (`bdfe6696ec27bdfcce823bfe69ed4aaa32de53b89a9624dd99a227e1ec021971`)
- executable: `axi_dvcs_print_only_3c1afb6d.exe`
  (`3c1afb6d4dd0cf9a84851a013c400e389277903ba1dcc54a210c79fd52e5123e`)

## Verification evidence

Canonical `.axi` driver build:

```text
lang\axi_compiler.exe network\axi_dvcs\build_and_verify.axi network\tmp\axi-wrap-sdd\dvcs-build-and-verify.exe
```

Exit `0`; generated C was strictly compiled and the requested executable was
verified.

Canonical `.axi` DVCS verification:

```text
network\tmp\axi-wrap-sdd\dvcs-build-and-verify.exe C:\Ethos\ethos-products\axi C:\Ethos\ethos-products\axi\network\tmp\axi-wrap-sdd\axi-rc-rebuilt.exe
```

Exit `0`:

```text
PASS dvcs strict native build
PASS dvcs init-wrap-resume durable round trip
PASS native .axi DVCS verification
```

Post-promotion supplemental suites, using PowerShell 7 because the historical
Windows PowerShell 5.1 harness is incompatible with the current duplicate-case
PATH environment and lacks `Convert.ToHexString`:

```text
pwsh.exe -NoProfile -File network\axi_dvcs\tests\run_behavior_tests.ps1 -axiExecutable C:\Ethos\ethos-products\axi\network\axi_dvcs\axi.exe
```

Exit `0`: `TEST SUMMARY: 11 passed; 0 failed`.

```text
pwsh.exe -NoProfile -File network\axi_dvcs\tests\run_native_unit_tests.ps1 -axiExecutable C:\Ethos\ethos-products\axi\network\axi_dvcs\axi.exe
```

Exit `0`: shared path safety and content-addressed object identity verified.

Live false-success regression:

```text
network\axi_dvcs\axi.exe wrap --root C:\Ethos\ethos-products\axi
```

Exit `3`; no root `.axi` directory was created.

## Current SHA-256

- live source coordinator:
  `63d0715ce914810c583be1269aaa08e600f8008de035a851c0c321082cfd264c`
- live executable rebuilt from canonical `axi.c`:
  `e57df80273be7b7eac8c94045cd073b6729df98393ac9bb4c9b79228c7c5c4e6`

## Explicitly uncovered requirements

The current evidence does not claim the entire extended specification matrix.
Compile-time failure injection for short writes, flush, promotion, ref
transition, verification, and rollback remains to be exercised. Concurrent
payload mutation and a broader corrupt/truncated/trailing object matrix also
remain open. The eleven supplemental cases have not all been ported into native
`.axi` assertions yet. Ethos Server synchronization, real-store initialization,
and protected cognition capture remain separately unauthorized and unimplemented.

## One-line summary

Live local DVCS: strict native build PASS; `.axi` durable round trip PASS;
behavior 11/11 PASS; object identity PASS; real-root false-success regression PASS.
