# Ethos Ecosystem Directory and Authority Standard

Status: ACTIVE (Business Organized)
Date: 2026-08-27  
Scope: `C:\Ethos` (Top-Level Ecosystem)

## 1. Purpose

This document defines the official business structure of the Ethos Ecosystem. The repository architecture has been reorganized to clearly separate cognitive AI engines, governance standards, and distributable software products.

## 2. The Ecosystem Triad

The top-level `C:\Ethos` directory is strictly divided into three pillars:

| Pillar | Path | Responsibility | Prohibited Responsibility |
| --- | --- | --- | --- |
| **Logos (Mind)** | `ethos-logos/` | The Cognitive Core. Houses the Psyche engine (`cogni-core`), agent logic, and internal ML models. | Product UI, public language compilers, network transport. |
| **Nomos (Law)** | `ethos-nomos/` | Governance, documentation, and internal automation. Contains `PRE/DEV/POST` scripts and Slack webhooks (`tools/`). | Active application runtime logic or compiled binaries. |
| **Products (Body)** | `ethos-products/` | The shipped software. Contains `Axi`, `ethos-office-suite`, and public-facing UIs. | Proprietary cognitive weights or internal company governance. |

## 3. Axi Product Architecture (`ethos-products/Axi/`)

The `Axi` directory is the canonical compiler, DVCS engine, and server shell. It has been strictly reorganized into business-standard directories:

| Path | Canonical Responsibility |
| --- | --- |
| `apps/` | Web/UI endpoints and Javascript backends (e.g., `server.ts`, `dvcs.ts`). |
| `bin/` | Compiled executables (`Axi_compiler.exe`, `ethos_server.exe`, `domus.exe`). |
| `lib/` | Shared dynamic libraries (`raylib.dll`, `ggml.dll`, `libllama.dll`). |
| `src/` | Pure source code (`.axi`, `.c`, `.h`). Divided into `compiler/`, `kernel/`, and `ipc/`. |
| `src/ipc/` | The bridge! Contains `eros_Axi_mcp.axi` and `lea_core.h`. The boundary where Axi talks to Psyche. |
| `tests/` | Unit tests (`test_*.axi`). |
| `tools/` | Build scripts, python helpers, and patching tools (`build_native.bat`). |
| `docs/` | Official architecture specs, implementation plans, and `PRE/DEV/POST` archives. |
| `.Axi/` | The official DVCS cryptographic ledger, object store, and `releases/` manifest. |

## 4. Authority & Version Distribution

1. **Axi DVCS** controls authoritative project provenance and version state.
2. **Cognitive Salting**: Official `STABLE` releases in `.Axi/releases/` must be cryptographically bound to Psyche's Geometric Ethics Field via the IPC/MCP bridge.
3. **Codex / Notion**: Notion remains the human-facing source of truth for planning. Codex consumes `PRE/DEV/POST` handoffs directly via Slack integrations (`Invoke-CodexSlackSync.ps1`) instead of polling repo files.
4. **Git/GitHub**: Are optional historical mirrors only. They never override Axi DVCS.

## 5. Role boundary

- **Codex**: Acts as Engineering Project Manager for discovery, architecture, documentation, Notion work records, review synthesis, and implementation handoffs. Does not edit runtime code.
- **Antigravity**: Owns heavy implementation work from an approved, bounded work item. Executes code generation, compiler management, and DVCS orchestration.
- **Psyche (Eros)**: The Cognitive Gate. Governs commit hashes, verifies AST mutations, and serves as the autonomous internal agent running in `ethos-logos`.

