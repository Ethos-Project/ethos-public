# [ISSUE-024] Implement Cogni-Core-governed mentor routing and user-input capability selection

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
Parent umbrella: The-Ethos-Project/cogni-core-0060a606#576

## Objective

Implement the runtime realization of the Developmental Mentor Network so Eros can interpret user-input needs, select approved mentor capabilities, and consult the wider mentor roster through the canonical Brain-controlled inference path.

This issue carries forward the specialist-routing and cognition-trace scope deferred from The-Ethos-Project/cogni-core-0060a606#576 and operationalizes the doctrine established by The-Ethos-Project/cogni-core-0060a606#567.

## Lineage

Derived from and aligned with:

- The-Ethos-Project/cogni-core-0060a606#567 — Developmental Mentor Network canon and GUIDED-phase model roles
- The-Ethos-Project/cogni-core-0060a606#576 — specialist routing, quarantine, placeholder, and cognition-trace follow-up
- The-Ethos-Project/cogni-core#11 / PR The-Ethos-Project/cogni-core-0060a606#556 — protected inference readiness
- The-Ethos-Project/ethos-products#17 / PR The-Ethos-Project/cogni-core-0060a606#565 — provider-authority remediation
- PR The-Ethos-Project/cogni-core-0060a606#587 — model quarantine, unauthorized placeholder, bounded selection verdicts, and fail-closed generation-role foundation

## Canonical authority

All model consultation must remain subordinate to the existing cognitive authority chain:

`User input -> SignalRouter -> BrainOrchestrator -> canonical pipeline -> capability assessment -> governed mentor-role resolution -> InferenceGate -> llm_bridge -> mentor candidate evidence -> Cogni-Core synthesis -> product response`

Products, ErosOS, assistants, runtime capsules, and user-facing selectors must not directly choose, invoke, or execute mentor models.

Cogni-Core remains the sole cognitive and developmental authority. Mentor output is candidate evidence, not an independently authoritative answer and not automatically retained learning or training data.

## Goals

- Interpret user input into bounded capability demands.
- Route those demands to approved mentor roles rather than product-selected model identifiers.
- Wire the approved Developmental Mentor Network into the canonical inference path incrementally.
- Improve support for varied user inputs, including language, conversation, planning, vision, OCR, retrieval, reranking, safety review, voice, developmental critique, and bounded fallback behavior.
- Preserve hardware-aware, readiness-aware, and fail-closed selection.
- Provide interpretable model-selection and consultation traces without exposing an alternate executor.
- Preserve developmental accounting for mentorship interactions, dependence, handoff evidence, regression, and retirement.

## Required capabilities

### 1. Input capability assessment

Introduce or formalize a bounded assessment that identifies which approved capabilities an input requires, such as:

- language and conversation
- planning
- vision
- OCR
- retrieval
- reranking
- safety review
- voice processing
- developmental critique or evaluation
- executor support where already authorized
- bounded fallback

The assessment must preserve uncertainty and may return multiple candidate capabilities with confidence or evidence rather than pretending certainty.

### 2. Governed mentor-role resolution

Resolve capability demands to approved mentor roles through kernel-governed registry and readiness evidence.

Selection must consider:

- authorization state
- quarantine state
- role compatibility
- local availability
- hardware compatibility
- readiness state
- developmental phase constraints
- bounded fallback policy

Unknown, denied, quarantined, incompatible, or unavailable candidates must fail closed.

### 3. Canonical mentor consultation

Any mentor consultation must route through:

`BrainOrchestrator -> canonical pipeline -> InferenceGate -> llm_bridge`

No direct provider, Ollama, HTTP, SDK, subprocess, or application-layer inference path may be introduced.

### 4. Cogni-Core synthesis

Mentor responses must return as bounded candidate evidence to Cogni-Core. Cogni-Core must retain responsibility for evaluation, conflict handling, ethical review, synthesis, and final response generation.

### 5. Interpretability and trace lineage

Record bounded trace evidence including:

- interpreted capability demand
- candidate mentor roles considered
- authorization and readiness verdicts
- selected mentor role and model identifier where disclosure is permitted
- fallback or rejection reason
- consultation lineage
- return to Cogni-Core synthesis
- uncertainty and disagreement where applicable

Trace visibility must not leak secrets, hidden reasoning, unrestricted prompts, or raw internal state.

### 6. Developmental accounting

Separate ordinary inference from governed mentorship interactions.

Future slices must be able to distinguish:

- mentor consultation
- developmental critique
- developmental learning record
- mentor dependence
- native-node handoff evidence
- regression evidence
- mentor retirement eligibility

No automatic training, fine-tuning, memory retention, or autonomy advancement is authorized by this issue.

## Implementation strategy

Do not implement the complete mentor roster in one pull request. Break the work into small, reviewable slices, beginning with repository evidence and the stable runtime baseline.

Recommended slices:

1. capability taxonomy and input-demand contract
2. governed mentor-role resolver
3. readiness- and hardware-aware candidate selection
4. single-mentor consultation through InferenceGate
5. bounded multi-mentor consultation and disagreement handling
6. cognition-trace propagation
7. developmental interaction records
8. ErosOS trace presentation
9. specialist integrations for vision, OCR, retrieval, reranking, safety, voice, developmental critique, and fallback

Each slice requires its own explicit protected-surface approval when applicable.

## First gate

Before implementation:

- reassess the current canonical runtime and model-registry baseline from repository evidence;
- inventory the approved mentor roster and role assignments from `docs/engineering/MODEL_MENTORSHIP_PLAN.md`;
- identify existing capability-classification, routing, readiness, and trace surfaces;
- identify protected files and authority boundaries;
- document which mentor roles are currently wired, registry-only, readiness-only, or entirely unwired;
- produce a small task-intake breakdown for the first implementation slice.

## Constraints

- Preserve Kernel Space / User Space separation.
- Preserve BrainOrchestrator and canonical pipeline authority.
- Preserve InferenceGate as the sole authorized caller of `llm_bridge`.
- Preserve fail-closed model selection.
- No app-layer model execution.
- No product-selected mentor invocation.
- No duplicate executor, shadow pipeline, alternate router, or fallback generation path.
- No direct Ollama or provider access outside the approved kernel path.
- No model output may bypass ethics, executive review, or Cogni-Core synthesis.
- No autonomy phase advancement.
- No automatic learning or training retention.
- No broad protected-surface changes without a separate explicit approval gate.
- Preserve uncertainty and bounded evidence throughout selection and tracing.

## Acceptance criteria

- [ ] Current runtime, registry, readiness, and mentor-roster baseline is documented from repository evidence.
- [ ] Approved mentor roles are mapped to capability demands without making models user-selectable.
- [ ] A bounded input-capability contract is defined.
- [ ] Mentor-role selection is authorization-, readiness-, hardware-, and phase-aware.
- [ ] Unknown, denied, quarantined, incompatible, or unavailable candidates fail closed.
- [ ] All consultation routes through BrainOrchestrator, the canonical pipeline, InferenceGate, and `llm_bridge`.
- [ ] Cogni-Core remains responsible for synthesis and final response authority.
- [ ] Selection and consultation traces preserve causal lineage and uncertainty.
- [ ] No direct product, provider, HTTP, Ollama, SDK, or subprocess inference path is added.
- [ ] Developmental mentorship records remain distinct from ordinary inference and automatic learning.
- [ ] Work is split into small, independently reviewable implementation slices.
- [ ] Architecture Guard, LLM Bridge Guard, Brain Law, focused tests, and relevant regression suites pass for each slice.

## Explicit non-authorization

This issue does not itself authorize:

- a single large implementation PR;
- direct changes to protected ethics or kernel authority;
- replacing the canonical pipeline;
- direct product-to-model routing;
- user-selectable mentor execution;
- hosted fallback providers;
- unrestricted multi-agent execution;
- automatic memory or training retention;
- fine-tuning implementation;
- mentor retirement decisions;
- autonomy advancement;
- Eros Psyche cloud authorization.

## Relationship to The-Ethos-Project/cogni-core-0060a606#576

PR The-Ethos-Project/cogni-core-0060a606#587 partially addresses The-Ethos-Project/cogni-core-0060a606#576 by establishing quarantine, placeholder, bounded verdict, and fail-closed role foundations. Keep The-Ethos-Project/cogni-core-0060a606#576 open as the parent routing-and-trace track until its remaining specialist-routing and cognition-visibility requirements are decomposed or completed.
- **Scope:** GitHub Labels: None

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
