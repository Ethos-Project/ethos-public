# [ETHOS-LOGOS-033] Track Eros positioning as local human-trust governance layer

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
Parent umbrella: The-Ethos-Project/ethos-nomos#45

## Context

A competitor/market research pass on CREAO AI raised a useful positioning concern: Eros should not be framed publicly as merely another agent platform that “runs the business” or replaces human authority.

The safer and more strategically distinct framing is:

> Eros is a local-first governance layer for AI agents, designed to make tool use, coding workflows, business automation, and future autonomy auditable, constrained, reversible, and human-trustworthy.

This captures the key distinction:

- CREAO-style products emphasize agents running workflows.
- Eros should emphasize trusted local governance over agent action.
- Humans remain the authority during developmental phasing.
- Delegation is bounded, inspectable, and reversible.

## Decision to track

Use **“local governance humans can trust”** as a strategic positioning direction, with careful wording:

- Trustworthy does not mean infallible.
- Trust comes from structure, not claims.
- Public language should emphasize auditability, bounded authority, local control, human oversight, verification, and reversibility.

## Recommended placement

This belongs in project-management / positioning / governance documentation, not runtime code.

Candidate docs to update or create:

- `docs/project_management/EROS_CANONICAL_CONTEXT.md`
- `docs/project_management/IDE_AGENT_COWORKER_CONTRACT.md` if agent/tool governance language is relevant
- a future positioning note such as `docs/project_management/LOCAL_GOVERNANCE_POSITIONING.md`

## Scope

Docs/planning only.

Allowed:

- Add or update positioning/governance wording.
- Clarify the distinction between agent execution and Eros governance authority.
- Add safe public-language guidance for ErosOS / Dev Forge / Ethos Suite positioning.
- Preserve local-first, human-supervised, audit-first framing.

Forbidden:

- Runtime changes.
- CI changes.
- Import changes.
- Brain Health scoring changes.
- Architecture guard changes.
- Protected brain/orchestrator/kernel changes.
- New inference providers or SDK dependencies.
- Any claim that Eros is infallible, fully autonomous, or conscious in public positioning.

## Acceptance criteria

- A canonical docs note captures Eros as a local-first human-trust governance layer.
- The note clearly distinguishes Eros from generic workflow-agent platforms.
- The note includes safe public wording and unsafe wording to avoid.
- The note preserves the rule that humans remain authority during developmental phasing.
- No runtime/protected architecture files are modified.

## Suggested safe wording

> Eros helps humans safely delegate work to AI agents by keeping decisions local, auditable, constrained, reversible, and governed by explicit constraints.

> Eros is the trusted local governance brain that decides what agents are allowed to do, verifies what they did, and keeps humans in control.

## Suggested unsafe wording to avoid

- Eros runs your business for you.
- Eros replaces employees.
- Eros cannot do harm.
- Eros is fully autonomous.
- Humans can fully trust Eros without review.
- Eros is conscious.

## Stop conditions

Stop if the work begins touching runtime, protected brain architecture, CI, imports, model integration, or dependency policy. This issue is only for positioning/governance documentation.
- **Scope:** GitHub Labels: documentation | Original Repo: The-Ethos-Project/ethos-logos

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
