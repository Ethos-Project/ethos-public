# [ETHOS-NOMOS-011] P2: Validate private Nomos separation without authority handoff

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Goal

Complete the private Nomos separation-validation phase without copying any unified-source record whose authority, security, template, connector, process, evidence, or license disposition remains blocked.

## Confirmed scope

The P1 manifest authorizes zero unified-source copies. The official destination already contains all 58 personal-staging paths/blobs plus the destination CI workflow.

In scope:

- preserve the existing destination-owned tools and unpublished Product-to-Cogni contract
- replace bootstrap MIT ambiguity with an interim private-development/no-public-release notice
- add destination-native validation for manifest accounting, authority precedence, contract `status: unpublished`, `runtime_usable: false`, compatibility `unreleased`, protected connector absence, secret patterns, generated/local state, and source-copy exclusion
- extend the existing single `nomos-boundary` job with named validation/test/compile steps without adding another workflow or dependency setup
- record the result as private separation validation, not governance handoff or contract publication

Explicit exclusions:

- all 173 authority-handoff records
- all evidence, template-generalization, reusable-candidate, connector, source-operation, process-boundary, and source-license rows
- the 12 Cogni-local enforcement files
- contract publication, `runtime_usable: true`, governance cutover, public release, source cleanup, or staging deletion

## Acceptance gates

- manifest counts and all 310 source dispositions are accounted for
- no source row with a blocked/retained/excluded disposition is newly imported by this PR
- all existing contract digest tests pass
- contract state remains unpublished, unreleased, and runtime-unusable
- PowerShell parsing, Python validator tests, secret/generated-state checks, and `git diff --check` pass in one grouped job
- all actionable GraphQL review threads are resolved

## Rollback

Close or revert the validation PR. A future authority handoff requires a new explicit decision and consumer pin/rollback plan.
- **Scope:** GitHub Labels: None | Original Repo: The-Ethos-Project/ethos-nomos

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
