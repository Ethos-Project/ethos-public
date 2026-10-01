# [COGNI-CORE-046] [Baseline] Complete Cogni-Core technical cutover and legacy-freeze readiness

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Goal

Define and close the evidence required for the organization Cogni-Core repository to become the sole protected cognitive authority before the legacy repository is frozen or archived.

## Current evidence

- Canonical destination: `The-Ethos-Project/cogni-core`.
- Native `Repository baseline` CI is green on the current `main` head.
- The repository is a private P2 authority slice, not yet runtime-cutover evidence.
- Nine protected boundary-refactor paths and seven destination contract-test rewrites remain recorded blockers.
- The destination-native 238-test selection does not prove complete boot, ingress, memory, Product, or cutover readiness.
- Nomos migration and authority handoff remains tracked by `The-Ethos-Project/ethos-nomos#94`.

## Required baseline

- [ ] Pin the destination head SHA and exact runtime/package boundary.
- [ ] Disposition every remaining boundary-refactor path without restoring Product or Nomos imports.
- [ ] Replace unified-layout tests with destination-native contract tests.
- [ ] Prove governed local boot and end-to-end cognition through the canonical pipeline.
- [ ] Prove Product ingress is authenticated, fail-closed, and cannot reach providers directly.
- [ ] Prove identity, governed memory, refusal, and causal-lineage continuity.
- [ ] Record unavailable behavior and rollback/abandon criteria.
- [ ] Obtain consumer evidence from Products, Nomos, and Domus where applicable.
- [ ] Record authority-handoff approval.
- [ ] Freeze/archive the legacy source only through a separately approved final transaction.

## Related issues

- #10 — local functional baseline umbrella
- #12 — ownership separation and synchronization
- #13 — post-baseline cognition demonstration
- #30 — protected inference-readiness endpoint
- #31–#38 — destination diagnostics
- `The-Ethos-Project/ethos-nomos#94` — migration and stale-authority reconciliation

## Non-authorization

This umbrella does not authorize protected runtime edits, contract publication, source deletion, issue closure, repository archival, direct model access, or weakening Kernel/User, ethics, inference, memory, or routing law. Each implementation unit requires a separately approved issue and reviewable PR.
- **Scope:** GitHub Labels: type:baseline, type:migration, risk:protected-core, sync:nomos, status:ready-for-planning | Original Repo: The-Ethos-Project/cogni-core

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
