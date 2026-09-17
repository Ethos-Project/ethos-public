# [ISSUE-002] P1: Define exact Cogni-Core clean-copy manifest and dependency closure

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Program authority

The maintainer directed Codex on 2026-08-02 to continue the Five Strata program until all five official repositories are separated, extracted, destination-validated, and explicitly cut over.

Program umbrella: Chris-Cheeseman/cogni-core#109
Inventory anchor: Chris-Cheeseman/cogni-core#629

## Pinned baseline

- Unified source: Chris-Cheeseman/cogni-core@327d38c6a8533de0d6cb80469548339f135c1d30
- Destination: The-Ethos-Project/cogni-core@82c8d6536aec7bd7b72e96876433446e134b27b5
- Destination is a bootstrap shell with repository-baseline CI.

## Scope

1. Produce the exact tracked-file source-to-destination manifest for protected Cogni-Core code, tests, minimum context, dependency metadata, and repo-local blocking enforcement.
2. Identify every reverse dependency on Product, Domus, Logos, or Nomos-owned paths.
3. Separate reusable governance tooling from Cogni-Core-local enforcement without weakening local gates.
4. Define exact exclusions, protected-surface review evidence, destination checks, rollback, abandon, and cutover criteria.
5. Prepare one copy-first PR only after the exact manifest is reviewable.

## Required invariants

- Cogni-Core remains the sole protected cognitive authority.
- Ethics, kernel/user separation, HMC routing, inference, memory, identity, and causal lineage remain intact.
- No Product, Domus, Logos, or Nomos implementation is copied as hidden Cogni-Core ownership.
- No source deletion or canonical cutover occurs in the copy PR.

## Stop gates

Exact protected-surface changes, destination promotion, canonical cutover, and source cleanup require phase-specific evidence and explicit review.
- **Scope:** GitHub Labels: None

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
