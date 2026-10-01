# [ETHOS-NOMOS-005] Build unpublished inference-readiness contract candidate artifacts

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Goal

Complete the Nomos-owned, machine-readable contract-candidate artifacts for the approved `inference-readiness-status` boundary without publishing the package or making it runtime-usable.

Parent tracker: `Chris-Cheeseman/cogni-core#649`

## Confirmed baseline

- `The-Ethos-Project/ethos-nomos#2` created the stable Product-to-Cogni-Core package.
- `The-Ethos-Project/ethos-nomos#3` added the first capability request/result schemas and structural fixtures.
- `The-Ethos-Project/ethos-nomos#4` tightened schema identity and stale/current invariants.
- The normative design and conformance authority is the merged Cogni-Core plan through `Chris-Cheeseman/cogni-core#806`.

## Scope

Extend the existing stable package at:

`contracts/product-cogni-core/`

Required artifacts:

1. globally unique JSON Schema identifiers for every package schema;
2. a machine-readable signed discovery-descriptor schema;
3. a machine-readable package-digest-manifest schema;
4. a machine-readable product-ingress audit-event schema;
5. canonical valid and invalid fixtures for those schemas;
6. deterministic tooling that inventories the exact contract package, rejects unsafe paths and links, emits an RFC 8785 canonical digest manifest, and independently verifies the same digest;
7. package metadata and documentation proving the candidate remains `unpublished`, `unreleased`, and `runtime_usable=false`.

The package digest must cover the contract manifest, all referenced schemas, and every fixture while excluding the digest manifest itself. Paths, byte lengths, SHA-256 values, sorting, canonicalization, and rejection behavior must match the Cogni-Core conformance specification.

## Acceptance criteria

- Every schema has a globally unique, stable identifier and all references resolve.
- Valid fixtures pass their declared schemas and invalid fixtures fail them.
- Digest generation is deterministic across repeated clean-checkout runs.
- Verification rejects missing, extra, changed, linked, duplicate-normalized, unsafe, or non-inventoried files.
- The generated manifest contains no timestamps, machine paths, credentials, signatures, or mutable filesystem metadata.
- Package metadata remains `status=unpublished`, compatibility identifiers remain `unreleased`, and `runtime_usable=false`.
- Focused tests and `git diff --check` pass.
- No CI, dependency, listener, port, IPC, MCP, credential, Cogni-Core runtime, or Product client behavior is added.

## Non-authorization

This issue does not authorize package publication or promotion, a runtime listener, credentials, protected Cogni-Core changes, Product client implementation, extraction, CI changes, dependencies, or repository settings changes.
- **Scope:** GitHub Labels: documentation, enhancement | Original Repo: The-Ethos-Project/ethos-nomos

## 2. Agent Assessment
- **Confirmed Repo Facts:** Migrated GitHub State: CLOSED
- **Risk Level:** Medium
- **Unknowns:** N/A (Cloud migration)

## 3. Implementation Plan
- **Proposed Architecture/Changes:** N/A
- **Files Likely to Change:** N/A
- **Required Human Approval Points:** Final axi Ship Sign-off

## 4. Acceptance Criteria
- [x] Cloud issue fully reconciled into axi DAG
- [ ] Local pre-commit checks pass
- [ ] CI/Review gate checks pass
