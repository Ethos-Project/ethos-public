# [ETHOS-NOMOS-027] Run inference-readiness cross-repository compatibility and promotion gate

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Goal

Run the terminal three-repository conformance, security, compatibility, rollback, publication, and runtime-promotion gate for `inference-readiness-status`.

Parent: The-Ethos-Project/cogni-core-0060a606#649

## Dependencies

- Reviewed Nomos contract candidate with an exact deterministic digest.
- Separately approved and reviewed Cogni-Core implementation.
- Separately approved and reviewed `ethos-products` client.
- Terminal native CI and zero unresolved contract review threads in all three repositories.

## Required evidence

- Accepted plus refused, unavailable, stale, incomplete, unsupported, and safe-error results.
- Every documented pre-envelope HTTP/TLS failure.
- Wrong certificate profile, claims, principal, process, executable/package digest, contract identifier, payload identifier, and Nomos digest.
- Expired/replayed requests, descriptor conflict, clock rollback, process exit, executable change, rotation overlap, revocation, ledger rollback, and audit-chain discontinuity.
- Deterministic package-digest equality from clean checkouts at recorded SHAs.
- Product limited mode and no-fallback proof.
- Rollback or withdrawal leaves the boundary unavailable unless a previously approved exact digest remains fully valid.

## Promotion record

Record:

- exact SHAs for `cogni-core`, `ethos-nomos`, and `ethos-products`;
- exact Nomos package digest and immutable compatibility identifiers;
- supported platform evidence;
- terminal CI, security-review, and review-thread state;
- credential, descriptor, audit, rollback, and withdrawal evidence; and
- explicit human approvals for publication and runtime promotion.

## Acceptance criteria

- All repositories independently reconstruct the same package digest.
- All mandatory evidence passes against that exact digest.
- Publication and `runtime_usable=true` occur only after explicit human approval.
- Any missing, unsupported, or failed evidence blocks promotion and leaves the boundary unavailable.

## Non-authorization

Creating this issue does not authorize implementation, publication, runtime enablement, credential provisioning, extraction, cutover, or bypass of any repository-local review gate.
- **Scope:** GitHub Labels: None | Original Repo: The-Ethos-Project/ethos-nomos

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
