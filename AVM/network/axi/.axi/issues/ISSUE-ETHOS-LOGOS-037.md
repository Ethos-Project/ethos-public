# [ETHOS-LOGOS-037] Define NotebookLM and Gemini mobile research lane authority boundary

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
Parent umbrella: The-Ethos-Project/cogni-core-0060a606#392

## Goal

Define the governed role, capability boundary, source-ingestion rules, and handoff workflow for using Google NotebookLM and Google Gemini as mobile-accessible research/planning surfaces for The Eros Project.

This issue exists because NotebookLM/Gemini can support mobile project work, but they must not become undocumented repo authorities, shadow dev lanes, or alternate canonical planning systems.

## Context

NotebookLM and Gemini are now part of the project documentation/research workflow:

- NotebookLM gets source information from the `chrischeeseman.dev@gmail.com` Google Drive connector / Google Drive document set.
- NotebookLM sources must be added manually per Notebook.
- Each Notebook may function as a focused research/planning workspace, or "lane," for a bounded topic.
- Google Gemini can also now be used directly as a user-facing LLM surface.
- Gemini may use the notebooks/source sets created for these lanes as context for synthesis and planning.
- Outputs from NotebookLM or Gemini are not project authority until converted into repo-tracked issues, comments, docs, or PRs and reviewed through the normal governance path.

## Related issues

- The-Ethos-Project/cogni-core-0060a606#392 — LLM co-worker contract for repo authority and handoff
- The-Ethos-Project/cogni-core-0060a606#433 — document authority registry and documentation lifecycle policy
- The-Ethos-Project/cogni-core-0060a606#434 — document inventory and classification across repo / Drive / Gemini / NotebookLM sources

## Required deliverable

Create a docs-only governance document, recommended path:

`docs/project_management/NOTEBOOKLM_GEMINI_MOBILE_RESEARCH_LANE.md`

The document must define:

1. **NotebookLM role**
   - document reading/synthesis assistant
   - manually curated source-set workspace
   - mobile research/planning lane
   - source-grounded contradiction finder
   - issue/comment/doc-prompt drafting assistant

2. **Gemini role**
   - user-facing LLM surface for project research/planning
   - synthesis assistant that may use NotebookLM notebooks/source sets
   - not a repo authority and not an implementation executor

3. **Source model**
   - source material originates from the `chrischeeseman.dev@gmail.com` Google Drive connector / Google Drive document set
   - sources are manually added per Notebook
   - Notebooks are scoped workspaces, not canonical repositories
   - source freshness and source completeness must be treated as explicit uncertainty

4. **Allowed workflows**
   - read/summarize source docs
   - compare Drive docs, repo docs, exports, and Notebook source sets
   - generate mobile-friendly research notes
   - draft issue bodies, PR prompts, and documentation proposals
   - identify stale/conflicting/duplicate project documents
   - prepare handoffs for Codex, Copilot, ChatGPT, or human local work

5. **Forbidden workflows**
   - no branch ownership
   - no commits
   - no PR creation or merge authority
   - no CI/check interpretation as final authority
   - no runtime/protected brain edits
   - no inference/ethics/orchestrator bypass
   - no replacing GitHub issues, repo docs, or live Git state as source of truth
   - no treating NotebookLM/Gemini generated text as canonical without repo review

6. **Mobile lane boundary**
   - NotebookLM/Gemini may support mobile research and planning when away from the workstation
   - mobile lane output must become actionable only by being converted into a GitHub issue/comment/doc PR/handoff prompt
   - implementation still follows branch → focused change → checks → commit → PR

7. **Handoff format**
   - source notebook name/topic
   - Drive source set or exported source list, when available
   - summary of findings
   - confidence/uncertainty notes
   - recommended GitHub issue/doc update/PR prompt
   - explicit statement that repo docs and live Git state remain authoritative

## Constraints

- Docs/planning only.
- Do not modify runtime code.
- Do not modify tests or CI.
- Do not change imports.
- Do not change Brain Health scoring.
- Do not authorize Five Strata extraction.
- Do not promote Google Drive, NotebookLM, or Gemini output to canon in this issue.
- Do not create a new implementation lane equivalent to Codex or Copilot.

## Acceptance criteria

- A canonical NotebookLM/Gemini mobile research-lane document exists.
- It explicitly records that NotebookLM source info comes through `chrischeeseman.dev@gmail.com` Google Drive connector / Google Drive source material.
- It explicitly records that sources are manually added per Notebook.
- It explicitly records that each Notebook may act as a scoped research/planning lane, not a repo authority.
- It explicitly records that Gemini can be used as a direct user-facing LLM surface using notebooks/source sets as contextual lanes.
- It clearly subordinates NotebookLM and Gemini output to repo authority, GitHub issues/PRs/docs, and live Git state.
- It links back to The-Ethos-Project/cogni-core-0060a606#392, The-Ethos-Project/cogni-core-0060a606#433, and The-Ethos-Project/cogni-core-0060a606#434.
- No runtime, CI, import, protected architecture, Brain Health, or extraction changes are made.
- **Scope:** GitHub Labels: bug | Original Repo: The-Ethos-Project/ethos-logos

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
