# [ETHOS-NOMOS-017] Build complete read-only Five Strata issue exporter

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Current goal
Build the complete read-only remote issue exporter for the Five Strata cutover.

## Live remote baseline
Refreshed 2026-08-05 through GitHub GraphQL:

- `Chris-Cheeseman/cogni-core`: 499 issues — 189 open, 310 closed.
- `Chris-Cheeseman/ethos-products`: 3 issues — 2 open, 1 closed.
- Total: 502 issues — 191 open, 311 closed.
- The five active `The-Ethos-Project` destination repositories currently have zero open issues.

Counts are evidence only; exporter pagination must discover totals live and must not use these numbers as loop bounds.

## Remote-authority contract
GitHub issues are the task authority. Source issues remain authoritative provenance until later reviewed transactions create and verify their one-stratum replacements in the correct private `The-Ethos-Project` repositories. No local directory, worktree, raw export, or cache becomes a migration destination.

Raw API envelopes may exist only in disposable staging outside every Git repository. After independent count/digest verification, that staging must be removed. Only sanitized manifests and digests may be committed to Nomos.

## Exact changed paths

- Create `tools/issue_migration_common.py`
- Create `tools/export_issue_migration.py`
- Create `migrations/issues/five-strata-cutover/README.md`
- Create `migrations/issues/five-strata-cutover/sources.json`
- Modify `tools/README.md`
- Modify `validation/tests/test_issue_migration_model.py`

## Acceptance criteria

- Fake-transport tests cover cursor pagination, partial-page rejection, and preservation of issue body, author, timestamps, state/state reason, comments, timeline, assignees, labels, milestones, parent/sub-issues, linked PRs, cross-references, project items, task-list text, reactions, attachment URLs, and edit-history availability.
- PR-shaped issue API nodes are excluded and reported separately.
- GitHub subprocess calls use argument arrays without `shell=True` and allow only read endpoints.
- Errors never print sensitive bodies, comments, attachments, tokens, or raw API payloads.
- Source repositories are exactly `Chris-Cheeseman/cogni-core` and `Chris-Cheeseman/ethos-products`.
- Output paths inside any Git worktree are rejected.
- Each remote issue receives stable node/URL/`updatedAt` identity and a canonical digest; the sanitized manifest records repository identity, counts, highest `updatedAt`, per-record digests, and aggregate digest.
- Resume succeeds only when existing staged record digests validate.
- First remote export totals are independently verified against REST/GraphQL and first/middle/last pages are sampled without logging content.
- Disposable raw staging is removed after verification.
- All native Nomos checks pass.

## Safety boundaries
This issue authorizes read-only GitHub export tooling and ephemeral local staging only. It does not authorize classification, JSONL ledger exceptions, destination issue creation, source issue closure, transfer, project mutation, publication, visibility changes, release promotion, CI/dependency changes, or any protected/runtime/credential surface.

## Stop gate
Stop after review of remote totals, unavailable fields, sensitive-record count, and aggregate digest. Do not classify or create destination issues in this task.
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
