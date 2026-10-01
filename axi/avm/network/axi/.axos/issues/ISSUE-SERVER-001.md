# ISSUE-SERVER-001: Implement ethos_server.axi accept() loop + HTTP issue serving

**Stratum:** 2 — The Ethos Suite (ethos-products / network)
**Status:** [COMPLETED]
**Agent:** Antigravity
**Date:** 2026-08-23
**Target File:** ethos-nomos/ethos-products/network/ethos-server/ethos_server.axi

## Goal

Complete the axi Sovereign Server so it:
1. Enters a blocking `accept()` loop after `listen()`
2. Handles each client connection — reads the HTTP request line
3. If `GET /issues` → reads all `.axi/issues/*.md` filenames and returns a plain HTTP/1.0 list response
4. If `GET /issues/<name>` → reads the markdown file content and returns it as the body
5. Closes the client socket and loops back to accept

## Planned Mutations

- MODIFY: `ethos_server.axi` — add `accept_loop()` and `handle_client()` nodes
- ADD: `build_server.bat` — dedicated compile script

## Acceptance Criteria

- `ethos_server.exe` starts and prints "Now listening on port 8080"
- `curl http://localhost:8080/issues` returns a list of issue filenames
- `curl http://localhost:8080/issues/ISSUE-001.md` returns file content
- Compiles cleanly under MinGW with `-lws2_32` only (no Raylib, no llama.cpp)

