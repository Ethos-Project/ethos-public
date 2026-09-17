# [COGNI-CORE-023] Audit capability-demand semantics against the geometric ethics field

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
Parent umbrella: The-Ethos-Project/cogni-core-0060a606#589

## Objective

Track a focused, assessment-only audit of the new capability-demand taxonomy against the canonical geometric ethics field so semantic alignment work is not lost while ErosOS/runtime stabilization proceeds.

## Context

PR The-Ethos-Project/cogni-core-0060a606#590 introduces a pure capability-demand contract under The-Ethos-Project/cogni-core-0060a606#589. Recent review findings were primarily structural contract defects, but several capability labels are semantically loaded (`SAFETY_REVIEW`, `RISK_PREFILTER`, `HIGH_RISK_ESCALATION`, `REFLECTION`, `IDENTITY`). These must eventually align with the canonical geometric ethics field rather than becoming a parallel or conflicting ontology.

## Scope

- Identify the active production geometric ethics field implementation and canonical attractor/weight definitions.
- Distinguish active runtime sources from demos, archived modules, legacy documents, and generated artifacts.
- Inventory current attractor names, categories, weights, phase multipliers, semantic keywords, and evaluation outputs.
- Compare capability-demand terminology against the field’s canonical semantic vocabulary.
- Identify duplicated, contradictory, overloaded, or missing concepts.
- Define which capability demands require ethics-field evaluation before any mentor selection or execution.
- Define a bounded mapping contract between capability assessment and ethics-field review requirements.
- Preserve uncertainty and causal lineage in the proposed mapping.
- Produce recommendations only; do not modify runtime or protected field code in the first slice.

## Architectural Boundary

Capability assessment answers:

`What cognitive operation does this input require?`

The geometric ethics field answers:

`What ethical tensions, risks, trajectory, and authorization conditions apply?`

The capability taxonomy must not duplicate attractor weights, authorize inference, or replace field evaluation.

## Constraints

- Assessment/documentation only for the first slice.
- No changes to geometric field weights or attractor geometry.
- No changes to EthicsKernel, InferenceGate, BrainOrchestrator, pipeline orchestrator, `llm_bridge`, or ErosOS routing.
- No runtime integration.
- No new executor, shadow pipeline, or alternate semantic authority.
- Do not import protected field modules into the pure capability-demand contract.
- Treat demos, archived modules, legacy documents, and generated evidence as non-canonical until current runtime use is proven.
- Preserve Kernel Space / User Space separation.

## Deliverables

- Canonical active-field source map.
- Attractor and weight inventory with evidence paths.
- Capability-to-field-review mapping matrix.
- Semantic conflict/gap report.
- Recommendation for a later bounded integration slice, if warranted.
- Explicit list of protected files that would require approval before implementation.

## Acceptance Criteria

- [ ] Active production field source is identified from repository/runtime evidence.
- [ ] Demo, archived, legacy, and generated sources are clearly separated from canon.
- [ ] Capability labels are compared against field attractors and categories.
- [ ] Safety/identity/reflection/escalation semantics do not create a second ethics ontology.
- [ ] Required ethics-field review triggers are documented without wiring runtime behavior.
- [ ] No protected runtime or field implementation files are changed.
- [ ] Findings are linked back to The-Ethos-Project/cogni-core-0060a606#589 and PR The-Ethos-Project/cogni-core-0060a606#590.

## Relationship

- Follow-up to The-Ethos-Project/cogni-core-0060a606#589
- Semantic-governance companion to PR The-Ethos-Project/cogni-core-0060a606#590
- Must not block tonight’s ErosOS/runtime stabilization unless the audit finds a direct safety-critical contradiction in the active path.

## Stop Conditions

Stop and request explicit approval if the audit indicates a need to:

- modify field weights or attractors;
- alter protected ethics/kernel code;
- wire capability assessment into runtime routing;
- change mentor selection or inference authorization;
- reinterpret archived/demo semantics as canonical without runtime evidence.
- **Scope:** GitHub Labels: None | Original Repo: The-Ethos-Project/cogni-core

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
