# Axi Open-Core verification — 2026-09-08

Work item: [WRK-15](https://app.notion.com/p/3d4b679aa7008167b710c236017b8769).

## Result

Builds, tested public-command separation, short-input SHA3, semantic type metadata, and remote synchronization pass. Cryptographic and failed-write acceptance remain incomplete.

### Confirmed

- Rebuilt the internal compiler from its six C++ translation units with `-DAxi_INTERNAL_BUILD`, the public compiler without that macro or `dvcs.cpp`, and standalone FOSS DVCS from `main.cpp`. All exited 0 with no compiler/linker diagnostics. Output was confined to a new scratch folder.
- The supplied `build_internal.sh` was absent at the initial read and appeared during verification. Its translation-unit list and internal macro match the isolated build; the release-writing script itself was not executed.
- Both packaged and rebuilt public compilers reject `init` with exit 1 and create no ledger. This confirms the tested command gate, not a full public-package dependency or IP audit.
- Packaged and rebuilt standalone FOSS DVCS create objects with `type_ref` and `size_bytes`. For the 104-byte metadata-plus-timestamp inputs, the identifiers match Python `hashlib.sha3_256` exactly.
- Read-only `git ls-remote origin refs/heads/master` confirms remote `f16e4e405f89cfa3f6e3a477962bd6090a7c665c`. Earlier in this verification it returned `207a69d5d3cf7727e99cb82cc4cbd3b5806216e7`. The local public checkout was clean when inspected. No push or fetch was performed.

### Remaining failures

1. **SHA3 implementation fails at the block boundary.** Empty input, `abc`, and 135 repetitions of `a` pass comparison with `hashlib.sha3_256`; inputs of 136, 137, 272 and 1000 repetitions fail. The tested FOSS header has SHA-256 `05a06fb24c706fb16ca5921d8936880fd7d633c71bba6f753418ffeec6c25c03`. After processing a 136-byte block, `update` resets the length but leaves old bytes in the buffer; `finalize` XORs padding into that stale tail. The tested public header initially matched this file.
2. **Actual long-payload wrap fails independent digest verification.** A three-file ledger produces a 267-byte metadata-plus-timestamp hash input. Expected SHA3-256: `e9fef7abbc5a5a54275f0a08c4d84c1a9a0b6cd738ee6b3d0377edfeb5f6de10`; emitted identifier: `ec14c6cd104e7cbc20c47313013d8d8a853f1a247e32214b83602b096ce18394`. A 64-character hexadecimal shape alone does not establish hash correctness.
3. **FOSS failed writes still advance refs.** In separate synthetic stores with object creation unavailable, both packaged and rebuilt FOSS `wrap` return 0, report success and replace `main`, despite no object being written.
4. **Revised supplied receipt not located.** `FOSS/axi_tui/.axi/objects/4614ffdfa6a16086bbf4479e390c58a5e88cbffc9c90ebfcbd50b86c2d1b0d2d` was absent when checked; that workspace's main ref still named `1a1125a9f5540714a7f3af564c07210a14a9771a11a39289657e2ed9dea45dbe`. The latter object exists, but its local timestamp/content differs from the originally pasted excerpt. Its identifier matches the earlier timestamp-only input. Treat these as distinct evidence versions.

The revised FOSS source does now hash semantic metadata plus timestamp; the earlier timestamp-only finding is superseded for the current source. Metadata covers immediate file names, types and sizes. It does not bind file contents or establish restorable snapshots. Full object-file SHA3 equality is not required by the current input contract, so that raw-file comparison is diagnostic only.

## Architectural authority

The founder's latest direction supersedes the Raylib Terminal design: native `.axi` console TUI via `core::tui` and stdin/stdout. Public compilation belongs in `FOSS/axi_compiler/axic_public.exe`; FOSS DVCS commands belong in standalone `FOSS/axi_dvcs/axi.exe`; internal commands belong in `LANG/release/axic_internal.exe`. `INTERNAL/ethos_tui` remains an independent internal environment. The native C program retains its consumer-product role. This verification does not reopen those decisions.

## Reproduction and preservation

- Run `C:/Ethos/verification/axi-v3/proof-20260908.py`, then `revised-proof-check.py` with Python.
- Exact commands, logs, objects, independent expected digests and file manifests: `C:/Ethos/verification/axi-v3/proof-20260908-173329-5654e7/results.json`.
- Required `wrap` runs took place in isolated scratch stores. Their results are preserved in that JSON; they do not attest to persistence of this report or Notion content.
- No product source was edited by Codex. The internal `LANG/src/compiler/sha3.hpp` changed concurrently after the primary run; results are tied to recorded hashes rather than a claim of whole-workspace stability. This report does not certify that later internal header revision.
- Stop at verification: no cryptographic implementation repair, ref repair, source relocation, publication or architecture change was performed.
