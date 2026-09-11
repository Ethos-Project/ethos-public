# [ETHOS-LOGOS-027] create wiki and pages documentation surface policy

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
Parent umbrella: The-Ethos-Project/cogni-core-0060a606#238, The-Ethos-Project/ethos-nomos#21, The-Ethos-Project/cogni-core-0060a606#406

## purpose

Create a docs/planning policy for GitHub Wiki and GitHub Pages as project documentation surfaces.

The goal is to make future contributor documentation, internal how-to material, external walkthroughs, and public project guides useful without letting those surfaces override repository canon, governance docs, Brain Law, architecture guard, protected architecture, or human approval gates.

---

## parent / related roadmap anchors

- The-Ethos-Project/ethos-nomos#21 — roadmap tracker: five strata separation and company/product avenues
- The-Ethos-Project/cogni-core-0060a606#406 — document github governance system, issue hierarchy, and roadmap tracking model
- The-Ethos-Project/cogni-core-0060a606#482 — inventory canonical project workflows and authority boundaries
- The-Ethos-Project/cogni-core-0060a606#489 — plan automation security and reliability workflow checks, related only where documentation surfaces affect automation governance

---

## scope

Docs/planning only.

Plan documentation-surface policy for:

- GitHub Wiki reference pages
- GitHub Pages public/static documentation
- internal contributor how-to material
- external walkthroughs and public guides
- canon-mirror disclaimers
- repo-docs authority precedence
- page status labels such as reference, internal how-to, external walkthrough, canon mirror, and historical
- future approval requirements for any tool-assisted documentation edits

---

## core policy to preserve

GitHub Wiki and GitHub Pages are documentation/project surfaces. They may summarize, explain, or guide interaction with canon, but they do not override repository canon, governance docs, Brain Law, architecture guard, protected architecture, or human approval gates.

The repository remains the authority surface. Wiki and Pages are interaction/documentation surfaces.

---

## proposed documentation posture

| surface | role | authority posture |
|---|---|---|
| repository `/docs` | canonical governance, plans, architecture records, and reviewed project memory | authoritative |
| GitHub Wiki | contributor handbook, internal how-to, reference material, and living field manual | non-canonical reference |
| GitHub Pages | polished public/static documentation and external walkthrough surface | non-canonical publication surface |
| issues / PRs | decision trail, implementation evidence, and roadmap linkage | traceability / governance evidence |

---

## constraints

- Do not modify runtime code.
- Do not modify tests.
- Do not modify CI.
- Do not change imports.
- Do not change Brain Health scoring.
- Do not modify protected brain architecture.
- Do not enable GitHub Pages deployment.
- Do not create or mutate wiki pages unless separately authorized.
- Do not create MCP tools or connector access.
- Do not grant any new tool or agent authority.
- Do not imply wiki or Pages content can override repository canon.
- Do not authorize public release, publication, legal claims, healthcare operation, or autonomous agent access.
- Treat all workflow proposals as planning until separately implemented under reviewed issues.

---

## acceptance criteria

- [ ] A docs/planning artifact defines GitHub Wiki and GitHub Pages as non-canonical documentation/project surfaces.
- [ ] The artifact states that repository canon wins over wiki/page content in any conflict.
- [ ] The artifact distinguishes internal how-to, external walkthrough, reference, canon mirror, and historical page statuses.
- [ ] The artifact defines a safe future posture for tool-assisted documentation changes without granting new authority.
- [ ] The artifact links The-Ethos-Project/ethos-nomos#21, The-Ethos-Project/cogni-core-0060a606#406, The-Ethos-Project/cogni-core-0060a606#482, and The-Ethos-Project/cogni-core-0060a606#489 where appropriate.
- [ ] The artifact preserves lowercase naming standards for new docs/branches/artifacts unless an existing repo convention requires otherwise.
- [ ] No runtime, tests, CI, imports, Brain Health scoring, protected architecture, wiki mutation, Pages deployment, connector creation, MCP tool creation, or new agent authority is authorized.

---

## suggested branch

```text
codex/issue-###-wiki-pages-documentation-surface-policy
```

Replace `###` with the actual issue number once created.

---

## suggested doc path

```text
docs/governance/wiki_pages_documentation_surface_policy.md
```

---

## suggested checks

For docs/planning PRs in this issue family:

```powershell
git diff --check
python tools/architecture_guard.py
python tools/brain_law.py
```

---

## stop conditions

Stop before:

- enabling GitHub Pages
- mutating wiki pages
- changing CI workflows
- editing runtime/protected architecture
- changing branch policy
- granting tool execution rights
- creating MCP tools or connectors
- publishing public-facing claims
- treating wiki or Pages as canon
- bulk-changing issue state, labels, or roadmap fields
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
