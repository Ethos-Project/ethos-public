# [ETHOS-NOMOS-094] Reconcile issue migration, destination authority, labels, and legacy references

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Objective

Reconcile the Five-Strata issue migration and active issue topology so the five private destination repositories are sufficient to understand current authority, ownership, dependencies, and next work.

## Verified ledger condition

The committed classification ledger contains 516 deliverable rows:

- 193 `complete`;
- 118 `historical`;
- 180 `migrate`;
- 24 `split`;
- 1 `restricted-review`.

All rows currently record destination creation and verification as pending, `status: unpublished`, and `close_eligible: false`. The ledger also contains rows where `destination_owner` disagrees with the repository named in acceptance criteria.

The legacy source redirects to `The-Ethos-Project/cogni-core-0060a606`, which remains private and unarchived. It must be treated as historical evidence pending final freeze/archive approval, not as current destination authority.

## Baseline authorities

- Cogni-Core: `The-Ethos-Project/cogni-core#46`
- Products: `The-Ethos-Project/ethos-products#42`
- Domus: `The-Ethos-Project/domus-ethos#15`
- Logos: `The-Ethos-Project/ethos-logos#64`
- Nomos: #96

## Work phases

### 1. Ledger correction

- [ ] Validate all 516 rows against live source and destination state.
- [ ] Correct owner/acceptance-criteria mismatches.
- [ ] Record destination issue URL/number and immutable verification evidence.
- [ ] Distinguish migrated, split, historical, completed, restricted, and destination-native issues.
- [ ] Keep unresolved security/restricted rows fail-closed.

### 2. Label and ownership normalization

- [ ] Apply the shared `type:*`, `risk:*`, `sync:nomos`, `sync:logos`, status, and concept labels.
- [ ] Remove incorrect default-label meanings only through reviewed batches.
- [ ] Record exactly one primary repository owner for each active issue.
- [ ] Use synchronization labels for cross-repository visibility rather than duplicate authority.

### 3. Active-reference repair

- [ ] Repair #21 and #83.
- [ ] Repair active baseline, Product naming, S.O.C.R.A.T.E.S., workflow, dependency, and ownership links.
- [ ] Preserve source IDs under explicit historical-lineage headings.
- [ ] Remove claims that the destination repositories do not yet exist.

### 4. Verification and legacy freeze readiness

- [ ] Prove active roadmaps require no legacy issue topology.
- [ ] Verify all five repository baselines.
- [ ] Record consumer pins and rollback evidence.
- [ ] Prepare—but do not execute—the final legacy freeze/archive transaction.

## Rules

- No blind global replacement.
- No issue closure, transfer, or deletion during reconciliation.
- No historical evidence erasure.
- No source/code/data movement.
- No repository archival without separate explicit approval.
- Migration evidence does not prove technical cutover or public-release readiness.

## Completion criteria

A new contributor or agent can start from the five private repositories and their current issues without consulting the legacy issue topology for active authority, and every migration ledger row has an evidence-backed disposition.
- **Scope:** GitHub Labels: type:governance, type:migration, risk:high | Original Repo: The-Ethos-Project/ethos-nomos

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
