# [ETHOS-NOMOS-015] Define fail-closed Five Strata issue-migration records

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Current goal
Define the fail-closed Five Strata issue-migration record and batch contract in Nomos.

## Scope
- Add JSON Schemas for individual migration records and migration batches.
- Add a Python validation model and focused unit tests.
- Integrate the schemas/model into Nomos boundary validation.
- Preserve the default prohibition on tracked JSON Lines files; later migration-ledger tasks may allow only the exact reviewed ledger paths named in the approved plan.

## Acceptance criteria
- Source repositories are restricted to `Chris-Cheeseman/cogni-core` and `Chris-Cheeseman/ethos-products`.
- Every remaining-work record has exactly one Five Strata owner; mixed owners are rejected and split deliverables use sibling records.
- Source identity, timestamps, state, digest, evidence, scope, acceptance criteria, dependencies, stop gates, and ownership rationale are validated fail-closed.
- Restricted-content metadata requires both a private reference and digest.
- Destination creation and verification state determine closure eligibility.
- Batch uniqueness and destination URL uniqueness are enforced.
- Focused tests are written and observed failing before implementation.
- All native Nomos validation passes.

## Safety boundaries
This issue authorizes Nomos-owned schema and validation work only. It does not authorize source issue migration, destination issue creation, source issue closure, publication, visibility changes, protected Cogni changes, runtime/port/IPC/MCP changes, or release-state changes.

## Planned paths
- `schemas/issue-migration-record.schema.json`
- `schemas/issue-migration-batch.schema.json`
- `validation/issue_migration_model.py`
- `validation/tests/test_issue_migration_model.py`
- `validation/validate_nomos.py`
- `validation/tests/test_validate_nomos.py`

## Stop gate
After native validation succeeds, stop for human approval of the schemas, controlled vocabulary, and restricted-evidence lane before implementing the exporter.
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
