# [ISSUE-025] Architect governed execution fabric using biological motor-control mapping

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
Parent umbrella: The-Ethos-Project/ethos-nomos#22



## Goal

Define and track a biologically mapped, safety-governed execution architecture for Eros, using the self-hosted CI runner pattern as the first practical precursor.



Eros must not gain a generic runner subsystem or a second cognitive authority. External execution is an embodied motor act whose full control loop is distributed across the appropriate brain regions and remains subordinate to the canonical BrainOrchestrator, unified geometric ethics field, safety systems, executive review, and InferenceGate.



## Architectural premise

The external worker is analogous to muscle/peripheral tissue, not another brain. The complete execution loop should map as follows:



- **Neocortex / Executive / motor planning** — intent formation, action planning, risk envelope, completion criteria, human-approval requirements.

- **Subcortical / Basal Ganglia** — approved action selection, inhibition, capability matching, developmental permission enforcement, duplicate-action suppression.

- **Subcortical / Cerebellum** — sequencing, timing, expected-versus-actual comparison, bounded correction signals, procedural refinement.

- **Subcortical / Brainstem** — controlled command relay, watchdogs, heartbeat, timeout, emergency stop, safe shutdown.

- **Eros / Body / Peripheral Nervous System** — node identity, capability manifests, transport, sensory return, result provenance.

- **Applications / Execution Nodes** — bounded operating-system worker that performs approved tasks and returns evidence.



## Canonical signal path



```text

User or system intent

  -> Applications/Sensory Relay

  -> canonical BrainOrchestrator

  -> Neocortex safety + unified ethics field

  -> Executive motor planning

  -> Basal Ganglia action selection/inhibition

  -> Cerebellar sequencing/monitoring

  -> Brainstem controlled relay

  -> Peripheral execution protocol

  -> external execution node

  -> sensory evidence return

  -> thalamic/canonical ingress

  -> safety/anomaly assessment

  -> expected-vs-actual comparison

  -> executive interpretation

  -> governed memory/procedural learning

```



## Non-negotiable constraints



- No direct application-to-runner control path.

- No runner-to-LLM or runner-to-`llm_bridge` path.

- No independent planning, self-authorization, or final-response generation by execution nodes.

- No ethics kernel, InferenceGate, BrainOrchestrator, or unified field bypass.

- No duplicate executor or shadow orchestration pipeline.

- No arbitrary shell autonomy.

- All tasks require explicit causal lineage, authorization evidence, capability bounds, timeout, result provenance, and revocation/emergency-stop semantics.

- Ollama/local inference policy remains unchanged.

- Brain Health, `architecture_guard`, and `brain_law` remain protected.



## Developmental authority model



- **DEPENDENT** — human approval for every dispatched task.

- **GUIDED** — only preapproved low-risk routines may execute without per-step approval; consequential work still requires approval.

- **SEMI_AUTONOMOUS** — bounded diagnostics and maintenance may be scheduled within explicit policy.

- **AUTONOMOUS** — broader execution remains constrained by immutable kernel authority and complete traceability.



## First practical precursor

Use a hardened self-hosted GitHub Actions runner to restore CI capacity while preserving GitHub as the canonical repository, PR, issue, and branch-governance surface.



This runner is infrastructure only. It may inform the later Eros execution-node protocol, but it must not initially share privileged brain-runtime code or be represented as Eros cognition.



## Planning tasks



- [ ] Inventory current brain map and identify existing modules that already implement portions of motor planning, action selection, sequencing, relay, peripheral transport, and sensory return.

- [ ] Produce a canonical placement decision for each responsibility.

- [ ] Define an explicit governed task-envelope schema.

- [ ] Define execution-result/evidence schema with task identity, authorization lineage, node identity, timestamps, exit states, evidence hashes, policy events, confidence, and uncertainty.

- [ ] Define capability registration and node-attestation requirements.

- [ ] Define developmental-phase authorization rules.

- [ ] Define action inhibition, duplicate suppression, timeout, retry, revocation, and emergency-stop semantics.

- [ ] Define the canonical sensory return path; results must not be injected directly into app responses or raw LLM context.

- [ ] Define trust boundaries between brain code, peripheral protocol, runner service, repository workspace, credentials, and host operating system.

- [ ] Define how the initial self-hosted CI runner remains operationally isolated from future Eros-native execution nodes.

- [ ] Add architecture-law language describing external execution as an embodied motor act.

- [ ] Identify required follow-up implementation issues; do not implement the generalized fabric in this issue.



## Acceptance criteria



- A reviewed architecture plan maps every execution responsibility to the correct biological brain/body region.

- The plan preserves one canonical cognitive authority and prohibits all runner, application, ethics, inference, and orchestration bypasses.

- The task and result schemas preserve causal lineage and structural interpretability.

- The developmental authority model is explicit and enforceable.

- The self-hosted GitHub runner is clearly separated from the future Eros-native execution fabric.

- Follow-up implementation work is decomposed into narrow, ordered issues with explicit safety gates.



## Scope boundary

Planning and architecture definition first. No broad refactor, no runtime extraction, no CI redesign, no protected architecture changes, and no generalized execution implementation until the mapping and governance plan are approved.
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
