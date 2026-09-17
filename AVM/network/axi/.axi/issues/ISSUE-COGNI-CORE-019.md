# [COGNI-CORE-019] [Discussion] Evaluate a PostgreSQL-centered polyglot development stack

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Status

**Discussion record only.**

This issue captures a brainstormed stack direction for structured evaluation. It does **not** approve an architecture, authorize a SQLite migration, select an implementation language, change dependencies, or authorize edits to protected brain, kernel, ethics, memory, identity, audit, or routing surfaces.

## Current goal

Explore a cleaner development and data workflow that allows individual concepts or bounded project areas to be developed, imported, exported, reviewed, and tested without treating a live SQLite database as something GitHub can directly branch or merge.

GitHub would remain the source of truth for code, schemas, migrations, contracts, tests, issues, and pull requests. Database state would need its own deliberate workflow for scoped fixtures, exports, imports, snapshots, and migrations.

## Discussed stack shape

```text
                         PostgreSQL
                              |
                    Governed data contracts
                              |
          +-------------------+-------------------+
          |                            |                   |
 Java / TS                   Python        analytics
 API + GraphQL     Cogni-Core        research/reports
            (MCP)
          |
 C native components where targeted low-level performance requires them
```

### Candidate responsibilities

- **PostgreSQL** — contemplated durable relational foundation and possible replacement for selected or all SQLite-backed stores.
- **GraphQL** — candidate language-neutral application interface over governed data and service contracts.
- **Java, Go, C#, or TypeScript** — candidates for an API/service boundary; no language has been selected.
- **Python** — retains Cogni-Core, AI, cognition, orchestration, and existing Python strengths.
- **R** — analytical research, statistics, visualization, and reporting.
- **C** — narrowly targeted native or performance-critical components, not a default application layer.

## Why this direction was discussed

- Make bounded data subsets easier to import and export.
- Support isolated work on individual concepts without sharing or committing a live database file.
- Give schema and data-contract changes a reviewable GitHub workflow.
- Avoid forcing every project concern into one programming language.
- Establish clearer service and ownership boundaries.
- Preserve relational integrity where relationships, provenance, and causal lineage matter.
- Keep analytics and research workflows independent from the protected cognition runtime.

## Confirmed repository baseline

As of 2026-07-31:

- The repository contains multiple SQLite-backed stores rather than one interchangeable database.
- SQLite currently supports several materially different domains, including memory, identity, audit, model, anomaly, preference, credential, and thread-related state.
- SQLAlchemy and Alembic are not currently established repository dependencies.
- The repository does not currently contain an authorized application GraphQL server layer.
- Existing GraphQL usage for GitHub pull-request review is collaboration tooling only; it does not establish runtime GraphQL authority.
- Runtime database files are local application data and are not intended to be versioned directly in Git.

These facts mean “replace SQLite” is not yet one bounded migration. Each data domain would require classification, ownership, security, compatibility, and rollback decisions.

## Architectural constraints

Any later proposal must preserve the following:

- Cogni-Core remains the cognitive authority.
- Product and API layers may not bypass the governed cognitive pipeline.
- A GraphQL layer must not become an alternate inference, ethics, memory, or final-response path.
- Choosing an API language does not transfer cognitive authority out of Cogni-Core.
- Signal routing and causal lineage must remain interpretable.
- Protected data domains require explicit, separate approval before schema or storage changes.
- Five Strata ownership must be considered before placing new services or durable data.
- Hosting, replication, and protected-data residency are undecided.

## Open questions

1. Which SQLite-backed domains, if any, should PostgreSQL replace?
2. Should local/offline operation remain a hard requirement?
3. Would PostgreSQL be local, self-hosted, managed, or support multiple deployment modes?
4. Which language is the best fit for the governed API boundary: Java, Go, C#, TypeScript, or continued Python?
5. Should the application contract use GraphQL, REST, both, or another interface?
6. What is the safe workflow for scoped data export/import, test fixtures, snapshots, and branch-specific development?
7. How would schema migration, compatibility, rollback, backup, and recovery work?
8. Which data is safe to export, and how will secrets, identity, memory, audit, and other protected records be excluded or sanitized?
9. Are any NoSQL systems justified by measured requirements, or would PostgreSQL capabilities be sufficient?
10. Which future Five Strata repository should own each service, schema, and analytical artifact?
11. What proof of concept would answer the highest-risk questions without modifying protected runtime surfaces?

## Evaluation criteria

A later decision should compare candidates on:

- scoped import/export and reproducible fixtures;
- local and offline development;
- privacy and protected-data handling;
- relational integrity and transaction behavior;
- causal lineage and auditability;
- schema migration and rollback safety;
- compatibility with the Python cognition core;
- multi-language client support;
- CI and pull-request ergonomics;
- operational burden, cost, and recoverability;
- performance based on measured needs rather than assumption.

## Non-goals

This issue does not:

- authorize implementation or a proof-of-concept branch;
- authorize removing or replacing SQLite;
- select PostgreSQL, GraphQL, NoSQL, or an API language;
- authorize new runtime dependencies or hosted services;
- authorize schema changes or data movement;
- authorize changes to protected surfaces;
- make live database contents part of Git;
- treat GitHub PR-review GraphQL usage as an application architecture decision.

## Suggested discussion outcome

If the direction remains promising, the next artifact should be a repository-grounded decision record that:

1. inventories the SQLite stores and classifies their protection and ownership boundaries;
2. defines the required import/export development experience;
3. compares a small set of viable architectures against the criteria above;
4. identifies one deliberately non-protected proof-of-concept candidate;
5. establishes explicit approval gates before any implementation or migration work begins.
- **Scope:** GitHub Labels: None | Original Repo: The-Ethos-Project/cogni-core

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
