# [ETHOS-LOGOS-022] Audit residual non-canon documentation and references

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
Parent umbrella: The-Ethos-Project/cogni-core#10

## Goal

Perform a repository-wide, evidence-backed audit of residual non-canon documentation, terminology, repository-name references, path references, and governance guidance.

## Context

Substantial canonization has already occurred, including archival of obvious legacy documents and alignment of core governance around the current developer contribution roster, `dev`, contributor surfaces, and Five Strata terminology. However, remaining hits exist across active-looking governance, project-management, planning, roadmap, architecture, deployment, generated analysis, and product-support files.

This issue is **audit and classification only**. It must not blindly replace every textual match.

## Scope

Inventory and classify residual references including, but not limited to:

- stale `execution lane`, obsolete lane, or superseded contributor-surface terminology;
- legacy `The-Eros-Project` repository/path references;
- superseded architecture or product naming;
- duplicate or conflicting governance guidance;
- active documents that should instead be historical, quarantined, superseded pointers, or archived;
- generated artifacts that preserve obsolete paths or terminology;
- code, deployment, setup, or support files containing legacy repository/path assumptions.

## Required classification

Every finding must be classified as one of:

1. **Canonical** — current and authoritative.
2. **Historical/archived** — intentionally retained with clear non-authoritative status.
3. **Generated-but-acceptable** — generated evidence that may preserve historical values without governing current behavior.
4. **Stale requiring remediation** — active or ambiguous material that conflicts with current canon.
5. **Unknown / human decision required** — insufficient evidence to classify safely.

## Deliverable

Create a dated audit document under `docs/diagnostics/` containing:

- exact path;
- matched term/reference;
- surrounding context;
- classification;
- authoritative replacement or canonical pointer where applicable;
- recommended disposition: retain, clarify, supersede, quarantine, archive, regenerate, or repair;
- risk if left unchanged;
- whether a follow-up remediation PR may touch the file.

Update `docs/DOCS_INDEX.md` only if needed to make the audit discoverable.

## Constraints

- Documentation/audit only.
- No runtime behavior changes.
- No tests, CI, imports, dependencies, Brain Health scoring, Brain Law, architecture guard, protected Brain/kernel/ethics, inference routing, or product behavior changes.
- No broad refactor.
- No repository moves or Five Strata extraction.
- No worktrees.
- Preserve uncertainty; do not infer that a textual match is stale without contextual evidence.
- All cognition and inference authority boundaries remain unchanged.

## Acceptance criteria

- Repository-wide search coverage is documented.
- Each relevant hit is individually classified.
- Archive and historical references are not falsely treated as active canon.
- Active stale guidance is distinguished from legitimate historical discussion or explicit negation.
- Old repository/path references in deployment, setup, MCP, analysis, and docs surfaces are evaluated separately.
- A bounded remediation inventory is produced for a follow-up issue/PR.
- Working tree is clean after commit.

## Checks

- `git diff --check`
- `python .github/scripts/build_project_context.py --check` if indexed/generated governance content changes
- `python .github/scripts/check_lane_references.py` if lane terminology is documented
- `python tools/architecture_guard.py`
- `python tools/brain_law.py`

## Suggested branch

`codex/issue-###-canon-residue-audit`

## Suggested commit

`docs(audit): classify residual non-canon references`

## Stop conditions

Stop and request human review if:

- classification would establish new doctrine;
- an active file conflicts with multiple plausible canonical sources;
- remediation would require runtime, CI, test, dependency, protected architecture, or generated-report tooling changes;
- deletion rather than archival appears necessary;
- repository-wide replacement would risk erasing legitimate historical evidence.

## Follow-up

The remediation issue should remain blocked until this audit is merged and reviewed.
- **Scope:** GitHub Labels: None | Original Repo: The-Ethos-Project/ethos-logos

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
