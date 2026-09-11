# Notion and Axi Delivery Workflow

Status: Working operating procedure for human review  
Date: 2026-08-27

## 1. System roles

- **Notion:** official collaborative intake, task documentation, planning,
  decision review, and human approval surface.
- **Axi DVCS:** authoritative objects, refs, checkpoints, operations, receipts,
  and delivery provenance.
- **Ethos Server:** governed service and synchronization boundary.
- **Git/GitHub:** optional interoperability, historical evidence, or PR mirror.
- **Codex:** discovery, planning, standards, task records, review synthesis, and
  implementation prompts only; no runtime/source edits.
- **Antigravity:** bounded implementation and test execution; no self-approval.
- **Human reviewer:** accepts, rejects, or requests changes on consequential work.

## 2. One work record

Every executable change begins as one item in Notion `Work / Requests` and must
include:

- one outcome and bounded scope;
- exact repository/path ownership;
- data class and protected-surface declaration;
- acceptance criteria and verification commands;
- known unknowns and stop conditions;
- exact source/design evidence;
- Axi record/receipt when available;
- optional GitHub mirror URL labeled as non-authoritative.

Agents must cite the exact Notion item URL in prompts, handoffs, evidence, and
Axi operation metadata. A copied title is not a stable identifier.

## 3. Lifecycle

### Intake

Codex converts the founder request into a scoped item and inspects current
source/docs. Claims are labeled as confirmed, intended, or unresolved.

### Ready

The item has a bounded outcome, affected files, risks, acceptance criteria,
dependencies, and explicit approval requirements. Human decisions that would
materially alter the implementation are resolved or separated as Decision
Requests.

### In Progress

Antigravity implements only the approved scope. It records the exact Notion URL
and the current Axi baseline before changes. When syntax or architectural
ambiguity would change the design, it stops and asks the founder rather than
inventing a resolution.

### Needs Review

Antigravity returns:

- changed-file inventory;
- test/build commands and exact results;
- security and protected-surface notes;
- Axi wrap/checkpoint receipt;
- unresolved failures and deviations;
- optional GitHub PR mirror URL.

Codex checks the evidence against the acceptance criteria and summarizes gaps.
Codex may update documentation and Notion review records but does not repair the
runtime code.

### Approved, Changes Requested, or Rejected

The authorized human records the decision in Notion. The agent cannot infer
approval from silence, tests, a GitHub approval, or a merged mirror PR.

### Done

An item closes only when acceptance evidence, the human decision, and the
authoritative Axi record are linked. If an approved Axi workspace store does
not yet exist, the item remains `Needs Review` or `Blocked`; a Git commit cannot
substitute for the missing authoritative receipt.

## 4. Pull request workflow

1. Antigravity creates a verified Axi checkpoint.
2. The change proposal references the exact Notion work item and Axi object/ref.
3. When Axi-native PR/review objects are implemented and verified, they are
   the authoritative review proposal.
4. If external collaboration requires GitHub, create a mirror PR from the same
   checkpoint.
5. Store the GitHub URL under Notion `Source / Evidence` or the item body and
   label it `GitHub mirror`.
6. Record review outcomes in Notion and the corresponding Axi operation.
7. Reconcile the mirror to the accepted Axi state; never allow the mirror to
   silently advance authoritative refs.

Until Axi-native PR objects are verified, Notion is the human review record
and Axi wrap/checkpoint evidence is the authoritative technical record.

## 5. Required review gates

Human approval is required before:

- protected brain, kernel, ethics, routing, identity, or cognition changes;
- provider authority/routing changes;
- live credentials, authentication policy, or external exposure;
- real workspace initialization, server synchronization, or ref mutation;
- source relocation or generated-artifact deletion;
- public release, visibility, licensing, or publication changes;
- production deployment.

Routine implementation inside an already approved bounded task does not require
additional ceremonial gates. Stop only when a decision is consequential,
architecturally unresolved, safety-relevant, or outside the approved scope.

## 6. Evidence labels

- **Verified:** reproduced from current source with recorded command/result.
- **Maintainer-confirmed:** founder direction not yet fully evidenced in source.
- **Intended:** approved target architecture not yet implemented.
- **Unresolved:** decision or implementation status is genuinely unknown.
- **Mirror:** non-authoritative external representation.

These labels prevent plans, simulations, and historical designs from being
mistaken for implemented capability.

