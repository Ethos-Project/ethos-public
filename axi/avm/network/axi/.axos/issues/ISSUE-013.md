# [ISSUE-013] Epic: Demonstrate end-to-end safety-governed Eros cognition after stable ErosOS baseline

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Purpose

Track the **canonical first post–stable-ErosOS-baseline implementation epic**.

This proof of concept must demonstrate that a useful AI response can be generated through the single approved Eros Brain authority path, with ethical evaluation, uncertainty, memory, action selection, inference, and causal lineage remaining structurally governed and inspectable.

This is a concrete implementation epic, not a standalone roadmap root and not a child of the one-person execution-plan issue.

## Roadmap hierarchy

```text
The-Ethos-Project/ethos-nomos#21 — Master roadmap / strategic atlas
└── The-Ethos-Project/cogni-core#10 — Complete Eros local functional baseline
    └── Stable ErosOS baseline issue family
        └── The-Ethos-Project/cogni-core#13 — End-to-end governed cognition PoC epic
```

Product-side context:

```text
The-Ethos-Project/cogni-core-0060a606#559 — ErosOS platform vision and sequencing
└── Stable ErosOS implementation track
    └── The-Ethos-Project/cogni-core#13 — Governed cognition PoC exposed through ErosOS
```

Sequencing lens:

```text
The-Ethos-Project/cogni-core-0060a606#523 — One-person execution-plan and backlog-compression lens
└── governs when The-Ethos-Project/cogni-core#13 becomes active
    but does not architecturally own The-Ethos-Project/cogni-core#13
```

## Roadmap position

**Priority:** First implementation epic after stable ErosOS baseline closure.

**Hard dependency:** Do not activate implementation until the canonical ErosOS stability/baseline gate is formally complete.

**Sequence:**

```text
Stable ErosOS baseline
→ The-Ethos-Project/cogni-core#13 End-to-end governed cognition PoC epic
→ broader product, research, commercialization, and autonomy tracks
```

This epic outranks other post-baseline implementation candidates unless a critical regression, security defect, governance drift, or failing protected check must be resolved first.

## PoC objective

Build a bounded, locally runnable Eros personal-assistant demonstration that:

1. receives a natural-language request;
2. converts it into the canonical cognitive signal;
3. routes it through `BrainOrchestrator` and the canonical pipeline;
4. performs safety and geometric ethical evaluation before inference;
5. retrieves bounded local memory with provenance;
6. performs governed action selection and executive review;
7. reaches inference only through `InferenceGate → llm_bridge → approved Ollama model`;
8. records a structured cognition trace with causal lineage;
9. fails closed when any protected component is unavailable, unauthorized, or denies the action.

## Canonical authority path

```text
ErosOS / Personal Assistant
→ Sensory Relay / SignalRouter
→ BrainOrchestrator
→ pipeline_orchestrator
→ Safety + Geometric Ethics Field
→ Limbic context and Hippocampal retrieval
→ Subcortical candidate action selection
→ Neocortical executive review
→ InferenceGate
→ llm_bridge
→ approved local Ollama model
→ governed output review
```

No application, runtime capsule, utility, ErosOS surface, Personal Assistant component, or DevForge component may call Ollama, `llm_bridge`, or another language model directly.

## Required demonstration scenarios

- **Normal cognition:** benign request completes through the canonical route.
- **Ethical tension:** exploitative or harmful request is blocked or redirected before ordinary generation.
- **Uncertainty preservation:** unresolved claims are represented without false certainty.
- **Memory-grounded cognition:** bounded local memory influences the result with provenance.
- **Fail-closed operation:** unavailable ethics, inference, model authorization, or Brain authority produces no bypass and no ordinary generated response.

## Minimum scope

### In scope

- one ErosOS or Personal Assistant user surface;
- canonical signal schema;
- process-scoped `BrainOrchestrator`;
- geometric ethics evaluation;
- threat/safety assessment;
- bounded memory retrieval;
- governed action classes;
- executive review;
- approved-model selection;
- `InferenceGate` and kernel-layer `llm_bridge`;
- local Ollama inference;
- structured trace recording and trace viewer;
- Brain Health, architecture guard, brain law, and automated tests.

### Out of scope

- autonomous real-world actions;
- unrestricted tools;
- financial, medical, or legal decision authority;
- self-modifying kernel behavior;
- developmental phase advancement;
- distributed multi-agent autonomy;
- cloud cognitive authority;
- robotics or embodiment;
- consciousness certification claims;
- production-scale deployment;
- broad refactors, repo extraction, CI redesign, or alternate executors.

## Epic decomposition rule

Do not create child implementation issues until the stable ErosOS baseline is formally closed and this epic is activated.

At activation, decompose this epic only as needed into focused child issues such as:

- canonical demonstration signal and scenario harness;
- pre-inference ethics and denial-path validation;
- bounded memory provenance demonstration;
- structured cognition-trace contract;
- ErosOS trace viewer;
- fail-closed readiness and authorization tests;
- PoC evidence package and reproduction guide.

Each child must remain subordinate to The-Ethos-Project/cogni-core#13 and must not create a duplicate executor, app-layer inference path, shadow pipeline, or protected-architecture bypass.

## Success criteria

### Architecture

- [ ] 100% of demonstrated cognition enters through the canonical Brain authority.
- [ ] Ethics and safety evaluation occur before protected inference.
- [ ] Denied verdicts prevent ordinary generation.
- [ ] No application-layer import or invocation of `llm_bridge` exists.
- [ ] No direct application/runtime Ollama request exists.
- [ ] No alternate executor can generate the final answer.
- [ ] Exactly one authorized final-generation route exists.
- [ ] Model authorization and readiness fail closed.

### Functional

- [ ] Benign requests produce useful governed responses.
- [ ] Harmful requests are blocked or redirected.
- [ ] Uncertainty is explicitly preserved.
- [ ] Relevant local memory can ground a response.
- [ ] Infrastructure failure produces a structured unavailable/blocked result.
- [ ] Every response attempt receives a trace ID.

### Interpretability

- [ ] Structured trace includes route, verdict, uncertainty, memory references, action class, model authorization, inference count, and final status.
- [ ] Trace exposes evidence and causal decisions without exposing private chain-of-thought.
- [ ] Blocked and degraded paths remain inspectable.

### Verification

- [ ] Focused routing and denial tests pass.
- [ ] Tests prove no direct Ollama or app-layer `llm_bridge` path.
- [ ] Tests prove no duplicate final executor.
- [ ] Tests prove one authorized inference call on the successful path.
- [ ] Brain Health passes.
- [ ] `architecture_guard` passes.
- [ ] `brain_law` passes.
- [ ] Canonical test suite passes.

## Deliverables

- runnable local demonstration;
- architecture diagram with protected boundaries;
- five scripted demonstration scenarios;
- user and developer cognition-trace views;
- automated verification suite;
- evidence report containing commit, environment, model manifest, checks, limitations, and demonstration artifacts;
- reproduction instructions.

## Activation gate

Implementation may begin only when the stable ErosOS baseline owner/anchor records all required closure evidence and no unresolved blocker invalidates that baseline.

At activation, create a focused branch using:

```text
codex/issue-597-governed-cognition-poc
```

Follow the standard branch → focused change → checks → commit → PR workflow. Do not create worktrees, duplicate pipelines, direct inference shortcuts, broad refactors, CI changes, repo moves, or protected architecture bypasses.

## Stop conditions

Stop and escalate rather than implement around the problem if:

- the canonical Brain authority path is ambiguous;
- a required component can only be reached through a bypass;
- the change would create a second executor or generation path;
- an application must import `llm_bridge`;
- Ollama would be invoked outside the protected inference path;
- Brain Health, architecture guard, or brain law fails;
- the ErosOS baseline has not actually reached its formal stable closure gate.

## Roadmap relationships

- **#320 — master roadmap / strategic atlas:** highest-level roadmap owner.
- **#322 — local functional baseline anchor:** baseline umbrella above the stable ErosOS family.
- **Stable ErosOS baseline family:** direct prerequisite and operational parent context.
- **#559 — ErosOS platform vision:** product-side sequencing and interface context.
- **#523 — one-person execution plan:** sequencing and backlog-compression lens only; not the parent or architectural owner of this epic.

This issue is the canonical first post–stable-ErosOS-baseline implementation epic.
- **Scope:** GitHub Labels: None

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
