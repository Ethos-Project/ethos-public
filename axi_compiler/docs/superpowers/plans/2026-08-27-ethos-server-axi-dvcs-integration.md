# Ethos Server and Axi DVCS Integration Plan

Status: Working plan; implementation requires human review  
Date: 2026-08-27  
Implementation owner: Antigravity  
Project management and documentation owner: Codex

## 1. Outcome

Create a truthful, authenticated, fail-closed Ethos Server boundary that can
exchange verified Axi DVCS objects and refs, coordinate human review through
Notion, and route model requests only through Psyche/Eros cognitive authority.

This plan does not authorize source edits, production deployment, real-store
initialization, credential creation, public release, or protected cognition
changes.

## 2. Current evidence

The focused local DVCS v1 currently has evidence for native build, local
content-addressed `init`, durable `wrap`, verified empty-destination `resume`,
path safety, and object identity. It does not yet establish Ethos Server sync,
real workspace initialization, remote ref mutation, Axi-native PR objects, or
the extended failure-injection matrix.

The current Ethos Server implementation requires review because it binds
broadly, uses wildcard CORS, exposes unauthenticated mutation, constructs
filesystem paths directly, contains conflicting WebSocket implementations, and
proxies provider-compatible requests outside the approved Psyche/Eros cognitive
authority route.

The canonical root `apps` scaffold is under development and review. Its current
server adapter contains simulated success paths that must be replaced by
truthful capability states only after the corresponding backend contracts are
implemented and verified.

## 3. Non-negotiable boundaries

- Axi DVCS is authoritative. Git/GitHub are optional mirrors.
- Notion is the official human coordination and approval surface.
- Ethics, kernel/user separation, interpretable signal routing, and causal
  lineage are structural requirements.
- Models and UI clients do not receive direct filesystem, ref-mutation, provider,
  or approval authority.
- All high-impact operations require authenticated human authorization and an
  auditable Axi operation receipt.
- Ethos Server transports governed requests; it does not become Eros or replace
  Eros identity and cognitive semantics.
- Unsupported capabilities fail closed and are reported as unavailable, never
  simulated as successful.

## 4. Target request path

```text
Root app / authorized client
  -> Ethos Server authentication and policy envelope
  -> versioned operation contract and correlation ID
  -> Psyche ingress
  -> Eros pre-inference cognitive/ethical gate
  -> bounded provider adapter
  -> Eros post-inference evaluation
  -> response plus causal lineage and operation receipt
```

DVCS operations follow a separate capability path:

```text
Authorized client
  -> Ethos Server DVCS adapter
  -> validate object/ref contract and permissions
  -> Axi local object/ref engine
  -> verify content identity and transition preconditions
  -> durable operation receipt
  -> Notion evidence link for human review when required
```

## 5. Phased work

### Phase 0 — Truth and dependency inventory

Deliverables:

- enumerate all Ethos Server routes, callers, hard-coded paths, write effects,
  provider paths, binaries, and WebSocket implementations;
- identify every simulated success response in `apps/`;
- write characterization tests for current behavior without endorsing it;
- classify compatibility requirements and protected surfaces;
- produce a decision request for authentication and canonical WebSocket design.

Exit: inventory and decision requests are in Notion and approved for the next
bounded phase. No runtime behavior changes.

### Phase 1 — Fail-closed server shell

Deliverables:

- loopback-only default binding with explicit configured exposure;
- origin allowlist and correct preflight behavior;
- bounded request parsing, method routing, body limits, and normalized paths;
- authentication/authorization interface with deny-by-default behavior;
- correlation IDs, structured errors, and append-only operation audit hooks;
- disable direct provider proxying and unauthenticated sandbox writes by default.

Exit: negative-path tests prove unauthorized access, traversal, malformed
requests, oversized bodies, and unsupported methods fail closed.

### Phase 2 — Versioned contracts

Define and review versioned schemas for:

- `OperationEnvelope` — actor, authority, intent, data class, correlation ID;
- `HumanReviewReference` — exact Notion item URL and decision state;
- `AxiObjectDescriptor` — algorithm, object ID, length, and verification state;
- `AxiRefTransition` — expected-old, proposed-new, actor, and reason;
- `SyncReceipt` — transferred objects, verification, ref result, and timestamps;
- `ProviderRequest` — Psyche/Eros policy context without raw authority leakage.

Exit: fixtures and compatibility tests are approved before adapters depend on
the schemas.

### Phase 3 — Read-only DVCS integration

Deliverables:

- capability/status endpoint backed by the real local engine;
- read-only object existence and verified transfer operations;
- reconciliation report for local, server, and UI state;
- no remote ref mutation or workspace initialization.

Exit: corrupt, truncated, trailing, wrong-identity, duplicate, and interrupted
transfer tests prove no false success and no ref change.

### Phase 4 — Governed ref mutation and review

Deliverables:

- compare-and-swap ref transition with rollback evidence;
- human-review reference required for consequential mutations;
- Axi-native change/review object integration when evidenced ready;
- optional GitHub PR mirror created only from an Axi checkpoint and linked
  back to the same Notion work item.

Exit: an authorized reviewer can approve, request changes, or reject; the agent
cannot self-approve; Axi remains authoritative if GitHub is unavailable.

### Phase 5 — Root apps integration

Deliverables:

- replace simulated DVCS/build/provider responses with contract-backed results;
- explicit `unavailable`, `unverified`, `blocked`, and `ready` capability states;
- show Axi receipt, exact Notion work item, and optional GitHub mirror links;
- preserve application source ownership under root `apps/`.

Exit: UI integration tests prove it cannot claim success without an associated
verified server response and receipt.

### Phase 6 — Security, recovery, and controlled rollout

Deliverables:

- authentication abuse, authorization, CORS, traversal, resource exhaustion,
  replay, concurrency, crash recovery, and audit-integrity tests;
- local-only pilot with test stores and non-production credentials;
- rollback and incident response procedure;
- explicit human production-readiness decision.

Exit: production remains disabled until the authorized human records approval.

## 6. Verification matrix

Every phase must include:

- positive, negative, and false-success regression tests;
- current-source build commands and exact outputs;
- changed-file inventory and contract diff;
- data-class and protected-surface review;
- Axi wrap/checkpoint or an explicit blocked receipt if no authoritative store
  has yet been approved;
- exact Notion work-item URL in every handoff;
- optional Git/GitHub links labeled as mirrors.

## 7. Human decisions required

1. Authentication authority and credential lifecycle.
2. Canonical WebSocket implementation and compatibility contract.
3. Whether and when to initialize the real Axi workspace store.
4. When Axi-native PR/review entities are ready for authoritative use.
5. GitHub mirror creation policy and publication boundaries.
6. Any provider-routing or protected Psyche/Eros change.
7. Production exposure and rollout.

