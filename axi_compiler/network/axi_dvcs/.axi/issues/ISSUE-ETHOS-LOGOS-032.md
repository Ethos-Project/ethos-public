# [ETHOS-LOGOS-032] Track public research-agent readiness as dependent on Five Strata separation

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
Parent umbrella: The-Ethos-Project/ethos-nomos#45

## Goal

Track the realization that Gemini, NotebookLM, and similar external research agents cannot be fully utilized against the full Eros project until the project has a clean public-facing repository or research surface.

This is a Five Strata / public-readiness blocker, not a Gemini-specific tooling issue.

## S.O.C.R.A.T.E.S. linkage\n\n- Tracks through [#320](https://github.com/Chris-Cheeseman/cogni-core/issues/320) as the master separation roadmap.\n- Tracks through [#615](https://github.com/Chris-Cheeseman/cogni-core/issues/615) as the control-plane milestone/dashboard companion.\n\n## Realization

External research agents need enough project scope to reason accurately, but the current working repo is not yet safe or clean enough to expose wholesale. The barrier to public research-agent use is the unfinished Five Strata separation and the absence of a public-safe canonical surface.

## Why this matters

If Gemini, NotebookLM, or similar tools ingest the current working repo before separation is complete, they may incorrectly treat protected runtime material, internal governance notes, historical docs, private planning assumptions, or non-canonical artifacts as public/current project truth.

That creates risk of:

- public-language drift
- historical docs being treated as current canon
- governance confusion
- protected architecture exposure
- external-agent recommendations based on private or unstable material
- accidental pressure toward premature repo extraction

## Required public-facing surface

Before external research agents can be used safely at full project scope, the public-facing surface should:

- expose current canonical Eros architecture
- distinguish current canon from historical/archive material
- exclude protected runtime/private governance material
- distinguish public language from internal/private research language
- preserve Five Strata boundaries
- support safe ingestion by external research agents
- avoid implying that products, lab work, or experiments define the protected brain

## Scope

This issue should track planning and roadmap dependency only.

It may include:

- public-readiness criteria
- Five Strata dependency notes
- research-agent ingestion requirements
- candidate public repo / research-surface requirements
- documentation classification needs
- public/private language boundary notes

## Explicit non-authorizations

This issue does **not** authorize:

- Five Strata extraction
- repo moves
- runtime changes
- protected brain changes
- CI changes
- import rewrites
- Brain Health scoring changes
- architecture guard changes
- public publication
- external agent ingestion of the private working repo

## Acceptance criteria

- The roadmap explicitly records that public research-agent use is blocked by unfinished Five Strata separation.
- The issue identifies the minimum public-safe surface external research agents need.
- The issue distinguishes tooling capability from repository-boundary readiness.
- The issue preserves protected brain/runtime/governance boundaries.
- The issue does not authorize extraction or publication.

## Stop condition

Stop after documenting and tracking this dependency. Do not open implementation PRs or extraction work from this issue unless separately authorized.
- **Scope:** GitHub Labels: None | Original Repo: The-Ethos-Project/ethos-logos

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
