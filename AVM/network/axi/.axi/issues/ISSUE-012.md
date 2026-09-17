# [ISSUE-012] Reconcile Cogni-Core ownership, legacy quarantine, and Five-Strata synchronization

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Goal

Ensure the destination Cogni-Core repository contains only protected cognition authority, required local enforcement, and narrowly reviewed contracts while all other material has an evidence-backed owner and lifecycle state.

## Current Five-Strata ownership

- Cogni-Core — protected cognition, ethics, identity, memory, inference, routing, and local enforcement.
- Products — ErosOS, Eros chat, NatLP Console, Kradle design, deployable Product surfaces and adapters.
- Domus — simulation, embodiment, sanctuary, environments, and experimental traces.
- Logos — research, interpretation, product discovery, UX, and suggestive insights.
- Nomos — governance, standards, workflows, contracts, ownership, automation authority, and coordination.

## Current scope

- inventory remaining mixed, legacy, generated, historical, and cross-stratum artifacts;
- classify ownership, runtime reachability, tests, dependencies, evidence value, and lifecycle;
- preserve protected local enforcement;
- quarantine historical evidence so it cannot appear as active canon;
- identify destination-native contract and test replacements;
- synchronize active names, links, READMEs, indexes, ownership, tests, and workflows;
- prepare bounded child issues before any destructive or protected change.

## Required classification

- keep as protected Cogni-Core authority;
- keep as Cogni-local enforcement;
- Products-owned;
- Domus-owned;
- Logos-owned;
- Nomos-owned;
- historical/quarantined evidence;
- generated/rebuildable;
- superseded;
- deletion candidate requiring proof;
- founder decision required.

## Current dependencies

- #46 — technical cutover baseline
- `The-Ethos-Project/ethos-nomos#31` — ecosystem/cutover doctrine
- `The-Ethos-Project/ethos-nomos#89` — cross-repository authority contract
- `The-Ethos-Project/ethos-nomos#94` — issue and legacy-reference reconciliation
- `The-Ethos-Project/ethos-nomos#96` — operational handoff

## Safety gates

Stop before deleting, moving, rewriting, or declassifying anything with unresolved runtime, test, security, identity, IP, research, historical, or protected-authority value. No broad destructive PR may combine inventory, movement, protected refactor, test rewrite, and cleanup.

## Non-authorization

This issue does not itself authorize source movement, deletion, protected runtime changes, workflow/settings changes, issue transfer or closure, public release, or legacy repository archival.
- **Scope:** GitHub Labels: type:migration, risk:high, sync:nomos

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
