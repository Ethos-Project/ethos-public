# [ETHOS-NOMOS-030] Audit the unified checkout against the final Five-Strata model

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Goal

Maintain the repo-grounded, file-level ownership inventory for the final Five-Strata model so extraction can proceed safely without moving code, changing runtime behavior, or weakening Eros Cogni-Core authority.

## Current status

- Active unified source: `Chris-Cheeseman/cogni-core`
- Active local checkout: `C:\Chris-Cheeseman\cogni-core`
- Canonical base: `main`
- PR The-Ethos-Project/cogni-core-0060a606#628 was originally merged to `dev`; its Five-Strata content was reconciled into current `main` through PR The-Ethos-Project/cogni-core-0060a606#405.
- Issues The-Ethos-Project/cogni-core-0060a606#661-#666 are complete.
- The canonical path inventory, mixed-boundary contract, clean-copy rule, test-ownership plan, and rollback checklist now exist on `main`.
- This issue remains open as the inventory and ordered extraction-backlog anchor.

## Canonical strata

| Stratum | Authority | Canonical repository |
|---|---|---|
| 1 | Eros Cogni-Core | `cogni-core` |
| 2 | The Ethos Suite | `ethos-products` |
| 3 | Eros Genome Initiative / Logos | `ethos-logos` |
| 4 | Nomos | `ethos-nomos` |
| 5 | Domus Ethos | `domus-ethos` |

`ethos-archive` is an evidence repository, not a sixth active stratum.

## Current downstream anchors

- Products readiness and first extraction manifest: The-Ethos-Project/ethos-products#23
- Nomos tooling/governance readiness: The-Ethos-Project/ethos-nomos#29
- Domus Ethos readiness: The-Ethos-Project/domus-ethos#10
- Logos classification/disclosure readiness: The-Ethos-Project/ethos-logos#14
- Program umbrella: The-Ethos-Project/ethos-nomos#25

## Remaining inventory work

1. Turn the canonical path-family inventory into exact source-SHA and file manifests for each approved extraction phase.
2. Record destination mappings, explicit exclusions, companion tests/docs, and ownership for every copied path.
3. Complete import/caller and process/data ownership evidence for protected-adjacent boundaries.
4. Keep shared-contract candidates narrow and prevent a sixth authority layer.
5. Record stop gates, rollback evidence, and source-cleanup gates for each phase.

The first manifest should cover Products under The-Ethos-Project/ethos-products#23 using both the unified source repository and the product staging repository, with `The-Ethos-Project/ethos-products` as the destination.

## Constraints

- No file movement or source deletion in this issue.
- No runtime, test, CI, import, package, database, or protected-architecture changes.
- Preserve historical names when they identify real issue, PR, branch, commit, path, or audit evidence.
- Do not treat organization bootstrap shells as canonical before approved cutover.
- Mark unresolved ownership rather than guessing.

## Done when

- Every active architecture-relevant root has an accepted owner or explicit unresolved disposition.
- Each extraction phase has an exact traceable source/destination manifest.
- Protected authority and contract-test ownership remain explicit.
- The ordered backlog is reversible and approval-gated.

Current reconciliation PR: The-Ethos-Project/cogni-core-0060a606#799.
- **Scope:** GitHub Labels: documentation | Original Repo: The-Ethos-Project/ethos-nomos

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
