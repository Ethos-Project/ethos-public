# [ETHOS-LOGOS-023] Canonize axi and user-designed governed companions across surfaces

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
Parent umbrella: The-Ethos-Project/cogni-core-0060a606#559

## Purpose

Canonize **axi** as the project’s first embodied companion while defining a broader **Pets** system through which users may select, configure, and design their own governed companions across ErosOS and the wider Eros product ecosystem.

axi remains the canonical first-party companion and already has a continuity-ready asset package:

- `pet.json`
- `spritesheet.webp`
- `axi_sprite_sheet.png`

The current manifest identity is:

> A calm synthetic field-companion for ethical AI research: curious, protective, local-first, and quietly precise.

This issue also establishes the planning direction for user-created pets as an optional personalization layer that adds warmth, identity, and nuance to the Eros experience without creating alternate cognition authorities.

This is a **canon planning issue track**. It records product architecture, companion identity, customization boundaries, settings ownership, state-contract rules, and future roadmap sequencing. It does **not** authorize runtime implementation, protected Brain changes, direct inference access, package movement, repo extraction, CI changes, or application-layer cognition.

## Roadmap independence

The **ErosOS stability track is not dependent on this companion-planning track**.

ErosOS stabilization must continue independently and must not be blocked, delayed, or conditioned on axi, Pets settings, custom pet creation, companion rendering, asset packaging, or companion-state integration.

This issue may proceed as canon and product-planning work in parallel, but no item here becomes a prerequisite for ErosOS stability closure.

Future companion implementation may rely on a sufficiently stable host surface, but the dependency is one-way only:

```text
ErosOS stability -> may enable later companion implementation
Companion planning -> does not block or gate ErosOS stability
```

## Canonical companion model

A companion is a governed embodied interface for safely exposed Eros state and user-facing presence.

```text
Eros Brain authority path
  -> governed companion-state projection
  -> product-side companion adapter
  -> axi or user-selected pet renderer
```

A companion may provide:

- ambient presence;
- expressive but causally grounded task-state communication;
- gentle interaction cues;
- optional decorative idle behavior;
- continuity across approved product surfaces;
- user-selected visual identity and personality presentation;
- a more personal and nuanced relationship with the Eros experience.

A companion is **not**:

- a separate agent;
- a second brain;
- a duplicate executor;
- an application-layer LLM client;
- an alternate memory system;
- an independent emotional authority;
- an inference, ethics-kernel, or orchestrator bypass.

A companion may express only state deliberately projected through the approved Eros Brain authority path. User customization may alter presentation, but must not alter the meaning, authority, or provenance of governed state.

## axi’s canonical role

axi is the canonical first-party companion of The Eros Project and the reference implementation for the companion contract.

axi should remain available across approved surfaces with continuity in:

- name;
- visual identity;
- manifest identity;
- animation semantics;
- governed state meanings;
- provenance;
- personality description;
- accessibility behavior;
- local-first operation.

axi may serve as:

- the default project companion;
- the reference pet for validating sprite and manifest formats;
- the design baseline for state-to-animation semantics;
- the canonical example used in documentation and demonstrations;
- a stable continuity anchor while users create or select other companions.

## User-designed custom pets

Users should be able to design their own custom pets through an approved Eros interaction flow.

The purpose is not merely cosmetic. A custom companion can add nuance by allowing the user to shape the felt character of the interface while preserving the same governed cognitive authority beneath it.

Candidate creation flow:

```text
User describes desired companion
  -> request enters approved Eros Brain authority path
  -> governed design assistance produces a reviewable pet specification
  -> user reviews and approves identity, assets, and behavior mappings
  -> product layer validates and installs the companion package
  -> selected companion renders only approved state projections
```

Custom pet design may include:

- name;
- species or form;
- visual style;
- palette;
- sprite or animation set;
- display description;
- non-semantic idle behaviors;
- approved state-expression mappings;
- size and placement preferences;
- sound preferences, if later authorized;
- accessibility variants;
- reduced-motion variants.

Custom pet design must not include:

- independent system prompts that create a shadow persona or cognition path;
- direct LLM credentials or provider configuration;
- scripts with unrestricted execution authority;
- hidden network calls;
- private memory stores outside approved Eros memory governance;
- custom meanings that falsify or reverse governed state;
- arbitrary code executed by the renderer;
- alternate inference, tool, ethics, or orchestration paths.

Any generative assistance used to help create a pet must route through Eros’s approved Brain, field, and orchestrator path. The application layer must never call a model directly to generate companion assets, descriptions, dialogue, or behavior.

## Pets settings category

All relevant companion configuration should reside under a dedicated, clearly discoverable **Pets** settings category.

Candidate structure:

```text
Settings
  -> Pets
       -> Active pet
       -> Browse installed pets
       -> Create a custom pet
       -> Import pet package
       -> Export pet package
       -> Appearance
       -> Position and docking
       -> Animation and motion
       -> State expressions
       -> Sounds
       -> Interaction behavior
       -> Accessibility
       -> Privacy and telemetry
       -> Reset or remove pet
```

The exact surface hierarchy may vary by platform, but companion-related controls should not be scattered across unrelated settings pages.

### Candidate Pets settings

- enable or disable the companion;
- choose the active pet;
- open the custom pet creator;
- import and validate a local pet package;
- export a user-owned pet package where permitted;
- choose startup visibility;
- set scale;
- choose screen location, docking edge, or free movement;
- toggle always-on-top behavior;
- toggle click-through behavior;
- control idle animation intensity;
- enable reduced motion;
- pause semantic animations while preserving accessible text status;
- configure optional sounds independently from visual state;
- inspect installed pet identity and provenance;
- review what state information the pet may display;
- remove custom pets without affecting Eros cognition or memory;
- restore axi as the default companion.

Settings that affect only rendering remain product-side preferences. No Pets setting may grant cognition, inference, tool, memory, ethics, or execution authority.

## Companion identity package

A common declarative package contract should support axi and future custom pets.

Candidate package:

```text
pet-package/
  pet.json
  spritesheet.webp
  source-spritesheet.png
  state_map.json
  accessibility.json
  provenance.md
  license.md
  README.md
```

Candidate manifest fields may include:

- stable package identifier;
- display name;
- description;
- creator or owner;
- format version;
- sprite-sheet path;
- supported states;
- animation metadata;
- accessibility metadata;
- provenance metadata;
- license or usage status;
- local-only or shareable designation;
- compatibility version.

Pet packages should be data-only by default and must not contain executable code unless a separately governed extension model is explicitly authorized later.

## Governed state contract

Only a narrow, read-only companion-state projection may cross from the Brain authority path into the product renderer.

Candidate state vocabulary:

- `resting`
- `attentive`
- `thinking`
- `needs_guidance`
- `uncertain`
- `safety_alert`
- `system_degraded`
- `task_complete`

Each meaningful projection should be causally grounded by safe metadata such as a reason code, timestamp, severity, and trace identifier. It must not expose private chain-of-thought, unrestricted memory, raw prompts, or sensitive internal telemetry.

The Brain does not own sprite playback, window positioning, theme settings, package installation, or pet asset storage.

## Product-side responsibility

The product layer owns:

- the Pets settings category;
- canonical axi asset bundle;
- user pet package storage and validation;
- sprite parsing and animation playback;
- state-to-animation mapping;
- transparent or dockable companion UI;
- click-through, tray, accessibility, and reduced-motion settings;
- custom pet creation workflow UI;
- package import, export, reset, and removal controls;
- platform adapters;
- tests proving the renderer and package loader cannot invoke inference or mutate protected Brain state.

## Design laws

> axi may express what Eros has safely exposed, but must never invent what Eros is experiencing.

> A custom pet may change the expression of a state, but not the truth or authority of that state.

Every semantic expression must have a traceable source. Decorative idle motion is allowed when clearly non-semantic. User customization must not turn an uncertain, blocked, or degraded state into a misleading success signal.

The companion experience should be warm and personal without becoming deceptive, manipulative, or architecturally privileged.

## Phased roadmap

### Phase 0 — Canon and asset preservation

- preserve the original axi manifest and sprite assets;
- document provenance, dimensions, sprite format, and license status;
- establish axi as the canonical first-party project companion;
- define the distinction between first-party, user-created, and imported pets;
- prevent independent redraws or divergent axi manifests across products.

### Phase 1 — Companion and Pets UX canon

- define the Pets settings category and information architecture;
- define installation, selection, removal, import, export, and reset behavior;
- define the custom pet creation journey;
- define accessibility and reduced-motion expectations;
- define package-validation and provenance requirements;
- define which options are local-only and which may synchronize later.

### Phase 2 — Product-side visual prototype

- create a renderer using a mock state provider only;
- support axi and at least one synthetic test package;
- support a minimal four-state set: resting, thinking, needs guidance, task complete;
- validate sprite animation, transparency, scaling, docking, selection, and reduced motion;
- validate that switching pets changes presentation but not state semantics;
- no live Brain connection.

### Phase 3 — Governed custom pet design prototype

- prototype a review-first custom pet specification flow routed through the approved Eros Brain path;
- produce declarative assets and metadata only;
- require explicit user approval before package installation;
- validate provenance and compatibility before activation;
- prohibit application-layer direct model calls and arbitrary executable pet content.

### Phase 4 — Governed projection contract

- define the smallest safe Brain-to-product state contract;
- review against Brain Law, architecture guard, confidentiality boundaries, and developmental-phase constraints;
- prove no inference, memory, tool, ethics, or action authority is granted to any pet.

### Phase 5 — ErosOS integration

- replace mock state with a read-only governed adapter;
- add traceable state transitions and failure-safe fallback behavior;
- ensure renderer, package, or custom-pet failure cannot affect cognition or task execution;
- preserve axi as the safe default and recovery companion.

### Phase 6 — Multi-surface continuity

- reuse the canonical companion contract across approved ErosOS, DevForge, ECC, mobile, glasses, spatial, and future embodiment surfaces;
- preserve consistent state semantics while allowing platform-specific presentation.

## Acceptance criteria for planning closure

- [ ] axi is canonized as the first-party project companion.
- [ ] User-created and imported pets are explicitly distinguished from first-party companions.
- [ ] A dedicated Pets settings category is defined.
- [ ] The custom pet creation journey is documented.
- [ ] Product and Brain ownership boundaries are explicit.
- [ ] No direct LLM, `llm_bridge`, inference gate, ethics kernel, or orchestrator bypass exists.
- [ ] Pet packages are declarative and non-executable by default.
- [ ] A minimal governed state vocabulary is approved.
- [ ] Semantic animation states require traceable causes.
- [ ] Raw chain-of-thought, prompts, unrestricted memory, and sensitive telemetry are excluded.
- [ ] Switching pets cannot alter cognition or governed state meaning.
- [ ] Accessibility, reduced motion, provenance, failure isolation, and local-first behavior are included.
- [ ] ErosOS stability is explicitly not gated by this issue.

## Roadmap linkage

- Supports The-Ethos-Project/cogni-core-0060a606#559 — cognition-native ErosOS platform vision and flagship application family.
- Supports The-Ethos-Project/cogni-core-0060a606#562 — ErosOS as the governed AI-ecosystem unification layer.
- Supports The-Ethos-Project/cogni-core-0060a606#564 — ErosOS multi-surface continuity across mobile, glasses, and spatial computing.
- Relates to The-Ethos-Project/cogni-core-0060a606#471 — Eros Companion Console naming and interface boundary.
- Track under The-Ethos-Project/ethos-nomos#21 — Five Strata and company/product roadmap tracker.
- Sequence under The-Ethos-Project/cogni-core-0060a606#523 — one-person execution plan and issue families.
- Remains subordinate to The-Ethos-Project/cogni-core#10 for implementation priority, while **not gating any ErosOS stability work**.

## Stop conditions

Stop and require human review if any proposed implementation:

- gives axi or a custom pet independent cognition or action authority;
- imports or calls `llm_bridge` from an application layer;
- bypasses the ethical field, inference gate, BrainOrchestrator, or approved authority path;
- creates a shadow state interpreter from raw logs or UI heuristics;
- exposes private cognition, unrestricted memory, prompts, or confidential telemetry;
- allows executable pet packages or hidden network behavior without separate authorization;
- permits custom pets to falsify governed state meaning;
- changes protected Brain runtime, Brain Health scoring, architecture guard, CI, or repo boundaries without separate authorization;
- makes ErosOS stabilization contingent on companion planning or implementation;
- duplicates axi assets or semantics in ways that create identity drift.
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
