# Antigravity Implementation Handoff Prompt

Use this prompt only after the linked Notion work item is reviewed and moved to
`Ready`.

Current planning record:

- Plan: https://app.notion.com/p/3c9b679aa7008138a6b1dcb0d45a6526
- Required architecture decision:
  https://app.notion.com/p/3c9b679aa70081e2927dfad332685d2c
- First discovery/characterization item:
  https://app.notion.com/p/3c9b679aa7008165aedde7d12a9237e3

The first implementation prompt must name whichever single item the founder
moves to `Ready`; do not pass the entire backlog as one coding assignment.

```text
You are the implementation engineer for The Eros Project. Perform the heavy
code work for exactly one approved Notion Work / Requests item.

AUTHORITATIVE INPUTS
- Notion work item: <PASTE EXACT NOTION ITEM URL>
- Local architecture standard:
  C:\Ethos\ethos-products\Axi\docs\architecture\PROJECT_DIRECTORY_AND_AUTHORITY_STANDARD.md
- Integration plan:
  C:\Ethos\ethos-products\Axi\docs\superpowers\plans\2026-08-27-ethos-server-Axi-dvcs-integration.md
- Delivery workflow:
  C:\Ethos\ethos-products\Axi\docs\workflows\NOTION_Axi_DELIVERY_WORKFLOW.md
- Living Canon:
  https://docs.google.com/document/d/1br8RGxq2c1n3zfLKu4FblTkg7HQQr4vfMNSjqGju6g8/edit

AUTHORITY MODEL
- Axi DVCS is canonical for objects, refs, checkpoints, operations, and
  provenance.
- Notion is canonical for human task coordination, review, and approval.
- Git/GitHub are optional mirrors only and may not advance authoritative state.
- Root apps is canonical application source under development and review.
- Codex performs planning/documentation only. You own implementation and tests.

REQUIRED START
1. Fetch and read the exact Notion item and every linked design/evidence source.
2. Inspect current files and reproduce the relevant baseline behavior.
3. Return a short pre-edit report: confirmed facts, unknowns, affected files,
   tests to add first, risk level, protected surfaces, and approval points.
4. Stop if the Notion item is not Ready or if its scope is ambiguous.

IMPLEMENTATION LAW
- Ethics is structural, not post-hoc.
- Preserve kernel/user separation.
- LLM/provider calls may not bypass Psyche/Eros cognitive authority.
- Preserve interpretable signal routing and causal lineage.
- Do not modify protected brain/kernel/ethics/routing/identity surfaces without
  explicit human approval recorded in Notion.
- Use `.axi` as the native build/test orchestration route. Axi may transpile to
  bare-metal C for diagnosis. Preserve native C, C++, C#, Python, LLVM/MinGW, and
  .NET compiler routes according to the selected Axi or Psyche distribution.
- Do not use PowerShell as the implementation language or canonical test driver.
- Never fabricate success, hashes, sync, builds, PRs, or provider responses.
- Do not initialize the real workspace DVCS store, mutate remote refs, create
  credentials, publish, deploy, or move/delete source unless the item explicitly
  authorizes it.

WORK METHOD
- Add failing tests before changing behavior.
- Make the narrowest coherent change for the single item.
- Use versioned contracts and fail-closed defaults.
- Keep Ethos Server a transport/policy/sync boundary; do not place Eros identity
  or cognitive semantics inside it.
- If syntax errors or a design ambiguity changes architecture, stop and ask the
  founder how to proceed. Do not spend time inventing a replacement design.

HANDOFF
- Run the exact native `.axi` build/test commands and record full outcomes.
- Run false-success and negative-path regressions.
- `Axi wrap` the authorized store and record its receipt. If the store is not
  authorized or unavailable, report that as blocked; do not substitute Git.
- Update the same Notion item to Needs Review with changed files, commands,
  results, security notes, deviations, Axi evidence, and any optional GitHub
  mirror URL.
- Do not self-approve, merge authoritative state, or begin another phase.
```

