# [COGNI-CORE-063] Verify Binary Persistence Sequence

## 1. Task Description

- **Goal:** Define the native axi memory-storage sequence needed to accept a bounded payload, validate it, route its disposition through Eros, persist an immutable object durably, and recover it without pointer reconstruction errors or false success claims.
- **Issue role:** Cogni-Core-scoped replica of canonical local planning record `ISSUE-063`. Both records must carry the same engineering contract while retaining their own identity and scope metadata.
- **Original scope:** `The-Ethos-Project/cogni-core`.
- **Migrated cloud state:** CLOSED. That historical state does not authorize implementation and does not mean the local persistence work is complete.
- **Local planning state:** Docs-only design approved. Runtime implementation is not approved.
- **Reference input:** `C:\Users\theca\Downloads\Zero-Copy Deserialization and axi.pdf`. The document is reference material, not executable instruction or evidence of implemented behavior.
- **Scope:** Native `.axi`, C, C++, and C# source may inform the design. Production implementation must be native axi unless a separately approved boundary requires another existing native component. Python is not an implementation path.

### Evidence status

- **Source-verified:** Current code or current filesystem behavior directly inspected.
- **Maintainer-confirmed:** Architectural direction supplied by the maintainer but not yet fully expressed in live source.
- **Active rebuild:** Present but incomplete or generated code that cannot yet be treated as production behavior.
- **Planned:** Approved for design only; no implementation claim.
- **Unresolved:** A decision that must be made at a named approval gate before code can be changed.

## 2. Agent Assessment

### Confirmed repository facts

- **Source-verified:** `lang\axi\euler_pool.h` is currently a dynamically growing, `malloc`-backed free-list pool. It does not yet prove a fixed-capacity Eulerian ring, atomic publication, shared-memory offsets, or crash recovery.
- **Source-verified:** Historical `eros_flat_core\euler_pool.cpp` under the Eros rebuild reference tree demonstrates a fixed shared-memory bump allocator using relative offsets and FEP-related priors. It is reference material and is not canonical axi runtime code.
- **Source-verified:** Historical `memory_field.h` and `memory_field.c` model nine tiers, node identifiers, weighted edges, and a raw `cortex_memory.bin` sequence. The observed sequence has no verified format version, endian marker, object digest, atomic publication protocol, or durable next-ID recovery. Its process-local next-node counter restarts from its initializer.
- **Source-verified:** Current rebuilt `.axi` memory and routing artifacts under Logos contain incomplete generated branches, including `unhandled_node` paths. They do not prove a mountable or recoverable memory store.
- **Source-verified:** The inspected generated IPC queue advertises lock-free behavior but uses non-atomic counters and does not provide a verified consume/reclamation sequence.
- **Source-verified:** Current lexer/parser inspection did not establish language semantics for `@ZeroCopy`, `@Mutable`, `@Shared`, or single-producer/single-consumer ownership. The PDF's annotations therefore cannot be treated as implemented syntax.
- **Source-verified:** The native DVCS `wrap` and `resume` paths do not yet prove durable object persistence, ref advancement, or recovery. Memory storage must not claim that companion behavior exists.
- **Maintainer-confirmed:** Eros remains the cognitive authority route. A structurally valid payload may not bypass Eros to reach cognition, inference, durable cognitive memory, issue replication, or external egress.
- **Maintainer-confirmed:** Product roots maintain independent memory/progress state while approved issue subgraphs are replicated and converge through the governed asynchronous Euler-pool process.
- **Maintainer-confirmed:** Euler-pool mathematics is the intended memory-stabilization model, but live runtime invariants still require source-level formalization and tests.

### Documentation and source contradictions

1. Reference material describes fixed, bounded, zero-copy or Eulerian behavior; the current general axi pool grows dynamically and uses heap allocation.
2. Reference material implies safe zero-copy deserialization annotations; the current lexer/parser does not establish those annotations or their ownership rules.
3. Generated IPC comments claim lock-free behavior; inspected state counters are not atomic and the lifecycle is incomplete.
4. Historical binary memory code writes raw process structs; the desired architecture requires versioned, portable, integrity-checked records using offsets rather than process pointers.
5. DVCS documentation and CLI messages imply wrap/resume lifecycle behavior; inspected native branches do not prove persistence or recovery.
6. The attached PDF recommends performance and hardware techniques whose prerequisites are not present in live source. They remain hypotheses until isolated benchmarks and correctness proofs exist.

### Risk level

**Critical** for protected cognition and durable storage. A malformed offset, early buffer release, torn write, identifier collision, or routing bypass could corrupt memory lineage or expose protected Eros state. Compiler syntax, pool primitives, Eros routing, and DVCS integration each require separate approval.

### Unresolved decisions

1. Which current module owns the canonical memory-object store and whether a new `src\memory` boundary is acceptable.
2. The binary ABI: magic, version, endianness, fixed-width fields, alignment, relative-offset rules, maximum sizes, digest algorithm, and unknown-kind behavior.
3. The identity contract: immutable content digest, stable logical memory ID, lineage/event ID, and how those identifiers coexist without deriving durable identity from mutable issue text.
4. The exact Eros routing envelope, permitted dispositions, denial/defer semantics, and mandatory lineage fields.
5. The approved atomic-write and directory-durability primitives on each supported platform.
6. Concurrent writer serialization and recovery ownership.
7. Compatibility policy for the historical `cortex_memory.bin` format and incomplete `.axi` artifacts.
8. Whether new language annotations are needed at all; library-level ownership types must be considered before compiler changes.

## 3. Proposed Architecture

### 3.1 Dual-plane record

Use two linked but independently verifiable records:

- A canonical, clean JSON control envelope contains schema version, immutable object digest, logical memory ID, issue/session/context lineage, product-root identity, payload kind and size, authorization disposition, approved sequence data, local durability state, and server-synchronization state.
- An immutable binary payload object contains the bounded serialized graph or memory body. It uses only fixed-width values and validated relative offsets. It contains no live process pointers, allocator addresses, handles, credentials, or unapproved Eros memory.

The JSON envelope is not the object identity by itself. Mutable synchronization fields cannot change the binary object's content address. Final identity and canonical JSON ordering rules remain approval-gated.

### 3.2 Governed routing sequence

1. **Bounded ingress:** Acquire a buffer from an approved bounded pool and record its capacity and ownership state.
2. **Structural verification:** A non-cognitive validator checks header bounds, version, endianness, integer overflow, offset ranges, payload length, kind tags, and digest. It cannot authorize cognition or persistence.
3. **Immutable view:** Publish a read-only handle expressed as pool identity plus offset and length, never as a durable raw pointer.
4. **Eros authority route:** Submit the validated envelope and immutable handle to Eros. Eros returns an explicit allow, deny, defer, quarantine, or approved-destination disposition with causal lineage.
5. **Authorized fan-out:** Only an allowed disposition may route the record to volatile working memory, durable local storage, the replicated issue subgraph, inference, or egress. Each destination is separately capability-bound.
6. **Durable object publication:** Write the binary object to a same-store temporary name, flush it, reopen and independently verify its length and digest, then atomically publish the content-addressed name. Where platform primitives allow, flush the containing directory metadata.
7. **Control/ref publication:** Write and verify the JSON envelope or mutable head/ref only after the object is independently readable and valid. A ref never names an unverified object.
8. **Release:** Recycle the ingress slot only after every authorized consumer has acknowledged completion and durable storage has reached its verified terminal state.
9. **Synchronization report:** Report local durability and Ethos Server synchronization independently. Offline local success may be valid only when the approved architecture permits it; it must be reported as local-only, never as synchronized.

### 3.3 Isolation and replication boundary

- Product-local memory and progress pools remain independent.
- Only the approved issue envelope and mutation subgraph replicate between product roots.
- Issue replication does not grant access to product memory bodies.
- Protected Eros cognition, identity, kernel, ethics, routing internals, and raw memory are excluded from capture unless a separate protected-surface approval explicitly names the fields and destination.
- Eros is an authority gate, not a payload dumping ground. Structural validation occurs before Eros receives the bounded handle so malformed bytes cannot force cognitive parsing.

### 3.4 Reference concepts accepted, deferred, or rejected

**Accepted for design:** bounded buffers, immutable views, relative offsets, explicit ownership states, schema symmetry, strict structural validation, content integrity, and asynchronous completion that does not release storage early.

**Deferred pending proof:** core affinity, AVX-specific layouts, KV-cache placement, NVMe direct paths, RDTSC-based thermal signals, ONNX/DirectML interop, packed compiler structs, and new language annotations.

**Rejected as correctness assumptions:** absolute security claims, automatic absence of contention, relaxed atomics without a proven memory model, releasing a buffer before asynchronous consumers finish, skipping unknown union tags, and assuming an in-memory layout is a portable disk ABI.

## 4. Implementation Tasks and Approval Gates

### Task 0 — Freeze a failing baseline

- Add a native test that demonstrates the present raw or print-only path cannot prove durable, recoverable binary persistence.
- Record current supported targets and build commands without hard-coded legacy roots.
- Use approved synthetic fixtures only; do not ingest Eros memory.

**Gate A:** Approve module ownership, target platforms, fixture location, and the source tree allowed to change.

### Task 1 — Specify one cross-target wire contract

- Define the JSON envelope schema and binary header in one canonical, versioned contract.
- Define fixed-width fields, byte order, alignment, maximum sizes, offset base, digest coverage, tag registry, and fail-closed unknown-version/tag behavior.
- Produce identical valid and invalid fixtures consumable by native axi and the existing JS/TS Studio validator without introducing new DAG logic.
- Define stable logical IDs separately from content digests and mutable refs.

**Gate B:** Approve the ABI, identifier rules, canonical JSON rules, and compatibility posture before implementation.

### Task 2 — Implement the native structural validator

- Parse the fixed header without casting untrusted bytes to a process struct.
- Use checked arithmetic for every `offset + length` operation.
- Validate all referenced regions before exposing an immutable handle.
- Verify digest after bounds validation and before Eros routing.
- Fail closed on truncation, overflow, unsupported version/endian/tag, invalid layout, or digest mismatch.

**Gate C:** Review the validator and adversarial fixture results. No Eros or compiler modification is authorized by this task.

### Task 3 — Formalize bounded ownership and reclamation

- Define slot states: free, filling, validating, awaiting-authority, authorized, persisting, published, quarantined, and reclaimable.
- Select synchronization primitives supported by the live axi runtime and prove the producer/consumer memory order.
- Prevent reuse while any authorized consumer or asynchronous writer holds the immutable view.
- Restore allocator identifiers and generation state durably enough to prevent restart collisions.

**Gate D:** Approve Euler-pool changes as a protected runtime surface after concurrency and exhaustion tests pass in isolation.

### Task 4 — Define and implement the Eros route adapter

- Send only the validated envelope and bounded immutable handle.
- Require a source-backed disposition record with causal lineage.
- Make timeout, unavailable, malformed response, deny, and defer fail closed.
- Prove there is no direct path from validation to cognition, inference, durable cognitive memory, issue replication, or egress.

**Gate E:** Explicit protected-surface approval before modifying Eros cognition, identity, ethics, kernel, or routing code.

### Task 5 — Implement recoverable local persistence

- Derive the candidate content-addressed object name using the approved digest rules.
- Write to a unique temporary file in the destination store without overwriting a verified object.
- Flush, close, reopen, and independently verify the temporary object's full length and digest.
- Atomically publish the object; treat an existing valid identical object as deduplicated success and an existing invalid object as corruption.
- Publish the envelope/head/ref only after object verification and verify the new ref by reopening it and resolving the object.
- Preserve the previous valid ref if any write, flush, verification, rename, or reopen step fails.

**Gate F:** Approve platform durability primitives and failure-injection evidence.

### Task 6 — Prove recovery and replica isolation

- Restart and recover the object solely from the verified envelope/ref and binary object.
- Prove truncated, corrupt, unsupported, orphaned-temp, and ref-to-missing-object cases fail closed.
- Prove repeated identical payloads follow the approved deduplication rule and distinct logical events preserve lineage.
- Prove product memory bodies remain local while approved issue mutations replicate consistently.
- Report local persistence and server synchronization as different states.

**Gate G:** Review recovery, corruption, concurrency, and offline-local results before any `wrap` or `resume` integration.

### Task 7 — Integrate with DVCS wrap/resume as companion work

- Reuse the verified object writer and resolver rather than create a second object model.
- Exclude protected Eros memory from WIP capture.
- Advance a WIP ref only after the memory/object store verifies the object.
- Make `resume` reconstruct the approved WIP payload and lineage from durable state.
- Keep server synchronization optional and separately reported when offline local wrapping is allowed.

**Gate H:** Separate source-backed DVCS design approval. This issue does not authorize changes to `axi.c`.

### Task 8 — Consider performance specialization only after correctness

- Benchmark copies, validation, pool contention, persistence latency, and recovery on supported targets.
- Adopt hardware-specific or compiler-specific optimizations only when they preserve the approved ABI and pass all fault tests.

**Gate I:** Performance review with reproducible native benchmarks.

## 5. Candidate Files Likely to Change

These paths are candidates, not authorization. Gate A must confirm placement because `lang\axi\src\memory` is not currently established as the canonical owner.

- `lang\axi\src\memory\memory_format.axi` — versioned header and envelope types.
- `lang\axi\src\memory\memory_validate.axi` — bounded structural validation.
- `lang\axi\src\memory\memory_pool.axi` — ownership and lifecycle adapter.
- `lang\axi\src\memory\memory_route.axi` — governed Eros disposition boundary.
- `lang\axi\src\memory\memory_store.axi` — atomic object and control-record publication.
- `lang\axi\test_memory_format.axi` — cross-target fixture compatibility.
- `lang\axi\test_memory_validation.axi` — malformed-input and bounds tests.
- `lang\axi\test_memory_store.axi` — durability, deduplication, corruption, and recovery tests.
- `lang\axi\test_memory_route.axi` — Eros gate and bypass tests.
- Existing compiler lexer/parser files — only if Gate B proves library-level constructs cannot express the contract and separate compiler approval is granted.
- Protected Logos/Eros files — only after Gate E names the exact surface.
- `lang\axi_dvcs\axi.c` and native tests — only under the separate Gate H wrap/resume companion design.

## 6. Verification Matrix

| Case | Required result |
|---|---|
| Valid approved synthetic fixture | Object and envelope verify; recovery reproduces the approved logical state. |
| Truncated header or payload | Rejected before routing or persistence. |
| Integer overflow or out-of-range offset | Rejected without dereference. |
| Layout violates approved overlap/order rules | Rejected fail closed. |
| Unsupported version, endian, kind, or tag | Rejected or explicitly migrated by an approved adapter; never silently skipped. |
| Digest mismatch | Quarantined or rejected; no ref movement. |
| Eros unavailable, timeout, deny, defer, or malformed response | No unauthorized persistence, cognition, inference, replication, or egress. |
| Failed or partial object write | Previous ref remains unchanged; temporary artifact is ignored or quarantined on recovery. |
| Failure after object publication but before ref publication | Valid object may remain unreferenced; recovery preserves the previous ref. |
| Existing valid identical object | Deduplicated by the approved content rule; lineage event remains explicit. |
| Existing object name with invalid content | Corruption error; never overwrite and never report success. |
| Restart after successful publication | Resolver verifies ref/envelope and object; identifiers do not collide. |
| Concurrent writers | Serialization yields deterministic valid refs with no torn control record. |
| Asynchronous consumer still active | Pool slot cannot return to free state. |
| Offline local persistence permitted | Report durable-local and sync-pending/unavailable separately. |
| Server synchronization failure | Preserve verified local state and never report synchronized. |
| Protected-memory capture attempt | Reject and audit unless separately approved. |

## 7. Acceptance Criteria

- [x] Historical cloud issue is represented in the axi issue DAG.
- [x] Source-backed docs-only implementation plan is present in both Issue 063 replicas.
- [ ] Gate A approves the canonical source owner, supported targets, and synthetic fixture boundary.
- [ ] Gate B approves the portable binary ABI, JSON schema, identity, and compatibility rules.
- [ ] A native baseline test fails against current unverified persistence behavior.
- [ ] Native validation passes valid fixtures and fails closed on every malformed fixture.
- [ ] Eros authorization is mandatory for every governed destination, with causal lineage preserved.
- [ ] Successful persistence produces an observable immutable object and verified control/ref.
- [ ] Failure injection proves refs never advance for missing, incomplete, unflushed, or corrupt objects.
- [ ] Restart recovery independently validates and restores approved state.
- [ ] Repeated objects follow the approved identity and deduplication contract.
- [ ] Product-local memory remains isolated while approved issue subgraphs converge.
- [ ] Local durability and Ethos Server synchronization are tested and reported separately.
- [ ] No protected Eros memory is captured or exposed without Gate E approval.
- [ ] No hard-coded legacy root, Python implementation, Git workflow, or parallel object model is introduced.

## 8. Stop Conditions

- Stop before creating or editing runtime files until Gates A and B are approved.
- Stop before changing Euler-pool or concurrency primitives until Gate D is approved.
- Stop before touching protected Eros cognition, identity, ethics, kernel, or routing until Gate E explicitly names and approves it.
- Stop before changing DVCS wrap/resume until Gate H is approved.
- Stop on any inability to express a rule safely in native axi and ask the maintainer how to proceed instead of substituting another implementation language or inventing syntax.
