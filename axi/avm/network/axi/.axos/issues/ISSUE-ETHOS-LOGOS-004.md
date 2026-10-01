# [ETHOS-LOGOS-004] P2: Copy the private Logos research corpus

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Goal

Privately copy the manifest-authorized Logos corpus into the official repository while preserving disclosure classifications and Cogni-Core/Nomos authority.

## Authority and scope

Authorized under `Chris-Cheeseman/cogni-core#109` and destination P1 issue #2.

In scope:

- copy all 18 manifest rows whose disposition begins with `copy-`
- preserve public-safe-candidate, private-only, private-research, and private-historical classifications inside the still-private destination
- retain historical non-authority labels and validate relative links
- add a grouped authority/disclosure/link/claim/secret validator to the existing single baseline job
- replace bootstrap MIT ambiguity with an interim private-development/no-public-release notice
- verify byte-identical copies against the pinned source blobs

Explicit exclusions:

- `docs/historical/credentials_setup_legacy.md`
- Cogni authority roots (`EROS.md`, `ETHICS.md`, `PHILOSOPHY.md`)
- public visibility, paper/dataset release, patent disclosure, source cleanup, or canonical cutover

## Acceptance gates

- 18/18 copied blobs match the manifest
- private/internal/historical content remains only in approved paths
- historical files retain non-authority treatment
- local relative links resolve or are explicitly identified for destination-authored repair
- secret-pattern scan passes without copying the blocked credential procedure
- grouped CI and all actionable GraphQL review threads are green/resolved

## Rollback

Before any separate cutover, close or revert the implementation PR. Publication remains a per-artifact approval gate.
- **Scope:** GitHub Labels: None | Original Repo: The-Ethos-Project/ethos-logos

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
