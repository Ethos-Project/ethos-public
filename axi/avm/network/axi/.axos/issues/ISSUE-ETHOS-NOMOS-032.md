# [ETHOS-NOMOS-032] [S.O.C.R.A.T.E.S.] Canonize full review-context reconstruction and corrective-scope validation

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
## Goal
Canonize a mandatory **Review Context Reconstruction Protocol** and **Corrective-Scope Validation Gate** for every authorized agent lane that inspects, remediates, or finishes a reviewed pull request.

The protocol must prevent agents from treating individual reviewer comments as isolated patch requests. Before editing, agents must reconstruct the full corrective context from the governing issue, parent/umbrella lineage, complete PR diff, all review comments and threads, referenced canon, current checks, and prior corrective work.

This issue extends the human-supervised multi-agent review workflow established by PR The-Ethos-Project/cogni-core-0060a606#514 / issue The-Ethos-Project/cogni-core-0060a606#513 and integrates the resulting gate into **S.O.C.R.A.T.E.S.** tracking under The-Ethos-Project/cogni-core-0060a606#608 and The-Ethos-Project/cogni-core-0060a606#615.

## Trigger / evidence
PR The-Ethos-Project/cogni-core-0060a606#614 exposed the process gap:

- an initial review-remediation pass surfaced two P1 findings;
- two additional P2 findings remained missed until all unresolved inline review threads were explicitly retrieved;
- the local-comment approach risked iterative defect discovery instead of full corrective-scope reconstruction.

This issue does not alter or delay PR The-Ethos-Project/cogni-core-0060a606#614. It records the systemic follow-up while The-Ethos-Project/cogni-core-0060a606#614 is completed separately.

## Repository
`thekalm0941/eros-brain`

## Branch
`codex/issue-<NUMBER>-review-context-reconstruction`

## Issue / PR title
Issue: `[S.O.C.R.A.T.E.S.] Canonize full review-context reconstruction and corrective-scope validation`

PR: `docs(governance): canonize full review-context reconstruction`

## Scope
Documentation/governance only.

Define the mandatory workflow, finding-ledger schema, corrective-scope expansion rules, status gate, validation requirements, prompt-template language, and S.O.C.R.A.T.E.S. integration for reviewed-PR remediation across Codex, Copilot, PyCharm, Antigravity, and any future authorized agent lane.

## Files allowed
- `docs/project_management/human_supervised_multi_agent_pr_review_workflow.md`
- `docs/project_management/multi_agent_pr_review_flashcards.md`
- directly relevant agent workflow or task-intake templates
- directly relevant governance indexes
- `docs/governance/SOCRATES/**` after The-Ethos-Project/cogni-core-0060a606#608 establishes the canonical structure
- `docs/DOCS_INDEX.md`
- `EROS.md` only if a minimal authority reference is required

## Files forbidden
- `PRODUCTION/**`
- runtime code
- tests
- CI/workflows
- imports
- dependencies or lock files
- Brain Health scoring or implementation
- `architecture_guard.py`
- `brain_law.py`
- inference/provider/model code
- issue The-Ethos-Project/cogni-core#11 implementation files
- PR The-Ethos-Project/cogni-core-0060a606#614 implementation branch/files except historical reference in documentation
- repository moves, extraction, or broad refactors

## Constraints
- Docs/governance only.
- Do not create a new execution lane or grant autonomous merge authority.
- Human final review/merge authority remains unchanged.
- Preserve canonical Brain authority and all Eros architecture laws.
- Do not interfere with active issue The-Ethos-Project/cogni-core#11 or PR The-Ethos-Project/cogni-core-0060a606#614.
- Do not modify CI to enforce the protocol in this issue.
- Do not assume one review comment represents the complete correction.
- Resolved and outdated threads remain relevant historical evidence and must be inspected when reconstructing context.
- Reviewers' suggested patches are evidence, not automatically the complete solution.
- No worktrees, branch deletion, history rewrite, repo moves, or broad cleanup.

## Required protocol

### 1. Mandatory context retrieval before editing
For every reviewed-PR correction, require agents to retrieve and inspect:

1. governing issue body and acceptance criteria;
2. parent/umbrella issue lineage where applicable;
3. PR title, body, base branch, head branch, and current state;
4. complete PR diff against the actual current base;
5. complete changed-file list;
6. all top-level PR comments;
7. all submitted reviews;
8. every inline review thread, including unresolved, resolved, and outdated threads;
9. current checks/workflow state and relevant logs;
10. canonical documents, schemas, APIs, or policies referenced by reviewers;
11. prior corrective commits, replies, and review cycles;
12. repository/branch status and any unrelated local changes.

No implementation may begin until this context set has been reconstructed or a documented tool/access limitation triggers a stop condition.

### 2. Review Finding Ledger
Require a ledger before edits begin. Each finding must include:

- stable finding ID;
- priority/severity;
- source reviewer and thread/comment link;
- live/resolved/outdated status;
- direct observation;
- inferred root cause;
- governing issue requirement affected;
- affected files and sibling surfaces;
- required correction;
- adjacent/symmetric checks;
- validation method;
- disposition: pending / corrected / rejected-with-evidence / blocked;
- corrective commit and reply reference when complete.

### 3. Root-cause and corrective-scope expansion
Require agents to treat comments as symptoms that may indicate a broader defect.

For every finding, inspect as applicable:

- sibling fields and enum values;
- related schemas and examples;
- producers and consumers;
- indexes and registries;
- authority documents;
- fallback and terminal states;
- analogous files or product surfaces;
- naming and alias consistency;
- issue acceptance criteria beyond the commented line.

Prevent unbounded refactoring: expansion must remain tied to the same evidenced root cause and governing scope.

### 4. Full-diff reassessment after correction
After edits, require agents to:

- reread the complete PR diff against the current base;
- rerun the governing issue acceptance criteria;
- verify every finding-ledger disposition;
- check that one correction did not create a new contradiction;
- inspect changed-file scope for unrelated drift;
- re-fetch live threads before requesting re-review;
- document any new reviewer findings separately rather than silently absorbing them.

### 5. Corrective review gate
Define a canonical gate such as:

`REVIEW_CONTEXT_COMPLETE`

A corrective PR may not move from active remediation to validation/re-review until:

- required context sources were retrieved;
- a finding ledger exists;
- every live finding has a disposition;
- governing issue scope was reconciled;
- the full diff was reassessed;
- validations were recorded;
- unresolved limitations or founder decisions are explicit.

Integrate this gate into S.O.C.R.A.T.E.S. milestone/work-queue/dashboard semantics under The-Ethos-Project/cogni-core-0060a606#608/#615.

### 6. Re-review and thread handling
Define that agents must:

- reply to each addressed inline thread with the actual correction and validation;
- not resolve threads before the correction is present in the pushed diff;
- explicitly reject a suggestion only with evidence and human-visible rationale;
- request re-review only after all live findings are corrected, rejected-with-evidence, or blocked;
- re-fetch review threads after new review activity;
- never claim review completion from a truncated summary feed alone.

### 7. Prompt-template canon
Add a reusable mandatory block to implementation/corrective prompts requiring:

- complete review-context retrieval;
- finding ledger creation;
- root-cause expansion;
- full-diff reassessment;
- original acceptance-criteria validation;
- re-review rules;
- stop conditions and handoff.

Ensure the standard implementation prompt fields remain present: goal, repo, branch, issue/PR title, scope, files allowed/forbidden, constraints, tasks, criteria, checks, commit message, PR body, stop conditions, and handoff.

### 8. Metrics for S.O.C.R.A.T.E.S.
Define trackable fields for dashboards:

- total findings;
- findings by priority;
- corrected findings;
- rejected-with-evidence findings;
- blocked findings;
- unresolved live threads;
- review-cycle count;
- new findings introduced per cycle;
- context reconstruction complete/incomplete;
- stale review-context timestamp;
- re-review requested/completed;
- corrective-scope expansion count;
- corrective regression count where evidenced.

Metrics must support process improvement, not automatic blame or agent autonomy.

## Required deliverables
At minimum, update or create:

- the canonical human-supervised multi-agent PR review workflow;
- the review workflow flashcards/quick reference;
- a reusable `REVIEW_CONTEXT_RECONSTRUCTION_PROTOCOL.md` or equivalent canonical section;
- a `REVIEW_FINDING_LEDGER` schema/template;
- the standard corrective-task prompt template;
- S.O.C.R.A.T.E.S. gate/metric references after The-Ethos-Project/cogni-core-0060a606#608/#615 structures exist;
- relevant documentation index entries.

Do not duplicate existing workflow documents where a focused extension is sufficient.

## Acceptance criteria
- Every authorized agent lane is explicitly required to read all review comments and threads before corrective edits.
- Unresolved, resolved, and outdated inline threads are included in the context requirement.
- The governing issue, parent lineage, complete diff, checks, and referenced canon are mandatory context.
- A review finding ledger is mandatory before implementation.
- Each finding is expanded to its evidenced root cause and adjacent affected surfaces.
- Corrective scope remains bounded by the governing issue and root cause.
- Full PR diff and original acceptance criteria are reassessed after correction.
- Truncated comment summaries cannot be treated as complete review context.
- Re-review cannot be requested until all live findings have documented dispositions.
- `REVIEW_CONTEXT_COMPLETE` or equivalent is integrated into S.O.C.R.A.T.E.S. planning semantics.
- Human merge authority remains unchanged.
- PR The-Ethos-Project/cogni-core-0060a606#614 is referenced as evidence, not modified by this issue.
- No runtime, test, CI, import, dependency, inference, Brain Health, architecture guard, brain law, or issue The-Ethos-Project/cogni-core#11 behavior changes occur.
- Diff remains focused and reviewable.

## Checks
Run:

```powershell
git diff --check
git status --short
python tools/check_lane_references.py
python scripts/build_project_context.py --check
```

Use canonical documented invocations if these have changed and record them in the PR body.

Perform targeted searches for:

```powershell
Get-ChildItem docs,EROS.md -Recurse -File |
  Select-String -Pattern "review context|review thread|finding ledger|corrective scope|REVIEW_CONTEXT_COMPLETE|re-review|outdated thread|resolved thread"
```

Do not run runtime tests or modify CI for this docs-only issue.

## Commit message
`docs(governance): canonize review-context reconstruction`

## PR body
Include:

- Summary
- Evidence from PR The-Ethos-Project/cogni-core-0060a606#614
- Relationship to PR The-Ethos-Project/cogni-core-0060a606#514 / issue The-Ethos-Project/cogni-core-0060a606#513
- Relationship to S.O.C.R.A.T.E.S. The-Ethos-Project/cogni-core-0060a606#608/#615
- Mandatory context sources
- Finding-ledger schema
- Corrective-scope expansion rules
- Review gate and dashboard metrics
- Agent-lane coverage
- Files changed
- Validation performed
- Pending decisions
- Confirmation of no runtime/CI/protected architecture changes
- Closes #<NUMBER>

## Stop conditions
Stop and report without guessing if:

- the existing multi-agent review workflow conflicts with another active canonical authority;
- The-Ethos-Project/cogni-core-0060a606#608/#615 structures are not yet present and direct S.O.C.R.A.T.E.S. file edits would create competing structure;
- a required change would modify CI, runtime, protected architecture, issue The-Ethos-Project/cogni-core#11, or PR The-Ethos-Project/cogni-core-0060a606#614 files;
- the protocol would grant autonomous merge/review authority;
- required review-context sources cannot be retrieved and no limitation can be documented safely;
- the task expands into broad agent-framework refactoring;
- governance checks fail for reasons outside docs-only scope.

Where S.O.C.R.A.T.E.S. dependencies are not yet merged, document the integration contract and defer direct file updates rather than creating duplicate authority.

## Handoff
Provide:

- branch name;
- commit SHA;
- PR URL;
- files created/updated;
- agent lanes covered;
- mandatory context sources canonized;
- finding-ledger schema summary;
- corrective gate established;
- S.O.C.R.A.T.E.S. integration status;
- validation results;
- pending decisions;
- confirmation that PR The-Ethos-Project/cogni-core-0060a606#614 and issue The-Ethos-Project/cogni-core#11 were untouched;
- recommended follow-up enforcement issues, ordered by dependency.
- **Scope:** GitHub Labels: documentation | Original Repo: The-Ethos-Project/ethos-nomos

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
