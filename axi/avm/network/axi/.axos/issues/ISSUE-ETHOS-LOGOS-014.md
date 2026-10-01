# [ETHOS-LOGOS-014] Classify ethos-logos document authority and internal disclosure boundaries

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Goal

Clarify document authority, privacy, historical status, and disclosure boundaries inside the **private canonical `ethos-logos` repository**.

## Current status

- Canonical private repository: `The-Ethos-Project/ethos-logos`
- Ecosystem governance: `The-Ethos-Project/ethos-nomos#92`
- Five Strata reconciliation: `The-Ethos-Project/ethos-nomos#25`
- Issue-topology reconciliation: `The-Ethos-Project/ethos-nomos#94`

`ethos-logos` is no longer treated as a future public destination shell. It is a private internal research/documentation stratum.

## Purpose

Classify Logos documents so internal authority is understandable and future public-release decisions can be made deliberately without requiring Logos itself to become public.

## Task breakdown

1. Inventory candidate Logos/research/explanatory documents as needed.
2. Classify each as appropriate:
   - active Logos research/canon;
   - supporting internal documentation;
   - historical evidence;
   - generated evidence;
   - stale/superseded;
   - candidate for a future public release package;
   - protected/private/internal-only.
3. Define citation and precedence relative to Cogni-Core runtime authority and Nomos governance.
4. Define stale-document labels, archive rules, and mirror/reference rules.
5. Identify documents whose authoritative home belongs in Cogni-Core, Nomos, Products, or Domus rather than Logos.
6. Identify candidate public-safe material without publishing or moving it.
7. For any future outward release, use the Public Release Readiness process under `The-Ethos-Project/ethos-nomos#93`.

## Authority rules

- Logos owns research and explanatory work within its defined boundary.
- Logos does not override protected Cogni-Core runtime authority or Nomos ecosystem governance.
- Private/internal is the default posture.
- A document being public-safe does not require it to be published.
- Future public research or documentation should normally be released through a purpose-built public repository/package after separate approval.

## Acceptance criteria

- Every reviewed candidate document has a clear authority/status classification.
- Active authority remains discoverable.
- Historical and generated materials cannot be mistaken for current authority.
- Private/internal research is preserved by default.
- Candidate outward-facing material is clearly separated from protected/internal material.
- Public release is treated as a separate readiness and publication decision.
- No competing doctrine or protected Cogni-Core authority is created.

## Non-authorization

This issue does not authorize document publication, creation of a public repository, source deletion, licensing decisions, runtime changes, protected Cogni-Core changes, or disclosure of private/protected material.
- **Scope:** GitHub Labels: documentation, type:governance, risk:planning-only, sync:nomos | Original Repo: The-Ethos-Project/ethos-logos

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
