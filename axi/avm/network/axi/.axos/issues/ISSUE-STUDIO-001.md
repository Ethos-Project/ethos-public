# ISSUE-STUDIO-001: Wire axi_studio_state into Spatial Viewport

**Stratum:** 2 — The Ethos Suite (ethos-products)
**Status:** [DONE]
**Agent:** Antigravity
**Date:** 2026-08-23
**Target File:** ethos-nomos/ethos-products/languages/axi-lang/axi_studio.axi

## Goal

Connect the live `SpatialNode[]` array and `Camera2D` state from `axi_studio_state.axi`
into the `draw_ui()` render node of `axi_studio.axi`. Replace all static mock content with
live, interactive spatial node graph rendering including:

1. Live node graph in the central viewport (renders `canvas_nodes[]` as draggable boxes)
2. Camera pan/zoom via `Camera2D` offset
3. Real `.axi/issues/` filenames in Explorer sidebar (read from filesystem)
4. Wire drag rendering between nodes
5. Keyboard/mouse input dispatch to state

## Planned Mutations

- MODIFY: `axi_studio.axi` — replace static `draw_ui()` with live node graph renderer
- ADD: `build_studio.bat` — dedicated build script for axi_studio (parallel to build_domus.bat)

## Acceptance Criteria

- `axi_studio.exe` launches and renders node boxes from `canvas_nodes[]`
- Nodes are mouse-draggable
- Explorer sidebar lists real ISSUE-*.md filenames from `.axi/issues/`
- Camera pan works via middle-mouse or arrow keys
- Compiles cleanly under MinGW with Raylib only (no llama.cpp dependency)
