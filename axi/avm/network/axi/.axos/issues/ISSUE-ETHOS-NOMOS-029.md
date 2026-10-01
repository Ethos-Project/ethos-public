# [ETHOS-NOMOS-029] Define ethos-nomos tooling scope and governance boundaries.

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Goal

Move `ethos-nomos` from a governance/tooling destination shell to readiness for a separately approved clean-copy extraction PR without creating parallel canon.

## Current status

- Private destination shell: `The-Ethos-Project/ethos-nomos`
- Program umbrella: The-Ethos-Project/ethos-nomos#25
- Inventory anchor: The-Ethos-Project/cogni-core-0060a606#629
- Nomos remains last in the extraction sequence.

## Known blockers

- Shared Nomos authority would be circular while active governance still protects the unified source repository.
- Local guard tooling and CI are Cogni-Core-protective, not generic infrastructure.
- MCP and connector surfaces can reach protected internals.
- Cross-repository governance precedence, versioning, publishing, and rollback are not complete.
- Moving governance first could create parallel canonical documentation.

## Task breakdown

1. Inventory governance, project-management, `.github`, tools, automation, MCP, and connector surfaces.
2. Classify each item as Cogni-Core-local enforcement, reusable Nomos tooling, connector/process support, historical governance, or template material.
3. Define cross-repository authority and precedence.
4. Define the reusable-versus-local guard split.
5. Define MCP and connector identity and ingress constraints.
6. Define documentation synchronization, versioning, release, and rollback.
7. Produce an exact destination manifest that cannot create a second authority layer.

## Acceptance criteria

- Per-tool and per-document ownership is explicit.
- Reusable tooling and Cogni-Core-local enforcement are separated.
- Connector access is excluded or governed by explicit contracts.
- No Nomos document can override `EROS.md`, Cogni-Core Law, or repo-local enforcement.
- A separately approved copy-first PR can use the manifest without weakening local gates.

## Non-authorization

This issue does not authorize extraction, CI changes, source deletion, canonical cutover, protected-surface changes, or parallel canonical documents.

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
