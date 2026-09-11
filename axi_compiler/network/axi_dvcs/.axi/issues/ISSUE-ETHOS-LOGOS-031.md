# [ETHOS-LOGOS-031] Create confidentiality exposure matrix for agents, workflows, and connectors

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
Parent umbrella: The-Ethos-Project/ethos-nomos#36



## Purpose



Create a canonical **confidentiality exposure matrix** for agents, workflows, connectors, MCP/tool surfaces, logs, memory, and roadmap artifacts.



The project needs a clear inventory of what each surface can see, read, write, execute, transmit, or persist so sensitive project/personal/business information is handled with the same seriousness as confidential human-controlled materials.



This is a security/confidentiality companion to The-Ethos-Project/cogni-core-0060a606#479, The-Ethos-Project/cogni-core-0060a606#480, The-Ethos-Project/cogni-core-0060a606#481, and The-Ethos-Project/cogni-core-0060a606#482.



---



## Proposed artifact



Create:



```text

docs/project_management/CONFIDENTIALITY_EXPOSURE_MATRIX.md

```



---



## Relationship to existing roadmap/governance issues



Parent / related:



- The-Ethos-Project/ethos-nomos#21 — Roadmap tracker: Five Strata separation and company/product avenues

- The-Ethos-Project/cogni-core-0060a606#406 — Document GitHub governance system, issue hierarchy, and roadmap tracking model

- The-Ethos-Project/cogni-core-0060a606#479 — Create AGENT roles ledger

- The-Ethos-Project/cogni-core-0060a606#480 — Create capability and skill ledger for repo-defined Eros capabilities

- The-Ethos-Project/cogni-core-0060a606#481 — Create external agent and connector contract ledger

- The-Ethos-Project/cogni-core-0060a606#482 — Inventory canonical project workflows and authority boundaries



---



## Scope



Docs/planning only.



The matrix should classify exposure for:



- ChatGPT conversational context

- Codex repo/workspace access

- GitHub Copilot IDE access

- IDE-hosted agent/coworker surfaces

- GitHub connector

- Gmail connector

- Calendar connector

- Drive connector

- Contacts connector

- web/browser access

- local terminal/filesystem access

- MCP servers/tools

- Dependabot PR automation

- project logs / traces / evidence packets

- memory/index/harness concepts

- roadmap issues and PR bodies

- future ErosOS PM/coding harness as future/planned only



---



## Required exposure levels



Define and use exposure levels similar to:



```text

Level 0 — No access / pasted context only

Level 1 — Read-only project context

Level 2 — Scoped write access

Level 3 — Tool or command execution

Level 4 — Private-data connector access

Level 5 — Destructive or external action authority

```



The final wording can be adjusted for repo conventions, but the risk gradient must remain clear.



---



## Required matrix fields



Each entry should define:



```text

Surface name

Surface type

Exposure level

What it can see/read

What it can write/change

What it can execute

What it can transmit externally

What it can persist/log

Primary confidentiality risk

Required approval

Audit/evidence expectation

Forbidden handling

Stop conditions

Related issues/docs

```



---



## Required leak vectors to cover



The matrix should explicitly discuss likely leak vectors:



- prompt/context leakage

- repo read access

- file write access

- command output/log leakage

- GitHub issue/PR body leakage

- email content exposure

- calendar/person/location exposure

- Drive/document exposure

- contact graph exposure

- web/browser query leakage

- MCP argument/context leakage

- cloud model/provider leakage

- memory persistence

- diagnostic trace persistence

- screenshots/attachments



---



## Key safety rule



Confidentiality classification does not grant access. It documents risk and handling expectations only.



Any Level 4 or Level 5 surface should require explicit human approval and careful audit handling unless a separate issue authorizes a narrower policy.



---



## Constraints



- Do not authorize new connectors.

- Do not authorize new MCP tools.

- Do not authorize new autonomous workflows.

- Do not modify runtime code.

- Do not modify tests.

- Do not modify CI.

- Do not change imports.

- Do not change Brain Health scoring.

- Do not alter protected brain architecture.

- Do not store or reproduce secrets, credentials, private email bodies, or private document contents.

- Mark uncertain exposure as `unknown / needs discovery`.



---



## Acceptance criteria



- [ ] `docs/project_management/CONFIDENTIALITY_EXPOSURE_MATRIX.md` exists.

- [ ] Exposure levels are defined.

- [ ] Known agent/workflow/connector surfaces are mapped to exposure levels.

- [ ] Private-data connectors are clearly identified as high-confidentiality surfaces.

- [ ] Tool execution and destructive/external-action surfaces are clearly separated.

- [ ] Leak vectors are documented.

- [ ] Human approval and audit expectations are documented.

- [ ] The matrix cross-references The-Ethos-Project/cogni-core-0060a606#479, The-Ethos-Project/cogni-core-0060a606#480, The-Ethos-Project/cogni-core-0060a606#481, The-Ethos-Project/cogni-core-0060a606#482, The-Ethos-Project/cogni-core-0060a606#406, and The-Ethos-Project/ethos-nomos#21.

- [ ] `docs/DOCS_INDEX.md` is updated if repo convention requires it.

- [ ] No runtime, tests, CI, imports, Brain Health, protected architecture, repo extraction, or new authority changes occur.



---



## Suggested branch



```text

codex/issue-483-confidentiality-exposure-matrix

```



---



## Suggested checks



```powershell

git diff --check

python tools/architecture_guard.py

python tools/brain_law.py

```



---



## Stop conditions



Stop before:



- exposing secrets/private data in the doc

- granting access to a connector

- authorizing new tools or agents

- editing runtime/protected architecture

- changing CI/tests/imports

- changing Brain Health scoring

- authorizing Five Strata extraction
- **Scope:** GitHub Labels: bug, documentation | Original Repo: The-Ethos-Project/ethos-logos

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
