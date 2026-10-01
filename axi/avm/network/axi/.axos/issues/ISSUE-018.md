# [ISSUE-018] Discovery: close Cogni external ingress and MCP connector boundary

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Current goal

Close the protected Cogni boundary required before canonical cutover and before Eros MCP hosting can move from the unified source into Nomos without direct protected imports.

This issue scopes discovery and review gates. It does not authorize protected implementation, contract publication, `runtime_usable` promotion, public release, source cleanup, or canonical cutover.

## Confirmed repository facts

- The approved P2 private byte-copy scope is complete across all five official destinations.
- The broader manifests retain 420 absent paths by design; they are refactor, contract, authority, evidence, process, security, connector, or missing-source decisions rather than unconditional copies.
- Nine Cogni-owned files still contain cross-stratum Product or Nomos coupling.
- Fourteen `eros_mcp/` paths are blocked from Nomos because they directly import or instantiate protected ethics, memory, continuity, drive, inference, and routing implementations.
- The Nomos `product-cogni-core` package remains `unpublished`, `unreleased`, and `runtime_usable: false`.
- `SignalRouter.mcp_invoke()` is a Product-side client of the current MCP HTTP server; it is not a safe external Cogni ingress for a Nomos-hosted connector.
- Cogni already contains a dormant first-release ingress stack under `eros/runtime/product_ingress_*`: dynamic IPv4 loopback binding, TLS 1.3/mTLS enforcement, authenticated service/process/principal evidence, signed discovery, lifecycle/replay/audit contracts, bounded HTTP framing, and an unpublished-contract evaluator.
- That stack is present in the official Cogni destination, intentionally absent from runtime boot, and currently returns `unavailable` without calling readiness or cognition because the Nomos package is unreleased.
- No focused test covers `process_ingress_connection()` as a complete pipeline; activation, credential provisioning, listener ownership, and Nomos consumption remain unimplemented.

## Exact protected files in discovery scope

1. `eros/Production/Core/llm_bridge.py`
2. `eros/Production/Limbic/Homeostasis/drive_tools.py`
3. `eros/Production/Limbic/Memory_Formation/memory_tools.py`
4. `eros/Production/Limbic/Memory_Formation/mongo_memory_importer.py`
5. `eros/Production/Neocortex/Geometric_Ethics_Field/ethics_tools.py`
6. `eros/interfaces/SignalRouter.py`
7. `eros/interfaces/kernel_singleton.py`
8. `eros/interfaces/product_entrypoints.py`
9. `eros/runtime/embodiments.py`

## Exact blocked contract-test replacements

1. `tests/architecture/test_enforce_architecture.py`
2. `tests/architecture/test_hmc_driver_system_compatibility.py`
3. `tests/architecture/test_product_ingress_lifecycle_boundary.py`
4. `tests/integration/test_assistant_result.py`
5. `tests/integration/test_integration_wiring.py`
6. `tests/integration/test_principal_memory.py`
7. `tests/integration/test_unified_runtime_path_smoke.py`

## Required invariants

- Ethics remains structural and inside Cogni authority.
- Kernel/User separation remains intact.
- Models remain instruments and cannot be called outside the approved cognition pipeline.
- Nomos/MCP remains transport and capability exposure only; it cannot become cognition, command authority, policy authority, model router, memory owner, or identity owner.
- Products, connectors, and lab callers cannot import protected Cogni implementation modules.
- Every external request carries explicit service, process, principal, capability, operation, trace, causal-lineage, audit, provenance, freshness, and redaction claims.
- Identity, approval, capability, compatibility, payload, and lineage validation fail closed.
- Transport loss returns unavailable and never enables a provider, cache, direct module, shared database, local daemon, or alternate executor fallback.
- Existing Cogni architecture, LLM bridge, Brain Law, health, and no-bypass guards remain blocking.

## Proposed task breakdown

### A. Separate Cogni capability authority from MCP registration

Define Cogni-owned typed capability ports. MCP registration decorators, request parsing, and process hosting remain outside protected Cogni modules.

### B. Complete the existing dormant external Cogni ingress

Use the already-reviewed `eros/runtime/product_ingress_*` loopback TLS/mTLS, identity/attestation, discovery, lifecycle, and contract-evaluation stack as the current authority. First add complete handler tests and close fail-closed response/lifecycle gaps. Then separately approve runtime boot ownership, credential provisioning, listener activation, and Nomos consumption. Do not repurpose the current MCP HTTP server or `SignalRouter.mcp_invoke()` as Cogni ingress.

### C. Invert Product dependencies

Replace Product imports in `SignalRouter`, `kernel_singleton`, `product_entrypoints`, `embodiments`, Mongo credential loading, and the LLM bridge allowlist with Cogni-owned ports or caller-supplied adapters. Product implementations remain in `ethos-products`.

### D. Rewrite destination-native contract tests

Prove stable ingress, caller identity distinctions, allowlisted capabilities, protected-import rejection, provider isolation, no fallback, exact refusal/unavailable semantics, audit and causal-lineage preservation, and lifecycle behavior without Product imports.

### E. Implement the Nomos connector only after A-D are reviewed

Author a Nomos-hosted MCP adapter against approved ingress. Do not raw-copy the existing 14 connector files. Keep the contract unpublished and `runtime_usable: false` until Cogni, Nomos, and Product compatibility evidence passes a separate promotion gate.

## Remaining unknowns

- Which current MCP capabilities remain externally exposeable versus become internal-only; only `inference-readiness-status` has an existing contract candidate.
- The production credential/issuer store, certificate provisioning, rotation, revocation, and local process-attestation implementation for the existing mTLS profile.
- The Cogni runtime component that owns listener startup/shutdown and signed descriptor publication without becoming a second orchestrator.
- Whether Product launch/credential configuration is injected at construction or provided through a separate Product-owned adapter service.
- The compatibility promotion, package signing, and three-repository attestation gate that moves the existing package from `unpublished` / `runtime_usable: false`.
- Which handler/evaluator gaps must close before activation; the first release remains inference-readiness only and may not include routed cognition.

## Risk assessment

**Critical / protected.** This lane touches inference, routing, memory, identity-adjacent behavior, ethics tooling, model access, process boundaries, and connector exposure. A broad rewrite or transport shortcut could create a second cognition path or leak protected authority into Nomos.

## Validation and rollback gates

- One protected implementation PR at a time from current canonical `main`.
- Declare the exact changed-file list and tests before each slice.
- Ruff, focused tests, Architecture Guard, LLM Bridge Guard, Brain Law, health/completeness, analyzer, and grouped native CI must pass.
- Re-poll GraphQL `reviewThreads`, reviews, and comments after every push.
- No merge without explicit maintainer approval and exact-head verification.
- Roll back by reverting the focused PR; keep the unified source and private destinations intact until cross-repository validation and separate cutover approval.

## Approval gate

Maintainer approval is required for the first protected implementation slice. The recommended slice is limited to completing and testing the existing dormant, fail-closed `inference-readiness-status` ingress pipeline: `eros/runtime/product_ingress_handler.py`, a new focused handler test module, and only the minimal transport/evaluator corrections those tests prove necessary. It must remain disconnected from runtime boot, keep the contract unpublished and `runtime_usable: false`, and expose no routed cognition. Listener activation, credential provisioning, Nomos MCP hosting, and compatibility promotion remain later explicit gates.
- **Scope:** GitHub Labels: bug

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
