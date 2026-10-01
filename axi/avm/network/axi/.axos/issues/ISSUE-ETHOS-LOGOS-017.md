# [ETHOS-LOGOS-017] Govern and expand generative code construction workflow

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Summary

Track the new governed workflow for using a generative LLM as a candidate code-construction surface without granting it live repository authority.

The workflow separates:

1. human architecture authority;
2. repository-aware specification;
3. generative candidate construction;
4. repository-aware integration;
5. protected validation and PR review.

## Current documentation added

- `docs/project_management/GENERATIVE_CODE_CONSTRUCTION_WORKFLOW.md`
- `docs/project_management/generative_templates/README.md`
- `docs/project_management/generative_templates/GENERAL_CODE_CONSTRUCTION_TEMPLATE.md`
- `docs/project_management/generative_templates/FEATURE_EXTENSION_TEMPLATE.md`
- `docs/project_management/generative_templates/TEST_SUITE_GENERATION_TEMPLATE.md`
- `docs/project_management/generative_templates/INTEGRATION_ADAPTER_TEMPLATE.md`
- `docs/project_management/generative_templates/UI_COMPONENT_GENERATION_TEMPLATE.md`
- `docs/project_management/generative_templates/API_DATA_CONTRACT_TEMPLATE.md`

## Required follow-up

- Link the workflow from `AGENTS.md`.
- Add generative candidate construction as a non-executing preparation surface in `docs/project_management/TASK_INTAKE_TEMPLATE.md`.
- Add a workflow gate covering candidate-generation authorization and repository-aware integration review in `docs/project_management/WORKFLOW_GATES.md`.
- Add remaining stable specialized templates only where they provide distinct value:
  - new subsystem;
  - refactor proposal;
  - documentation/architecture package.
- Confirm terminology does not create a third execution lane or competing governance authority.
- Verify docs-only consistency and project-context checks.

## Constraints

- Docs-only.
- No runtime, tests, CI, imports, protected architecture, Brain Health scoring, or repository-layout changes.
- The generative model remains a preparation surface, not a repository execution lane.
- Generated code remains untrusted until repository-aware integration and validation.
- Existing Codex, GitHub Copilot, ChatGPT connector-assisted, Terminal/Ollama, and human/manual lane governance remains authoritative.

## Acceptance criteria

- Canonical docs clearly distinguish candidate generation from repository execution.
- The reusable template library is discoverable from agent and intake docs.
- Tasks record the template/model/specification, assumptions, accepted/adapted/rejected candidate code, integration lane, validation, commit, and PR.
- No duplicate executor, alternate cognition pipeline, or governance authority is implied.
- Docs-only checks pass.

## Related

- Issue The-Ethos-Project/cogni-core#11
- Issue The-Ethos-Project/cogni-core-0060a606#606
- `docs/project_management/GENERATIVE_CODE_CONSTRUCTION_WORKFLOW.md`
- **Scope:** GitHub Labels: documentation | Original Repo: The-Ethos-Project/ethos-logos

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
