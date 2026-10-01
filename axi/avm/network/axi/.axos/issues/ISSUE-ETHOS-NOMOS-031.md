# [ETHOS-NOMOS-031] Canonize the Five-Strata ecosystem, authority boundaries, and cutover states

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Goal

Maintain the canonical ecosystem model for the five private repositories and distinguish repository establishment, technical baseline, authority handoff, public release, and final legacy disposition.

## Canonical repositories

- `The-Ethos-Project/cogni-core` — protected cognition.
- `The-Ethos-Project/ethos-products` — ErosOS and Product surfaces.
- `The-Ethos-Project/domus-ethos` — simulation and embodiment.
- `The-Ethos-Project/ethos-logos` — research, interpretation, product discovery, UX, and suggestive insights.
- `The-Ethos-Project/ethos-nomos` — governance, standards, contracts, workflow law, ownership, and coordination.

## Cutover states

1. Repository established.
2. Private destination baseline copied or authored.
3. Issue and authority references reconciled.
4. Repository functional baseline verified.
5. Cross-repository contracts and consumers verified.
6. Canonical authority handoff approved.
7. Legacy repository frozen/archived through separate approval.
8. Public release considered separately under #92/#93.

No earlier state implies a later state.

## Current baseline issues

- `The-Ethos-Project/cogni-core#46`
- `The-Ethos-Project/ethos-products#42`
- `The-Ethos-Project/domus-ethos#15`
- `The-Ethos-Project/ethos-logos#64`
- #96

## Required doctrine

- exactly one primary owner per behavior or artifact;
- cross-repository visibility through links and synchronization labels;
- repository-local enforcement remains blocking;
- Nomos coordination cannot override protected Cogni law or the owner of governed behavior;
- Logos research and insights are non-authoritative until adopted through an approved owner;
- Products and Domus cannot call providers directly;
- public surfaces are releases, not a sixth stratum;
- legacy material is evidence, not active authority.

## Related issues

- #21 — master roadmap
- #89 — authority contract
- #94 — issue reconciliation
- #96 — Nomos handoff
- `The-Ethos-Project/ethos-products#43` — Product naming

## Non-authorization

This issue does not move source or issues, publish artifacts, change repository settings, approve technical cutover, or archive the legacy repository.
- **Scope:** GitHub Labels: documentation, type:governance, type:migration, sync:logos | Original Repo: The-Ethos-Project/ethos-nomos

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
