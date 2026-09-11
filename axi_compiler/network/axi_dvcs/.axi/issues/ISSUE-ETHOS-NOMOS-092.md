# [ETHOS-NOMOS-092] Governance: private canonical strata and public release boundary

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Objective

Establish the repository-visibility and publication doctrine for the Eros ecosystem after issue separation.

## Decision

The five canonical strata repositories are private internal repositories and remain the authoritative working system:

- `The-Ethos-Project/cogni-core`
- `The-Ethos-Project/ethos-products`
- `The-Ethos-Project/ethos-logos`
- `The-Ethos-Project/ethos-nomos`
- `The-Ethos-Project/domus-ethos`

All five are currently configured as private repositories as of 2026-08-07.

The default policy is:

> Internal canon is private. Public repositories are intentional release surfaces created only after readiness, disclosure, security, licensing, and maintenance review.

## Public-release model

A public repository is not a sixth stratum and is not a parallel authority source. It is a purpose-built publication or product surface derived from reviewed private canon.

Potential future public repositories may include:

- dedicated product repositories;
- SDK/API repositories;
- reference implementations;
- research/publication packages;
- specifications or public documentation repositories.

## Authority rules

- Private strata remain upstream authority.
- Public repositories must not become alternate project canon.
- Internal issues, roadmap discussion, traces, private governance, protected architecture, and unfinished research are not exported by default.
- Public release must use an explicit synchronization/release method rather than informal copying.
- Public release is never implied by issue placement inside a private stratum.

## Required release gates

Before a public-facing repository is created or populated, record evidence for:

1. product/research functional readiness;
2. tests and validation;
3. security and dependency review;
4. protected-IP/disclosure review;
5. secrets, traces, internal paths, and private-governance removal;
6. public-safe claims and documentation review;
7. license/reuse decision;
8. support and maintenance owner;
9. API/product/research boundary;
10. versioning and release policy;
11. synchronization/update strategy from private canon;
12. explicit approval to publish.

## Non-authorization

This decision does not authorize creation of a public repository, publication of any private material, licensing decisions, product release, research publication, or disclosure of protected architecture.

## Follow-up

- Reconcile `ethos-nomos#21` and `ethos-nomos#25` to this doctrine.
- Remove active umbrella language that treats the five strata as future public destinations or bootstrap shells.
- Add a reusable Public Release Readiness issue template.
- Track remaining hierarchy/reference drift as a bounded reconciliation workstream.
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
