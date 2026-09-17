# [ETHOS-LOGOS-026] Define ErosOS cross-surface strategy for wearables, spatial computing, and ambient interfaces

## 1. Task Description
- **Goal:** (Migrated from GitHub)
- **Context:**
Parent umbrella: The-Ethos-Project/cogni-core-0060a606#559

## Purpose

Define the long-term ErosOS surface strategy across desktop, mobile, smart glasses, spatial-computing/XR headsets, vehicle interfaces, home interfaces, and future embodied systems.

This issue records product direction, boundary principles, privacy requirements, sequencing, and market implications. It does **not** authorize hardware development, device procurement, external platform integration, runtime implementation, direct provider access, public compatibility claims, CI changes, repo extraction, or protected Brain changes.

## Core thesis

ErosOS is not tied to a desktop application or conventional computer.

> The product is continuity with a governed intelligence across every surface of a person's life.

The same Eros identity, Cogni-Core authority, memory fabric, permissions, evidence, uncertainty, and approval model should persist across devices while each surface exposes only the interaction needed for its context.

## Surface model

```text
Eros Psyche
└── ErosOS cognition environment
    ├── Desktop surface
    ├── Mobile surface
    ├── Smart-glasses / wearable surface
    ├── Spatial-computing / XR surface
    ├── Vehicle surface
    ├── Home / ambient surface
    └── Future embodiment surfaces
```

The device is an interface and sensory/motor surface. It is not an independent cognition authority.

## Smart glasses and wearable role

Smart glasses are a candidate **ambient cognition surface** for:

- hands-free conversation;
- navigation and translation;
- reading and explaining signs, labels, and documents;
- reminders connected to context;
- accessibility support;
- bounded first-person visual assistance;
- simple confirmations and approval requests;
- continuity without requiring a conventional computer interface.

Wearables must not become continuous ungoverned surveillance devices.

## Spatial-computing / XR role

XR headsets are a candidate **spatial cognition workspace** for:

- immersive ErosOS workspaces;
- visible task, evidence, trace, and approval objects;
- spatial research and memory views;
- DevForge architecture and repository environments;
- simulation and embodiment control;
- collaborative planning spaces;
- accessibility-adaptive interfaces.

## Ordinary-user market principle

The surface strategy must prioritize people who:

- do not develop software;
- do not consider themselves AI power users;
- may use a phone more than a computer;
- may have limited comfort with traditional desktop interfaces;
- want help accomplishing real-life tasks without learning model selection, APIs, prompting systems, or technical configuration.

ErosOS should progressively disclose complexity rather than force technical operation on the user.

## Cross-surface continuity requirements

Every approved surface should preserve:

- one Eros identity;
- one governed Cogni-Core authority path;
- one user permission model;
- one memory and context fabric;
- explicit uncertainty and evidence;
- traceable capability use;
- consistent approval boundaries;
- revocation and recovery paths;
- no direct model/provider access from the device surface.

## Sensory and privacy boundary

Wearable and ambient devices create elevated risks involving:

- bystander privacy and consent;
- continuous audio/video capture;
- biometrics;
- location and behavioral profiling;
- family, health, workplace, and confidential information;
- unclear recording or retention states.

Any future sensory-device contract must require:

- visible and unambiguous sensing indicators;
- local processing by default where feasible;
- explicit capture modes rather than silent continuous recording;
- data minimization and automatic redaction;
- separate authorization for observing, retaining, recalling, and acting;
- revocable memory and deletion controls;
- user-visible records of what was sensed and retained;
- bystander-sensitive policies;
- fail-closed behavior when consent, identity, or authority is unclear;
- quarantine and revocation for unsafe device adapters.

## Architectural principle

```text
Device surface
    |
    v
bounded sensory / interaction contract
    |
    v
ErosOS
    |
    v
Cogni-Core ethical and executive authority
    |
    v
approved capabilities
    |
    v
governed response or proposed action
```

No surface may own:

- inference authority;
- provider lifecycle;
- ethics authority;
- memory authority;
- autonomous tool execution outside approved capability contracts;
- a duplicate or shadow cognition pipeline.

## Product strategy

The Ethos Project should not begin by manufacturing smart glasses or XR hardware.

Recommended sequence:

1. restore and verify the ErosOS desktop reference surface;
2. establish the mobile-first ErosOS client and cross-platform contracts;
3. define bounded sensory and ambient-interface contracts;
4. prototype against existing developer platforms only after privacy and authority gates exist;
5. validate accessibility and ordinary-user value;
6. evaluate partnerships, OEM relationships, or reference hardware only after demand and product stability are demonstrated.

## Business and market implications

Future business plans should test:

- demand for hands-free and ambient assistance;
- mobile-primary and non-computer user adoption;
- accessibility use cases;
- willingness to use one persistent cognition environment across devices;
- trust in local-first sensing and explicit retention controls;
- value of cross-device continuity;
- partner interest from device, platform, automotive, home, accessibility, and enterprise vendors;
- added support, compliance, and liability costs by surface type.

No hardware partnership or market claim is authorized by this issue.

## Relationship to ErosOS and Ethos Suite

- ErosOS remains the cognition-native operating environment.
- ErosOS Desktop is the near-term reference implementation.
- Ethos Suite distributes approved standalone product forms.
- DevForge remains the specialist development flagship.
- ECC remains the direct console and diagnostic surface.
- Wearable and XR surfaces are future ErosOS interfaces, not new cognition authorities or initial flagship products.

## Related issues

- The-Ethos-Project/cogni-core#10 — Complete Eros local functional baseline
- The-Ethos-Project/cogni-core-0060a606#559 — Define the cognition-native ErosOS platform vision and flagship application family
- The-Ethos-Project/cogni-core-0060a606#560 — Define Ethos Suite as the standalone Cogni-Core product family
- The-Ethos-Project/cogni-core-0060a606#561 — Model Ethos Suite tiered subscriptions and business-plan metrics
- The-Ethos-Project/cogni-core-0060a606#562 — Define ErosOS as the governed unification layer for the AI ecosystem

## Acceptance criteria

- [ ] Desktop, mobile, wearable, XR, vehicle, home, and future embodiment surfaces are recorded.
- [ ] Ordinary and nontechnical users remain a primary market.
- [ ] Smart glasses are defined as an ambient cognition surface.
- [ ] XR is defined as a spatial cognition workspace.
- [ ] Cross-surface identity, memory, permission, evidence, and authority continuity are explicit.
- [ ] Wearable privacy and bystander protections are explicit.
- [ ] Hardware manufacturing is deferred behind product and governance gates.
- [ ] No integration or implementation is authorized.

## Non-authorization statement

This issue does not authorize:

- hardware design or manufacturing;
- device purchases;
- Meta, Apple, Google, Microsoft, automotive, or OEM integrations;
- public compatibility or partnership claims;
- camera, microphone, biometric, or location ingestion;
- runtime or product implementation;
- direct model/provider access;
- CI changes;
- repo extraction;
- protected Brain modifications.
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
