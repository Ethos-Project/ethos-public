# SDD ledger — plan: docs/superpowers/specs/2026-08-24-axi-wrap-durable-wip-design.md

- No Git authority; review evidence is filesystem- and test-based.
- Ruling: Use the workspace-bundled GCC executable only as a native compiler; do not use or add Python implementation or test files.
- Ruling: The existing `lang/axi_dvcs/.axi` is an unversioned issue/dependency store and remains byte-for-byte out of scope.
- Ruling: Subagents may perform scoped work, but implementation tasks are serialized because all agents share the workspace.
- Task 1: complete (native red baseline, strict compile exit 0, expected contract exit 1; review clean after two cleanup-hardening rounds).
- Task 1 review ruling: cleanup must be handle-contained and fail closed; test evidence is invalid if cleanup can traverse a reparse point or silently leave artifacts.
- Task 2 focused local v1: verified and promoted at `network\axi_dvcs`; canonical `.axi` strict build/round-trip PASS, supplemental behavior 11/11 PASS, native identity PASS.
- Live truthfulness: `wrap` against the uninitialized axi workspace returns exit 3 and creates no `.axi` store.
- Remaining Task 2 extensions: failure-injection, concurrency, expanded corrupt-object matrix, and full native `.axi` port of all supplemental cases.
- Server synchronization, real-store initialization, and protected cognition capture remain separate work and are not authorized by the local v1 promotion.
