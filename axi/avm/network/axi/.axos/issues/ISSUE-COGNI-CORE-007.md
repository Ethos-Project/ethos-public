# [COGNI-CORE-007] P3: Mirror reviewed readiness ingress slice

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Goal

Mirror the reviewed dormant readiness ingress slice from `Chris-Cheeseman/cogni-core#824` into the private organization Cogni-Core bootstrap without activating ingress or declaring cutover.

## Source authority

- source merge: `Chris-Cheeseman/cogni-core@92d8d9a690cd74d3bcb14f1a16d17a24023f36d8`
- source issue: `Chris-Cheeseman/cogni-core#823`
- source PR: `Chris-Cheeseman/cogni-core#824`

## Exact scope

- mirror the three reviewed dormant ingress runtime files and three associated test files
- update destination-only clean-copy provenance, counts, and extraction notes
- keep the repository private and non-canonical
- keep nine boundary-refactor files and seven contract-rewrite tests absent

## Exclusions

- no boot wiring or provider invocation
- no MCP host change
- no Nomos capability publication or runtime promotion
- no public source/package release
- no canonical cutover
- no unified-source deletion

## Acceptance gates

- all six mirrored Git blobs equal the reviewed source merge
- 274 manifest-authorized blobs verify and 16 blockers remain absent
- destination-native grouped CI is green
- every actionable GraphQL review thread is resolved or outdated by a code-backed fix
- **Scope:** GitHub Labels: None | Original Repo: The-Ethos-Project/cogni-core

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
