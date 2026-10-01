# [COGNI-CORE-020] #582C: Decision-only archive placement and retention policy for historical evidence

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
Parent: The-Ethos-Project/cogni-core#12
Ledger: docs/governance/repository_cleanup_classification_ledger_2026-07-24.md

## Purpose

Decide the archive destination and retention policy for the seven artifacts classified as `HISTORICAL_EVIDENCE` in the baseline ledger.

## Decision Required

- confirm whether each artifact remains in `cogni-core` as preserved evidence or is eligible for later archive placement in `ethos-archive`;
- define the retention rule for each artifact;
- record the governing rationale and any required preservation conditions.

## Non-Scope

- no file movement in this issue;
- no deletion in this issue;
- no ownership migration in this issue;
- no movement allowlist in this issue;
- no planner rewrite in this issue.

## Notes

`ethos-archive` is the destination repository, but this issue only decides whether a given artifact should be retained locally as evidence or later placed there under a separate movement issue.
- **Scope:** GitHub Labels: None | Original Repo: The-Ethos-Project/cogni-core

## 2. Agent Assessment
- **Confirmed Repo Facts:** Migrated GitHub State: OPEN
- **Risk Level:** Medium
- **Unknowns:** N/A (Cloud migration)

## 3. Implementation Plan
- **Proposed Architecture/Changes:** N/A
- **Files Likely to Change:** N/A
- **Required Human Approval Points:** Final axi Ship Sign-off

## 4. Acceptance Criteria
- [ ] Cloud issue fully reconciled into axi DAG
- [ ] Local pre-commit checks pass
- [ ] CI/Review gate checks pass
