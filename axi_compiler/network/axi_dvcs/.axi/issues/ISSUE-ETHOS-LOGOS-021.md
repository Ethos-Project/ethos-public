# [ETHOS-LOGOS-021] Remediate audited non-canon documentation and references

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
Parent umbrella: The-Ethos-Project/cogni-core#10, Blocked by The-Ethos-Project/cogni-core-0060a606#591

## Goal

Apply the bounded remediation inventory produced by the repository-wide canon residue audit and ensure active documentation and references clearly conform to current Eros canon.

## Dependency

Blocked by The-Ethos-Project/cogni-core-0060a606#591.

Do not begin implementation until the audit is merged, reviewed, and each proposed disposition has sufficient evidence.

## Scope

Using the accepted findings from The-Ethos-Project/cogni-core-0060a606#591 only:

- repair stale terminology in active documentation;
- replace obsolete repository names and paths where current behavior or instructions depend on them;
- convert superseded active documents into concise pointer documents where appropriate;
- move truly historical material into an approved historical/archive location when explicitly authorized;
- add clear historical, deprecated, quarantined, or non-authoritative banners where retention is required;
- regenerate approved generated documents when their source tooling is already in scope and no tooling change is required;
- update `docs/DOCS_INDEX.md` so authoritative, supporting, historical, superseded, and diagnostic documents are accurately classified;
- remove or repair active cross-references that incorrectly promote non-canon material.

## Non-scope

- No new architecture or governance doctrine.
- No runtime behavior changes.
- No product behavior changes.
- No tests, CI, dependencies, imports, Brain Health scoring, Brain Law, architecture guard, inference routing, or protected Brain/kernel/ethics changes.
- No broad repository restructuring.
- No Five Strata extraction or repository moves.
- No worktrees.
- No blind global search-and-replace.
- No deletion of historical evidence without explicit human authorization.

## Files allowed

Only files explicitly classified as **stale requiring remediation** in the merged The-Ethos-Project/cogni-core-0060a606#591 audit, plus narrowly necessary index or superseded-pointer files identified by that audit.

## Files forbidden

Unless a separate issue explicitly authorizes them:

- `eros/Production/**`
- runtime, inference, ethics, BrainOrchestrator, InferenceGate, or `llm_bridge` files
- `.github/workflows/**`
- dependency manifests and lockfiles
- tests
- Brain Health, Brain Law, and architecture-guard implementation files
- unrelated product code

## Tasks

1. Read the merged The-Ethos-Project/cogni-core-0060a606#591 audit and build an exact allowed-file inventory.
2. Re-verify each target against current `main` before editing.
3. Apply the recorded disposition without expanding scope.
4. Preserve legitimate historical context while preventing it from acting as current guidance.
5. Ensure all superseded documents point to the exact canonical replacement.
6. Ensure canonical documents do not point back to non-canon sources as authority.
7. Update the docs index and generated context only where the audit authorizes it.
8. Document any unresolved or human-decision findings without guessing.

## Acceptance criteria

- Every remediated file maps to an accepted The-Ethos-Project/cogni-core-0060a606#591 finding.
- Active guidance consistently uses current repository, branch, contributor-surface, and governance terminology.
- Historical and archived material is visibly non-authoritative.
- Superseded documents contain bounded pointers rather than duplicate doctrine.
- Active deployment/setup/docs references no longer depend on obsolete repository names or paths where the audit marked them stale.
- No legitimate historical evidence is silently erased.
- No runtime or protected architecture authority changes occur.
- Repository-wide follow-up searches show no unclassified active stale matches within the audited term set.
- Working tree is clean after commit.

## Checks

- `git diff --check`
- repeat the exact search commands recorded by The-Ethos-Project/cogni-core-0060a606#591
- `python .github/scripts/build_project_context.py --check` when generated context is affected
- `python .github/scripts/check_lane_references.py` when contributor/lane terminology is affected
- `python tools/architecture_guard.py`
- `python tools/brain_law.py`
- docs link/reference validation if an existing approved check is available

## Suggested branch

`codex/issue-###-canon-residue-remediation`

## Suggested commit

`docs(canon): remediate audited stale references`

## PR body requirements

Include:

- linkage to The-Ethos-Project/cogni-core-0060a606#591 and this issue;
- exact audit findings resolved;
- changed-file inventory;
- disposition applied to each file;
- before/after terminology summary;
- unresolved findings;
- confirmation that no new doctrine was introduced;
- confirmation that no runtime, CI, tests, imports, Brain Health scoring, protected architecture, or inference paths changed;
- all check results.

## Stop conditions

Stop and request human review if:

- a target file is not listed in the accepted audit;
- current `dev` has materially changed the file since the audit;
- the correct disposition is ambiguous;
- remediation requires deletion, runtime/code changes, CI changes, test changes, dependency changes, protected architecture changes, or generator implementation changes;
- two canonical sources conflict;
- a broad refactor or repository move appears necessary.

## Handoff

Report completed findings, deferred findings, exact validation evidence, and any remaining canon drift requiring a separately scoped issue.
- **Scope:** GitHub Labels: None | Original Repo: The-Ethos-Project/ethos-logos

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
