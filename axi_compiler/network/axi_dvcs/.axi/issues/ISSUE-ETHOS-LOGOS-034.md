# [ETHOS-LOGOS-034] #421-C — RFC for ethical potential and attractor metrics

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
Parent umbrella: The-Ethos-Project/ethos-nomos#45

## Purpose

Create an RFC/spec for `ethical potential` and related attractor metrics so documented mathematical language, proxy behavior, archived/demo behavior, and live runtime behavior are not conflated.

This issue follows the runtime canon ledger merged in PR The-Ethos-Project/cogni-core-0060a606#460, which identified ethical potential and some attractor instrumentation mappings as documented-only, proxy, or unknown rather than fully canonical live runtime metrics.

## Parent / evidence

- Baseline roadmap: The-Ethos-Project/cogni-core#10
- Parent ledger: The-Ethos-Project/cogni-core-0060a606#421
- Evidence PR: The-Ethos-Project/cogni-core-0060a606#460
- Related runtime metric issue: The-Ethos-Project/cogni-core-0060a606#422
- Ledger artifact: `docs/diagnostics/eros_runtime_canon_completion_ledger_2026-07-05.md`

## Scope

### In scope

- Inventory documented references to ethical potential, attractor scores, attractor activations, curvature, dissonance, blocking attractor, confidence, and uncertainty.
- Distinguish design/spec/research language from live runtime outputs.
- Map current evaluator outputs to a proposed canonical datamodel.
- Define whether ethical potential is a cost, field-energy metric, stability measure, protective basin score, or another signed/unsigned quantity.
- Define gating semantics and how verifier/trace should treat the metric.
- Recommend implementation issues only after the RFC/spec is complete.

### Out of scope

- No runtime implementation.
- No ethics-kernel behavior changes.
- No inference routing changes.
- No Brain Health scoring changes.
- No CI changes.
- No public/research claims that the metric is already fully implemented.
- No duplicate ethics path or evaluator.

## Acceptance criteria

- RFC/spec distinguishes documented math, proxy fields, archived/demo behavior, and live runtime behavior.
- Proposed datamodel maps ethical potential and attractor metrics to current evaluator outputs and gaps.
- Gating semantics are described without changing runtime behavior.
- Follow-up implementation issues are identified only if needed.
- No runtime/protected architecture behavior changes occur.

## Stop conditions

Stop before implementing ethical potential, changing evaluator outputs, changing ethics-kernel behavior, changing inference gates, changing Brain Health scoring, or adding public claims without separate approval.

## Sibling follow-ups

Sibling issue numbers will be linked after the The-Ethos-Project/cogni-core-0060a606#421-A through The-Ethos-Project/cogni-core-0060a606#421-F child issue set is created.
- **Scope:** GitHub Labels: bug | Original Repo: The-Ethos-Project/ethos-logos

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
