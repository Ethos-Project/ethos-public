# Task 1 — Red baseline report

## Status

Expected red baseline reproduced. The checked-in `axi.exe` returned success while the native black-box contract test found no durable WIP session ref.

## Files changed

- `lang/axi_dvcs/tests/test_current_wrap_contract.c`
- `lang/axi_dvcs/tests/run_red_baseline.ps1`
- `tmp/axi-wrap-sdd/task-1-report.md`

## Exact compile command

```text
C:\Ethos\ethos-products\axi\lang\bootstrap\python_to_c_compiler\bin\mingw64\bin\gcc.exe -std=c11 -Wall -Wextra -Werror -pedantic C:\Ethos\ethos-products\axi\lang\axi_dvcs\tests\test_current_wrap_contract.c -o C:\Ethos\ethos-products\axi\tmp\axi-wrap-sdd\test_current_wrap_contract.exe
```

Compile process exit code: `0`.

## Exact test command

```text
C:\Ethos\ethos-products\axi\tmp\axi-wrap-sdd\test_current_wrap_contract.exe C:\Ethos\ethos-products\axi\lang\axi_dvcs\axi.exe
```

Test process exit code: `1` (intentional red result). The PowerShell runner propagated that exit code and removed its temporary test executable.

## Expected red failure

```text
FAIL: axi wrap exited 0 but did not create durable session ref 'C:\Users\theca\AppData\Local\Temp\\axi-wrap-red-10956-218797546-0\.axi\refs\wip\red-baseline-session'. Console output is not evidence of durability.
```

The native test created an isolated version 1 synthetic root with the required store layout, manifest, and payload; launched the checked-in executable there with `wrap --root <synthetic-root>`; required a lowercase 64-hex session ref and the addressed fan-out object; and then removed the synthetic root. It does not inspect console text as durability evidence.

## Concerns

The current executable prints an authoritative-success claim and exits `0`, but writes neither the required session ref nor its object. This is the specified review gate: do not rebuild or replace `axi.exe` until the red-baseline evidence is reviewed.

## Review-fix evidence — round 1

### Cleanup containment

`remove_tree` now opens every directory itself with `FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS`, `DELETE | FILE_LIST_DIRECTORY | FILE_READ_ATTRIBUTES` access, and `FILE_SHARE_READ` only. It reads `FileAttributeTagInfo` from that handle before any path-based enumeration. The held directory handle therefore denies concurrent write/delete opens while its children are walked. A reparse-point directory is marked for deletion through `SetFileInformationByHandle(FileDispositionInfo)` and closed without enumeration. Ordinary directories are likewise deleted through their already-open handles after their children have been removed. This is a smaller handle-anchored approach than trying to resolve targets: it never recurses into a handle identified as a reparse point.

Fixture setup initializes the root output to empty, records the root only after its unique directory exists, and invokes the same cleanup on every later setup failure. Cleanup now returns success/failure; any cleanup failure emits an explicit `FAIL` message and returns exit code `2`, superseding a normal or red assertion result.

The PowerShell runner now encloses build and test invocation in `try/finally`, so a terminating error also attempts removal of the isolated temporary test executable.

### One-line test summary

`TEST SUMMARY: strict compile exit 0; native contract test exit 1 (expected red): axi wrap exited 0 but did not create a durable session ref.`

### Re-run commands and exits

```text
C:\Ethos\ethos-products\axi\lang\bootstrap\python_to_c_compiler\bin\mingw64\bin\gcc.exe -std=c11 -Wall -Wextra -Werror -pedantic C:\Ethos\ethos-products\axi\lang\axi_dvcs\tests\test_current_wrap_contract.c -o C:\Ethos\ethos-products\axi\tmp\axi-wrap-sdd\test_current_wrap_contract.exe
```

Compile process exit code: `0`.

```text
C:\Ethos\ethos-products\axi\tmp\axi-wrap-sdd\test_current_wrap_contract.exe C:\Ethos\ethos-products\axi\lang\axi_dvcs\axi.exe
```

Test process exit code: `1` (intentional red result). The native assertion remained the expected missing-session-ref failure, and no cleanup failure message occurred.

### Resumed verification run

`TEST SUMMARY: strict compile exit 0; native contract test exit 1 (expected red): axi wrap exited 0 but did not create a durable session ref.`

The same strict compile and native test commands above were rerun after the round-1 fixes. The test again returned `1` only for the expected missing-session-ref assertion; it did not report a cleanup failure.

## Review-fix evidence — round 2

`remove_tree` now treats `FindFirstFileA(path\\*) == INVALID_HANDLE_VALUE` with `GetLastError() == ERROR_FILE_NOT_FOUND` as an empty directory and deletes that directory through its already-open cleanup handle. Any other enumeration error still closes the handle and fails cleanup. The prior machine run returned the expected test exit `1` without taking this branch, but the case is now explicit for portability.

### Exact re-run commands and exits

```text
C:\Ethos\ethos-products\axi\lang\bootstrap\python_to_c_compiler\bin\mingw64\bin\gcc.exe -std=c11 -Wall -Wextra -Werror -pedantic C:\Ethos\ethos-products\axi\lang\axi_dvcs\tests\test_current_wrap_contract.c -o C:\Ethos\ethos-products\axi\tmp\axi-wrap-sdd\test_current_wrap_contract.exe
```

Compile process exit code: `0`.

```text
C:\Ethos\ethos-products\axi\tmp\axi-wrap-sdd\test_current_wrap_contract.exe C:\Ethos\ethos-products\axi\lang\axi_dvcs\axi.exe
```

`TEST SUMMARY: strict compile exit 0; native contract test exit 1 (expected red): axi wrap exited 0 but did not create a durable session ref.`
