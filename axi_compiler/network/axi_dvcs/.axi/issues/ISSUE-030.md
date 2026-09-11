# [ISSUE-030] Implement protected inference-readiness endpoint and authentication conformance

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Goal

Implement and prove the protected Cogni-Core side of the approved `inference-readiness-status` endpoint and authentication design against one exact, separately reviewed Nomos contract-candidate digest.

Parent: The-Ethos-Project/cogni-core-0060a606#649

## Dependencies

- Normative documentation: The-Ethos-Project/cogni-core-0060a606#803, The-Ethos-Project/cogni-core-0060a606#804, The-Ethos-Project/cogni-core-0060a606#805, and The-Ethos-Project/cogni-core-0060a606#806.
- Blocked on the Nomos contract-candidate issue and its reviewed exact digest.
- Requires fresh explicit protected-runtime approval before implementation starts.

## Proposed review slices

1. Listener and transport shell for the exact route, loopback-only dynamic binding, TLS/HTTP profile, bounded framing, and pre-envelope rejection matrix. The route remains side-effect free and performs no inference initialization or provider probing.
2. Installation-local credential issuance, discovery publication, X.509 profiles, live service/process/principal attestation, and fail-closed platform checks.
3. Replay, clock, expiry, renewal, rotation, revocation-ledger, descriptor lifecycle, and append-only audit-chain enforcement.
4. Readiness-state mapping, safe result envelopes, exact candidate-digest pinning, protected conformance tests, and no-bypass proof.

Each slice must remain small, reviewable, and on its own explicit approval gate.

## Acceptance criteria

- The only route is `POST /api/capabilities/inference-readiness-status`.
- Binding, discovery, mTLS, certificate claims, process attestation, expiry, replay, rotation, revocation, audit/redaction, and wire rejection behavior match the approved conformance specification exactly.
- Unsupported platforms or unverifiable evidence remain unavailable or refused; no weaker fallback is introduced.
- The route never initializes, selects, recovers, or calls an inference backend.
- Cogni-Core pins the exact reviewed Nomos candidate digest but does not mark it runtime-usable before promotion.
- Cogni-Core-owned contract, security, audit-chain, and no-bypass tests pass.

## Non-authorization

Creating this issue does not authorize implementation. It does not authorize a fixed port, cleartext fallback, bearer credentials, browser access, public/LAN binding, alternate cognition, package publication, runtime enablement, Product implementation, CI/dependency changes, or extraction.
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
