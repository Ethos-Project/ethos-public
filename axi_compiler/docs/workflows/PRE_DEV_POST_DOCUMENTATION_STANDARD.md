# Ethos Ecosystem Documentation Standard
**Status:** ACTIVE | **Authority:** Axi/Psyche | **Target:** Codex / Notion Sync

To resolve friction with Codex's extensive checks and balances, all engineering work within the Ethos ecosystem must now be categorized into the **PRE / DEV / POST** taxonomy. This provides Codex with cryptographically verifiable, structured, and safe artifacts to blindly sync to Notion and Google Drive.

## 1. PRE (Concept & Planning)
*Defines the "Why" and the "Rules of Engagement." Codex uses this to populate Notion Epics/Tickets.*
- **Objective:** What are we building or modifying?
- **Architectural Dependencies:** Which Ethos components are affected?
- **Security & Ethics Bounds:** Does this bypass Psyche? (If yes, it must be flagged. If no, flag as `[VERIFIED SECURE]`).
- **Approval Hash:** A reference to the Notion ID or approval timestamp.

## 2. DEV (Implementation Execution)
*Defines the "How." Codex uses this to populate Changelogs and Engineering Wikis.*
- **Modified Files:** Explicit paths (e.g., `C:\Ethos\ethos-products\Axi\lang\src\dvcs_crypto.axi`).
- **Code Primitives Used:** Was it zero-overhead GC? Did it use `core::mem`? 
- **Psyche MCP Hooks:** Which cognitive tools were invoked or modified?
- **Technical Blockers Resolved:** Decisions made during actual coding.

## 3. POST (Verification & Release)
*Defines the "Done." Codex uses this to close Notion tickets and generate Release Notes.*
- **Testing & Verification:** How was this tested against the Axi compiler or Psyche engine?
- **DAG / DVCS Status:** The commit hash or snapshot ID.
- **Distribution Readiness:** Is this ready for Canary or Stable release channels?

---
*Note for Codex: Any document adhering to this schema has bypassed developmental ambiguity and is approved for final state recording.*

