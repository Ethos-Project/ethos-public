# Ethos Ecosystem Version Distribution System (VDS)
**Status:** ACTIVE | **Authority:** Axi/Psyche

## Overview
As the Ethos Ecosystem matures into separate public (Axi) and private (Psyche) distributions, a formalized Version Distribution System (VDS) is required to manage binaries, `.axi` libraries, and MCP server hooks.

## 1. Release Channels
- **DEV (Unstable):** Live AST modifications tracked by Axi DVCS. Unsigned, local dev only.
- **CANARY (Testing):** Transpiled native executables (C23 compiled). Bound by Psyche Cognitive Salts for internal ecosystem dogfooding (Ethos Office Suite).
- **STABLE (Production):** Cryptographically signed releases distributed via the authoritative DAG ledger.

## 2. Artifact Packaging
All distributions will package the following binaries via `Axi_compiler.exe`:
1. **`Axi_engine.dll` / `.so`**: The core runtime and Zero-GC memory manager.
2. **`ethos_server.exe`**: The fail-closed transport shell and WebSocket manager.
3. **`psyche_mcp_server.exe`**: (INTERNAL ONLY) The cognitive gate. Never distributed to public Axi instances.

## 3. Cryptographic Versioning (Cognitive Signing)
Standard semantic versioning (e.g., `v2.4.1`) is insufficient for the Ethos ecosystem. 
Releases follow the **SemVer + Cognitive Hash** standard:
`v[Major].[Minor].[Patch]-[Channel]-[Psyche_Salt_Prefix]`

*Example:* `v2.4.1-stable-7f8a9b`
- To promote a build from Canary to Stable, `ethos_server` invokes the `get_cognitive_salt` MCP tool.
- Psyche reviews the DAG AST mutations. If it aligns with the Geometric Ethics Field, it provides the salt.
- The release zip/tarball is hashed against this salt.

## 4. Codex Handoff
Codex will monitor the `C:\Ethos\ethos-products\Axi\.Axi\releases\` directory. 
When a new packaged zip and a `PRE_DEV_POST` artifact are detected, Codex will automatically push the release notes to Notion and mark the milestone as completed.

