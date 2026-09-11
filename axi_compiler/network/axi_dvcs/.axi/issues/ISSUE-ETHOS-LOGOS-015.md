# [ETHOS-LOGOS-015] [S.O.C.R.A.T.E.S./Logos] Define milestone UX and dashboard research contract

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Goal

Define the Logos-owned milestone UX, dashboard research, visualization semantics, and human-interpretation requirements for S.O.C.R.A.T.E.S.

## Current authority

- Logos owns research, UX, product definition, and evidence-bounded suggestive-insight design.
- Nomos issue `The-Ethos-Project/ethos-nomos#33` owns operational authority, schemas, permissions, audit, and authoritative state transitions.
- Logos issue #65 owns the broader administrative-platform research and suggestive-insight boundary.
- Products may implement deployable dashboard UI only through a separately approved Product issue.
- Cogni-Core remains protected cognition and is not a dashboard or administrative authority.

## Research and UX scope

Define:

- milestone, dependency, evidence, risk, confidence, freshness, and uncertainty presentation;
- readonly-first Gantt, table, card, dependency-graph, and status-summary views;
- accessible fallback when visualization dependencies fail;
- source-of-truth links and stale-data disclosure;
- founder/admin review flows;
- suggestive insights with evidence, counterevidence, uncertainty, and explicit non-authority;
- privacy and access-classification behavior;
- renderer-neutral requirements handed to Nomos and Products.

## Consistency requirements

- Every view uses Nomos canonical identifiers and status values.
- A forecast is never presented as a commitment.
- Progress is derived from evidence, not arbitrary percentages.
- An insight cannot mutate a milestone, schedule, approval, issue, PR, or repository state.
- Invalid, stale, incomplete, or unavailable governance data fails closed to a clearly labeled readonly state.
- Gantt is a renderer, not the source of record.

## Related issues

- #65 — admin-platform research, UX, and suggestive insights
- `The-Ethos-Project/ethos-nomos#33` — operational authority and platform contract
- `The-Ethos-Project/ethos-nomos#70` — visualization dependency planning
- `The-Ethos-Project/ethos-products#30` — possible Product renderer after approval

## Deliverables

- [ ] Milestone/dashboard UX brief.
- [ ] Visualization and accessible-fallback requirements.
- [ ] Insight evidence/uncertainty model.
- [ ] Privacy/access presentation matrix.
- [ ] Nomos handoff requirements.
- [ ] Products implementation handoff requirements.
- [ ] Recommendation for separately approved implementation work.

## Non-authorization

This issue does not create operational schemas, mutate governance state, install dependencies, implement deployable UI, schedule work autonomously, call providers directly, or modify protected Cogni-Core surfaces.
- **Scope:** GitHub Labels: type:discovery, type:product-design, risk:planning-only, sync:nomos | Original Repo: The-Ethos-Project/ethos-logos

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
