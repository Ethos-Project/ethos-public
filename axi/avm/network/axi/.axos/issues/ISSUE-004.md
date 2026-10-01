# [ISSUE-004] P2: Copy the private proprietary Cogni-Core slice

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Goal

Privately copy the manifest-authorized Cogni-Core authority slice into the official proprietary repository without modifying protected behavior or importing cross-stratum blockers.

## Authority and scope

Authorized under `Chris-Cheeseman/cogni-core#109`, destination P1 issue #2, and the maintainer's proprietary/no-public-source decision.

In scope:

- copy 254 `copy`, 14 `copy-local-enforcement`, and five `copy-context` records unchanged from `Chris-Cheeseman/cogni-core@327d38c6a8533de0d6cb80469548339f135c1d30`
- author minimal Cogni-only private packaging/dependency metadata
- replace bootstrap MIT ambiguity with an interim proprietary private-development notice
- extend the existing single baseline job with provenance, forbidden-path, Ruff, brain-owned test, Architecture Guard, LLM Bridge Guard, Brain Law, and analyzer steps supported by the copied slice
- preserve all local protected enforcement

Explicit exclusions:

- all nine `blocked-boundary-refactor` files
- all seven `blocked-contract-rewrite` tests
- Product, Domus, Logos, reusable Nomos, credential, archive, generated-analysis, and source-cleanup paths
- protected behavior edits, public source/package release, canonical cutover, or unified-source deletion

## Acceptance gates

- 273/273 copied destination blobs match the manifest
- every blocked path remains absent
- minimal dependency closure is documented and reproducible
- grouped CI passes the supported protected checks without weakening any guard
- no actionable GraphQL review threads remain

## Rollback

Before cutover, close or revert the implementation PR. Canonical cutover, blocked-adapter work, source cleanup, and any external packaging remain separate approvals.
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
