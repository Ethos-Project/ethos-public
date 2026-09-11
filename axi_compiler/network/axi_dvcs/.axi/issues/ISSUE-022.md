# [ISSUE-022] Add governed multi-channel retrieval evidence assembly

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
Parent umbrella: The-Ethos-Project/cogni-core#10

## Goal
Establish a bounded, fail-closed retrieval evidence assembly layer that supplements existing semantic and contextual memory with temporal, provenance/authority, constraint, contradiction, episodic-causal, active state/goal, and approved linguistic evidence.

All retrieved and assembled evidence must enter the approved Eros Brain authority path and unified geometric ethics field before it can influence candidate generation or final output.

## Repository
`thekalm0941/eros-brain`

## Base branch
`dev`

## Implementation branch
`codex/issue-596-governed-retrieval-evidence`

## Proposed PR title
`feat(memory): add governed retrieval evidence assembly contract`

## Architectural placement
- Memory formation and recall: `PRODUCTION/Limbic/Memory/` or the repository's canonical Hippocampus-equivalent package.
- Linguistic evidence: the approved in-process Linguistic Evidence Assembler (LEA) sensory-analysis boundary, once a repository-canonical implementation exists.
- Retrieval governance, ethical constraints, and executive review: existing Neocortex/Kernel/Executive authority path only.
- Candidate/action selection remains subordinate to the canonical `BrainOrchestrator → pipeline_orchestrator → InferenceGate → llm_bridge` path.
- Applications, ErosOS, Assistant, Dev Forge, and runtime capsules receive bounded Brain-owned results only.

## LEA contract alignment
The Linguistic Evidence Assembler is a subordinate, deterministic, in-process evidence producer. It is not a retriever, model router, executor, response generator, ethics authority, or alternate cognition path.

Where LEA evidence is available, this issue must consume it only through immutable, typed evidence contracts. The retrieval coordinator may use LEA outputs to improve query interpretation, ambiguity tracking, reference resolution, temporal/deictic anchoring, contradiction detection, and evidence traceability.

Required LEA semantics:

- original language signals remain preserved and unmodified;
- observed and inferred evidence remain distinct;
- inferred reconstructions use `INFERRED_NOT_ASSERTED` or the repository-equivalent status;
- competing interpretations remain explicit rather than prematurely collapsed;
- every linguistic evidence item preserves source span, module identity/version, triggered rules, upstream dependencies, confidence, and causal lineage;
- evidence identity is content-grounded and deterministic;
- full-record integrity uses a deterministic digest separate from semantic content identity;
- hashes are described as tamper-evident digests, never signatures or proof of immutability;
- LEA exposes a narrow declarative boundary equivalent to `assemble(signal, bounded_context) -> LinguisticEvidenceEnvelope`;
- the Brain never commands parser internals such as `SHIFT`, `LEFT_ARC`, or `RIGHT_ARC`;
- no FastAPI, gRPC, HTTP, subprocess, provider, model, or new service boundary is authorized for the initial implementation;
- no mutable global workspace or cross-request state is permitted;
- nested evidence payloads must be deeply immutable or canonically frozen before publication.

The Drive-side document `Master Engineering Specification: Linguistic Evidence Assembler (LEA) - Rev 1.1` is design input only. Repository canon and accepted implementation evidence remain authoritative.

## Scope
Implement the smallest safe foundation for multi-channel retrieval:

1. A typed `RetrievedEvidence` contract.
2. A typed retrieval request/context contract carrying signal lineage, active goal, temporal boundary, authorization context, and optional approved linguistic evidence references.
3. A Brain-owned retrieval coordinator that assembles evidence from approved channels without generating text or invoking a model.
4. Initial retrieval channels:
   - semantic/contextual compatibility adapter for existing memory retrieval,
   - temporal and canonical-status filtering,
   - provenance and authority classification,
   - mandatory constraint retrieval,
   - contradiction/conflict retrieval,
   - active state and goal context,
   - bounded adapter for existing LEA or capability-demand linguistic evidence, if already repository-canonical.
5. Deterministic evidence ranking and bounded result limits.
6. Fail-closed behavior for missing required governance/context evidence.
7. Cognition-trace visibility showing what evidence was retrieved, excluded, stale, contradicted, inferred, unresolved, or unavailable.

## Deferred follow-up channels
Do not implement these in the first PR unless they already exist and only need wiring into the contract:

- a full LEA parser implementation;
- morphology, dependency parsing, semantic-role labeling, discourse parsing, ellipsis engines, or modal-world evaluation;
- multimodal retrieval;
- affective/salience retrieval;
- counterfactual retrieval;
- external web/tool retrieval;
- autonomous memory consolidation;
- learned reranking;
- model-generated query expansion;
- new vector database or graph database dependencies;
- FastAPI, gRPC, HTTP, or standalone LEA services.

## Files allowed
Exact paths must be confirmed during repository inspection. Changes should remain narrowly bounded to:

- canonical Limbic/Hippocampus memory and retrieval modules;
- existing Brain signal/context contracts;
- existing executive/pipeline integration seam;
- cognition trace/evidence schema;
- an existing repository-canonical LEA evidence contract or a minimal adapter seam, if already present;
- focused unit and integration tests;
- one architecture document describing the retrieval evidence contract if required.

## Files and areas forbidden
- `PRODUCTION/Applications/**` cognition or generation shortcuts.
- Direct `llm_bridge` imports outside its approved kernel-layer caller.
- Direct Ollama/provider/model calls.
- New orchestrators, executors, agent loops, RAG response generators, or shadow pipelines.
- EthicsKernel, InferenceGate, BrainOrchestrator, or pipeline bypasses.
- Protected architecture changes unrelated to retrieval evidence assembly.
- Broad refactors, repository moves, worktrees, CI changes, dependency-provider additions, or branch deletion.
- OpenAI, Anthropic, Groq/FAST, or other hosted inference dependencies.
- New network servers, ports, service daemons, HTTP clients, gRPC clients, subprocess launchers, or mutable global LEA state.
- Reimplementation of parser algorithms solely to satisfy this issue.

## Core constraints
- Retrieval informs cognition; it is never a cognitive authority.
- LEA informs retrieval; it is never retrieval authority or cognitive authority.
- The coordinator must not generate prose, select final responses, call a model, execute tools, or mutate LEA evidence.
- Constraint/governance retrieval occurs before ordinary relevance retrieval can influence candidate generation.
- Canonical and current evidence outranks stale, historical, generated, inferred-only, or unverified material.
- Every evidence item preserves provenance, authority, temporal validity, epistemic type, confidence, commitment status, and causal lineage.
- `OBSERVED`, `INFERRED_NOT_ASSERTED`, and `REJECTED` evidence must remain distinguishable.
- Contradictory evidence must be retained and surfaced, not silently discarded.
- Ambiguous linguistic evidence must remain unresolved unless downstream authority has sufficient corroboration.
- Required governance evidence missing or indeterminate must produce a bounded unavailable/fail-closed result.
- Existing capability-demand taxonomy for retrieval/search/memory lookup and missing-context handling must be reused or extended, not duplicated.
- Ollama/local inference remains the default, but this issue must not invoke inference.
- Brain Health, `architecture_guard`, and `brain_law` remain critical gates.

## Required contracts

### Retrieved evidence
At minimum:

```python
@dataclass(frozen=True)
class RetrievedEvidence:
    evidence_id: str
    content_ref: str
    retrieval_channel: str
    source_id: str
    provenance: str
    authority_level: int
    relevance_score: float
    confidence: float
    valid_from: datetime | None
    valid_until: datetime | None
    canonical_status: str
    epistemic_type: str
    commitment_status: str
    source_span: tuple[int, int] | None
    module_id: str | None
    module_version: str | None
    contradiction_ids: tuple[str, ...]
    causal_parent_ids: tuple[str, ...]
    exclusion_reason: str | None = None
```

The implementation may adapt field names to existing repository conventions, but must preserve these semantics.

### Optional linguistic evidence reference
When repository-canonical LEA evidence exists, retrieval should consume references or immutable projections rather than duplicate the full envelope:

```python
@dataclass(frozen=True)
class LinguisticEvidenceRef:
    evidence_id: str
    evidence_kind: str
    commitment_status: str
    confidence: float
    source_span: tuple[int, int]
    module_id: str
    module_version: str
    causal_parent_ids: tuple[str, ...]
```

### Evidence assembly result
Must distinguish at least:

- `AVAILABLE`
- `PARTIAL`
- `UNAVAILABLE`
- `BLOCKED`

and include:

- selected evidence;
- excluded/stale evidence metadata;
- unresolved contradictions;
- unresolved linguistic ambiguities;
- inferred-only evidence counts;
- missing required context;
- bounded trace summary;
- causal lineage back to the originating signal.

## Tasks
1. Inspect existing semantic memory, contextual memory, Hippocampus, signal context, cognition trace, capability-demand, and any repository-canonical linguistic evidence contracts.
2. Document the current canonical retrieval call path before editing.
3. Identify and reuse the existing retrieval/search/memory capability-demand taxonomy introduced by the recent capability-demand work.
4. Determine whether an LEA contract exists in repository canon. If not, do not implement the full LEA here; define only the narrowest compatible optional seam or defer it explicitly.
5. Add the typed evidence and assembly-result contracts.
6. Add a single Brain-owned coordinator with deterministic channel ordering:
   1. governance and constraints,
   2. active state/goal,
   3. temporal/canonical filtering,
   4. approved linguistic evidence projection,
   5. semantic/contextual retrieval,
   6. provenance/authority scoring,
   7. contradiction and ambiguity preservation,
   8. bounded evidence packaging.
7. Integrate the coordinator at one approved pre-inference seam only.
8. Preserve compatibility with existing semantic/contextual retrieval behavior.
9. Add trace metadata without exposing private memory content unnecessarily.
10. Ensure observed, inferred, rejected, stale, and unresolved states remain distinguishable.
11. Add focused tests.
12. Update only the minimum architecture documentation needed to identify the canonical retrieval path and invariants.

## Acceptance criteria
- [ ] One canonical retrieval coordinator exists.
- [ ] No alternate response generator, executor, orchestrator, model call, or tool call is introduced.
- [ ] Existing semantic/contextual memory is consumed through an adapter or existing interface rather than duplicated.
- [ ] Governance and mandatory constraints are assembled before ordinary relevance evidence.
- [ ] Temporal validity and canonical status can exclude stale/non-canonical evidence deterministically.
- [ ] Provenance and authority metadata are present on every selected evidence item.
- [ ] Contradictory evidence is represented explicitly.
- [ ] Linguistic ambiguity is retained explicitly when unresolved.
- [ ] `OBSERVED`, `INFERRED_NOT_ASSERTED`, and `REJECTED` evidence remain distinguishable.
- [ ] LEA evidence, if used, is consumed through a bounded immutable projection or reference contract.
- [ ] No Brain control of parser-internal transition actions is introduced.
- [ ] No network/service boundary is introduced for LEA or retrieval.
- [ ] No mutable global workspace or cross-request evidence state is introduced.
- [ ] Missing required governance/context returns `UNAVAILABLE` or `BLOCKED` fail-closed state.
- [ ] Evidence counts and payload sizes are bounded.
- [ ] Signal/cognition causal lineage is preserved.
- [ ] Applications and product layers cannot access internal retrievers or LEA internals directly.
- [ ] No direct `llm_bridge`, Ollama, provider, or hosted-model path exists.
- [ ] Trace output identifies channels, counts, status, exclusions, inference status, ambiguity, and contradiction state without leaking sensitive content by default.
- [ ] Existing retrieval behavior remains compatible unless explicitly documented.

## Required tests
- Unit tests for evidence contract validation and deep immutability.
- Deterministic content-ID and full-record digest tests where those concepts are implemented.
- Test proving inferred linguistic evidence is never relabeled as observed.
- Test proving unresolved ambiguity is retained rather than silently collapsed.
- Deterministic ordering/ranking tests.
- Temporal expiry and supersession tests.
- Canonical-versus-historical authority tests.
- Contradiction preservation tests.
- Missing mandatory constraint/context fail-closed tests.
- Bounded result-count/payload tests.
- Causal-lineage preservation tests.
- Integration test proving retrieval occurs through the canonical Brain pipeline before inference.
- Architecture test proving no product-layer retriever or LEA-internal access.
- Architecture test proving no new direct `llm_bridge`, Ollama, HTTP, gRPC, subprocess, or provider path.
- Regression tests for existing semantic/contextual memory retrieval.

## Checks
Run the repository-canonical commands discovered from current documentation. At minimum, where available:

```text
pytest <focused retrieval/memory tests>
pytest <canonical integration test subset>
python tools/architecture_guard.py
python tools/llm_bridge_guard.py
python tools/brain_law.py
```

Also run Brain Health using the current canonical command. Do not regenerate or overwrite tracked evidence unless the repository workflow explicitly requires it.

## Commit message
`feat(memory): add governed retrieval evidence assembly contract`

## PR body

```markdown
## Summary
- adds a typed, Brain-owned retrieval evidence assembly contract
- integrates semantic/contextual memory with temporal, provenance, constraint, contradiction, state/goal, and approved linguistic evidence
- preserves ambiguity, observed/inferred distinctions, fail-closed governance, and the single Brain authority path

## Architecture invariants
- no model or provider calls
- no new executor/orchestrator/generation path
- no Applications-layer retrieval or LEA authority
- no network/service boundary or mutable global LEA state
- all evidence remains subordinate to BrainOrchestrator, the ethics field, InferenceGate, and canonical inference routing

## Validation
- [ ] focused tests
- [ ] canonical integration tests
- [ ] architecture_guard
- [ ] llm_bridge_guard
- [ ] brain_law
- [ ] Brain Health

Closes The-Ethos-Project/cogni-core#22
```

## Stop conditions
Stop without broadening scope if any of the following is encountered:

- more than one plausible canonical pre-inference integration seam;
- an existing retrieval coordinator already owns this responsibility;
- implementation requires changing EthicsKernel or InferenceGate semantics;
- implementation requires product-layer access to memory or LEA internals;
- implementation requires a new database/provider/model/network dependency;
- implementation would require building the full LEA parser rather than consuming an existing contract;
- capability-demand taxonomy cannot be reused without architectural duplication;
- observed and inferred evidence cannot remain structurally distinct;
- immutable payloads or deterministic serialization cannot be preserved using existing repository conventions;
- Brain Health, architecture guard, or brain law baseline is already failing for unrelated reasons;
- the working tree is not clean or `main` is not synchronized with `origin/main`;
- protected architecture behavior is ambiguous.

Record evidence and return for architectural review rather than guessing.

## Handoff requirements
The implementation handoff must report:

- branch and commit SHA;
- exact canonical retrieval path before and after;
- files changed;
- tests/checks and outcomes;
- evidence that no inference/tool/network bypass exists;
- evidence that existing capability-demand taxonomy was reused;
- whether repository-canonical LEA contracts existed and how they were consumed or deferred;
- evidence that observed, inferred, rejected, stale, contradictory, and unresolved states remain distinguishable;
- any deferred channels or unresolved contradictions;
- PR URL;
- clean working-tree status.
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
