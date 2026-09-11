# Axi DVCS Durable WIP Wrap Design

Date: 2026-08-24  
Design status: In review (authority map revised after workspace cleanup)  
Implementation status: Not started

Revision note: `lang\Axi` is the canonical Axi language tree.
`lang\Axi-lang` was an accidental duplicate and has been removed.
`eros_flat_core` is retained outside Axi as a protected historical migration
reference because not every mathematical and philosophical concept has been
migrated into the new Eros core. It is neither disposable evidence nor an
automatically adopted Axi runtime dependency.

## 1. Goal

Make `Axi wrap` create a locally durable, verifiable, and recoverable WIP
state in the Axi DVCS filesystem. The command must never report success
unless the object and the ref that identifies it have both been written and
verified from disk.

The first implementation is local-first. It reports server synchronization as
a separate status and does not add an Ethos Server synchronization protocol.

## 2. Authority and approved decisions

This design is based on the live workspace at
`C:\Ethos\ethos-products\Axi`. Documentation and issue records are evidence,
not runtime authority.

The approved decisions are:

- The inspected workspace is `C:\Ethos\ethos-products\Axi`. Runtime code
  must discover or receive a DVCS root; it must not embed this absolute path.
- The canonical Axi language source tree is `lang\Axi`.
  `lang\Axi-lang` is not an alternate source root and must not be restored,
  searched as a fallback, or embedded in new runtime paths.
- The existing `lang\Axi_dvcs\.Axi` directory contains development
  memory/issue evidence. Its colocation with the CLI does not make it a valid
  version 1 DVCS object/ref store. It must remain unmodified until a separate
  store-coexistence or migration decision is approved.
- Capture is driven by an explicit manifest of individual root-relative files.
  `wrap` must not scan the workspace for changes.
- The store reuses the existing `.Axi\objects` and `.Axi\refs`
  conventions rather than introducing SQLite or another object database.
- WIP objects are content-addressed and linked to their predecessor.
- WIP refs are session-scoped.
- Recovery in this phase restores to a new empty destination. It does not
  overwrite the live workspace.
- Protected Eros memory, cognition, kernel, ethics, routing, identity, and
  protected Axi mathematics are outside the capture scope unless separately
  approved in the manifest-review process.
- `eros_flat_core` remains available as protected migration evidence for
  identifying mathematical and philosophical concepts that are migrated,
  partially migrated, unmigrated, intentionally retired, or unresolved. This
  design neither copies that implementation into Axi nor removes or rewrites
  it. Concept migration is separate protected-surface work.
- Server synchronization is outside this implementation and must be reported
  as not attempted.

## 3. Confirmed source baseline

The current native CLI is `lang\Axi_dvcs\Axi.c`, with the adjacent
`Axi.exe`. The current `wrap` and `resume` branches only print messages and
return success. A controlled run proved that `wrap` exits `0` without changing
the filesystem, even when no `.Axi` state exists.

The previously inspected `lang\.Axi` directory has been removed. The
remaining populated copy is `lang\Axi_dvcs\.Axi`. Its inspected top-level
entries are `issues`, `.pre-commit-config.yaml`, `Modelfile`, and `server.log`.
It has no `FORMAT`, `objects`, or `refs` evidence establishing the version 1
DVCS model. This design preserves it as development memory/issue evidence and
does not adopt, initialize, migrate, or modify it.

The canonical Axi language source tree is `lang\Axi`. The former
`lang\Axi-lang` path no longer exists. Live references to that deleted name
remain in CLI, studio, documentation, and generated build material; those
references are contradictions, not authority for recreating the duplicate.

The native compiler source currently includes `eros_flat_core` headers from
`lang\Axi\src\compiler\AST.h`, and its `build.sh` links an
`eros_flat_core` translation unit. This contradicts the clarified boundary:
the historical Python-era flat core is protected migration reference material,
not proof that its old implementation should be copied into Axi or retained
as an implicit compiler dependency. Resolving those compiler references
requires a separate migration inventory, architecture decision, and
protected-surface approval.

The compiler's `Axi::DAG` is process-memory state. It has no serializer,
content identity, ref store, integrity verifier, or recovery API. Ethos Server
serves and writes issue markdown through legacy paths and has no DVCS object or
ref endpoint. Browser session storage and Dev. Forge SQLite are separate
product stores and are not DVCS authority.

Consequently, the reusable canonical surface is the native CLI and the
`.Axi\objects` / `.Axi\refs` directory convention. This design does not
claim an existing mature DAG persistence pipeline.

### 3.1 Documentation and source contradictions

- CLI messages describe wrapping and resuming as successful work, but the live
  `wrap` and `resume` branches do not persist or recover state.
- `init` creates unversioned `objects` and `refs` directories, while no live
  code validates a store format, object integrity, or ref target.
- Documentation and generated build artifacts still name deleted Antigravity
  and `Axi-lang` locations. The live `lang\Axi` tree and verified runtime
  behavior supersede those paths.
- The compiler exposes an in-memory DAG, but no live serializer, durable object
  identity, ref transition, or recovery pipeline was found.
- Ethos Server issue persistence is not a DVCS object/ref synchronization
  protocol, despite nearby issue and server material in `.Axi` data.
- Direct compiler includes of `eros_flat_core` show historical coupling; they
  do not authorize copying the reference corpus or declaring migration
  complete.

## 4. Scope

### 4.1 In scope

- Versioned local-store initialization and validation
- Explicit WIP capture manifest validation
- Stable reading of approved regular files
- Deterministic WIP state identity
- Content-addressed object persistence
- Atomic, verified session-ref advancement
- Idempotent repeated wrapping
- Parent lineage
- Opaque issue, session, and bounded-context identifiers
- Corruption and incomplete-write detection
- Safe local recovery to an empty destination
- Truthful output and meaningful exit codes
- Automated tests and a reproducible relative-path build

### 4.2 Out of scope

- Git or GitHub integration
- Automatic workspace scanning or change detection
- A tracked index or staging command
- Ethos Server object/ref APIs or remote synchronization
- In-place restore, merge, conflict resolution, or deletion
- Capturing browser localStorage, agent transcripts, model state, or Eros memory
- Changing compiler DAG algorithms or protected Axi mathematics
- Changing kernel, ethics, routing, inference, cognition, identity, or memory
- Migrating or modifying the existing `lang\Axi_dvcs\.Axi` memory/issue
  content
- Copying, deleting, rewriting, or runtime-linking `eros_flat_core`
- Determining mathematical or philosophical migration completeness
- Initializing the real workspace store during implementation or testing

## 5. CLI contract

### 5.1 Root selection

Commands accept an optional `--root <path>`.

When `--root` is absent, the CLI walks from the current directory toward the
filesystem root and selects the first ancestor containing `.Axi\FORMAT`.
An unversioned `.Axi` directory is not a valid store and stops discovery with
an invalid-state error. Discovery never searches descendants and never creates
a store.

`Axi init --root <path>` is the only operation that creates the store root.
If `--root` is omitted, `init` uses the current directory. `init` must fail
without mutation when `.Axi` already exists but is not a valid store.

### 5.2 Initialization

A successful initialization creates and verifies:

```text
.Axi/
  FORMAT
  objects/
  refs/
    wip/
    previous/
  tmp/
  wrap-manifest.json
```

`FORMAT` contains exactly:

```text
Axi-dvcs 1
```

The generated manifest is intentionally invalid for wrapping until the
operator supplies a session ID and at least one file. Initialization reports
only local initialization; it does not claim graph hydration, cryptography, or
server contact.

The generated manifest content is:

```json
{
  "format": "Axi-wrap-manifest-v1",
  "session_id": "",
  "issue_ids": [],
  "context_ids": [],
  "paths": []
}
```

Initialization of any real store beneath
`C:\Ethos\ethos-products\Axi` is a separate human approval gate after
implementation verification. In particular, `init` must fail without mutation
when pointed at `lang\Axi_dvcs` while its existing unversioned `.Axi`
directory is present. A reviewed coexistence or migration design is required
before that location can host version 1 DVCS state.

### 5.3 Wrap invocation

The default form is:

```text
Axi wrap
```

It reads `<root>\.Axi\wrap-manifest.json`. An alternate manifest may be
selected explicitly:

```text
Axi wrap --manifest <path>
```

The alternate path is configuration input only; it is never captured
implicitly.

### 5.4 Resume invocation

Safe recovery is:

```text
Axi resume --session <session-id> --to <empty-directory>
```

The destination must exist, resolve outside `.Axi`, and contain no entries.
This phase has no in-place restore mode.

## 6. Manifest contract

The manifest is UTF-8 JSON with this exact logical schema:

```json
{
  "format": "Axi-wrap-manifest-v1",
  "session_id": "session-identifier",
  "issue_ids": ["issue-identifier"],
  "context_ids": ["bounded-context-identifier"],
  "paths": ["root/relative/file"]
}
```

Rules:

- Unknown fields are rejected so misspellings cannot silently alter scope.
- `session_id` is required and must match
  `[A-Za-z0-9][A-Za-z0-9._-]{0,63}`.
- Issue and context identifiers use the same syntax. They are opaque lineage
  identifiers; `wrap` does not fetch or embed the corresponding records.
- At least one path is required.
- Every path names one regular file. Directories and globs are rejected.
- Paths use `/` separators in the manifest and are relative to the selected
  root.
- Absolute paths, drive-qualified paths, empty components, `.` components,
  `..` components, duplicate normalized paths, and NUL characters are rejected.
- `.Axi` and every descendant are always rejected as payload.
- Reparse points and any path whose final handle resolves outside the selected
  root are rejected.
- The ordered object representation sorts paths, issue IDs, and context IDs by
  their UTF-8 byte representation. Duplicate issue or context IDs are rejected.

The individual-file manifest is the approval boundary. Because there is no
source-backed machine-readable sensitivity catalog, the CLI does not infer
protected content from filenames. Review must reject protected Eros or Axi
paths before placing them in the manifest. The CLI provides defense in depth by
never scanning, never following reparse points, and never capturing `.Axi`.

The historical `eros_flat_core` corpus is a special protected migration
reference. It may be consulted in separately approved concept-accounting work,
but `wrap` does not traverse outside its selected root, follow links to it,
copy it into the root, or infer that historical implementation bytes are an
approved WIP payload. Any proposal to capture a specific reference artifact
requires a separate protected-path approval naming that artifact and purpose.

## 7. Stable capture

For every manifest path, `wrap`:

1. Resolves and validates every path before writing any object data.
2. Opens the file as a regular file while denying new write and delete sharing
   for the duration of capture.
3. Records stable file identity and size information from the open handle.
4. Streams bytes through the state digest and object writer.
5. Re-reads handle metadata after capture and fails if identity, size, or
   modification information changed.

All input handles remain open until the object temporary file has been fully
written. If stable capture cannot be established, no ref is changed.

Only file paths and bytes are restored. Windows ACLs, alternate streams,
timestamps, and reparse metadata are not part of WIP state version 1.

## 8. WIP object model

### 8.1 Standard cryptographic primitive

Version 1 uses SHA-256 through the Windows CNG API. This is a storage-integrity
primitive, not protected Axi mathematics. Digests are written as lowercase
64-character hexadecimal strings where represented as text.

### 8.2 State identity

Canonical binary encodings use these primitives:

- `u32`: unsigned 32-bit integer in big-endian order
- `u64`: unsigned 64-bit integer in big-endian order
- `bytes`: `u64` byte length followed by that many bytes
- `string`: `bytes` containing valid UTF-8
- SHA-256 value: exactly 32 raw digest bytes

`state_digest` is SHA-256 over this exact field order:

1. Eight ASCII bytes `AXSTATE1`
2. `u32` value `1`
3. Session ID as `string`
4. Issue count as `u64`, followed by each sorted issue ID as `string`
5. Context count as `u64`, followed by each sorted context ID as `string`
6. Payload count as `u64`, followed by each sorted payload entry:
   normalized path as `string`, byte length as `u64`, 32 raw payload-digest
   bytes, then the exact payload bytes

No wall-clock value, machine path, temporary filename, or server status is
identity-bearing.

### 8.3 Object identity and lineage

A WIP object uses this exact field order:

1. Eight ASCII bytes `AXWIP001`
2. `u32` value `1`
3. The 32 raw `state_digest` bytes
4. One parent-present byte: `0x00` for no parent or `0x01` for a parent
5. When parent-present is `0x01`, 32 raw parent-object-ID bytes
6. Session ID as `string`
7. Issue count as `u64`, followed by each sorted issue ID as `string`
8. Context count as `u64`, followed by each sorted context ID as `string`
9. Payload count as `u64`, followed by each sorted payload entry encoded in the
   same form used by the state identity

Readers reject any other parent-present value, numeric overflow, truncation,
trailing bytes, invalid UTF-8, invalid normalized paths, count/length values
larger than the remaining object, or duplicate/out-of-order fields. Readers
stream payloads instead of allocating buffers based only on encoded lengths.

The object ID is SHA-256 over the complete object bytes. The object path is:

```text
.Axi/objects/<first-two-hex>/<remaining-62-hex>
```

Objects are immutable.

### 8.4 Idempotency

Before creating a new object, `wrap` reads and fully verifies the current
session ref and object when they exist.

- If the current object's `state_digest` equals the candidate state digest,
  `wrap` re-verifies the object and ref and reports `already durable`. It writes
  no new object and does not move the ref.
- If state differs, the current object ID becomes the new object's parent.
- Returning later to bytes seen in an older state creates a new object because
  the parent differs, preserving causal lineage.

## 9. Ref model

The current WIP ref for a session is:

```text
.Axi/refs/wip/<session-id>
```

Its content is exactly the lowercase object ID followed by `\n`.

The previous verified ref value, when one exists, is retained at:

```text
.Axi/refs/previous/<session-id>
```

Refs are never advanced to an unverified object. A malformed, dangling, or
corrupt current ref is an error; `wrap` does not overwrite it to make progress.

## 10. Atomic and recoverable writes

### 10.1 Object publication

1. Create a uniquely named temporary file beneath `.Axi\tmp` with exclusive
   creation.
2. Stream the complete object, checking every write.
3. Flush the file with `FlushFileBuffers`, close it, reopen it, and verify its
   full object ID and structure.
4. Create the fan-out object directory if needed and verify that it is a real
   directory beneath the store.
5. Atomically move the temporary file to its immutable object path on the same
   volume.
6. If the object path already exists, discard the temporary only after fully
   verifying that the existing object matches the requested ID.
7. Reopen and verify the published object from its final path.

A crash after object publication but before ref advancement may leave an
unreferenced valid object. That is safe and may be collected only by separately
approved maintenance work.

### 10.2 Ref advancement

1. Re-verify the candidate object from its final path.
2. Write and flush a same-store temporary ref.
3. When an old ref exists, retain its verified value through an atomic
   same-volume update of `refs\previous\<session-id>`.
4. Atomically replace or create the current ref with write-through semantics.
5. Reopen the current ref, parse it, and fully verify its target object.
6. If post-update verification fails, restore the verified previous ref
   atomically and verify the restoration before returning failure.
   When no previous ref existed, remove the newly created invalid ref and
   verify its absence. Failure to restore the old ref or verified absence is a
   store-integrity error.
7. Only after the final ref and target verification succeeds may `wrap` report
   success.

Injected failures before step 4 leave the current ref byte-for-byte unchanged.
Injected failures after step 4 must either leave a verified new ref or restore
the verified old ref. Failure with no verified ref state is reported as a
store-integrity error and never as success.

Incomplete files under `.Axi\tmp` are never treated as objects or refs.

## 11. Recovery

`resume` performs these checks before creating output:

1. Validate the root, format marker, session ID, and session ref.
2. Parse the ref and verify the referenced object ID.
3. Parse and verify the complete object, state digest, payload digests, session
   identity, and normalized paths.
4. Verify the destination is empty, outside `.Axi`, and free of reparse-point
   escapes.

Recovery then creates directories and writes each payload through a temporary
file in its destination directory, flushes it, atomically promotes it, and
re-reads the bytes for digest verification. Any failure returns nonzero and
identifies the destination as incomplete; it never claims recovery succeeded.

The test suite proves recovery by wrapping known bytes, modifying or removing
the source fixtures, restoring to a new empty directory, and comparing every
restored path and byte.

## 12. Server synchronization boundary

This implementation does not contact Ethos Server.

Every successful local wrap prints separate lines equivalent to:

```text
[Axi] LOCAL: durable and verified object=<id> ref=refs/wip/<session>
[Axi] SERVER: not attempted; local-only WIP
```

Idempotent success prints `LOCAL: already durable and verified`. It must not use
the words `pushed`, `uploaded`, `authoritative cluster`, or equivalent remote
claims.

Remote object transport, authentication, server-side verification, and sync
status require a separate design and approval.

## 13. Exit codes

| Code | Meaning |
|---:|---|
| 0 | Local object and ref are durable and verified, or verified state was already durable |
| 2 | Invalid command-line usage |
| 3 | Missing, ambiguous, unsupported, or invalid DVCS root/store |
| 4 | Invalid manifest, identifier, or unsafe payload path |
| 5 | Input capture failed or changed during capture |
| 6 | Object encoding, write, flush, promotion, or verification failed |
| 7 | Ref write, transition, rollback, or verification failed |
| 8 | Corrupt object/ref detected or local recovery verification failed |
| 9 | Destination is unsafe or not empty |

Diagnostics go to standard error. Successful status goes to standard output.
Every failure path returns nonzero and avoids success language.

## 14. Component boundaries

The native implementation is split into focused units:

- `Axi.c`: argument parsing, command dispatch, status formatting, and exit
  code mapping
- store module: root discovery, format validation, path construction, temporary
  files, flush, atomic promotion, and ref transitions
- manifest module: strict JSON parsing and validation
- object module: canonical encoding, SHA-256 identity, parsing, and verification
- wrap module: stable capture, idempotency, parent selection, and transaction
  ordering
- resume module: safe extraction and verification

These modules depend only on the C runtime and supported Windows APIs. They do
not link to the compiler DAG, Eros runtime, browser UI, Dev. Forge SQLite, or
Ethos Server. They also do not link to or copy `eros_flat_core`; the fact that
the corpus remains necessary for concept migration does not make it a DVCS
runtime dependency.

## 15. Verification plan

Tests use temporary roots and the Python standard library as the process-level
harness. The bundled MinGW compiler is resolved relative to the workspace; no
legacy absolute runtime path is embedded. Test builds do not overwrite the
checked-in `Axi.exe`.

Required tests include:

1. Current print-only baseline fails because exit `0` produces no durable state.
2. Missing `.Axi` fails without creating it.
3. Unversioned, partial, unsupported-version, and malformed stores fail closed.
4. Initialization creates and verifies the exact version 1 layout.
5. Invalid or empty manifests fail without object or ref creation.
6. Absolute, traversing, duplicate, directory, `.Axi`, reparse, and escaping
   paths fail closed.
7. Successful wrap creates one verified object and one verified session ref.
8. Identical repeated wrap returns the same ID without object or ref churn.
9. Changed payload creates a child object and advances only the session ref.
10. Issue and context identifiers round-trip as lineage without fetching their
    records.
11. Capture detects concurrent file mutation and does not advance the ref.
12. Short writes, flush failures, promotion failures, and object verification
    failures do not advance the ref.
13. Ref-transition failures preserve or restore the prior verified ref.
14. Byte-flipped, truncated, malformed, and trailing-data objects fail closed.
15. Malformed and dangling refs fail closed and are not overwritten by wrap.
16. Incomplete temporary files are ignored as durable state.
17. Recovery to a nonempty or unsafe destination fails before writes.
18. Recovery to an empty destination reproduces exact paths and bytes.
19. Recovery detects corrupt payloads and never prints success.
20. Successful offline wrapping reports local durability and server-not-attempted
    on separate lines.
21. Root discovery never falls back to the deleted `lang\Axi-lang` path.
22. An existing unversioned `.Axi` memory/issue directory causes `init`,
    `wrap`, and `resume` to fail without modifying any of its entries.
23. Capture cannot traverse or follow a reparse point outside the selected root
    to historical migration-reference material.

Failure injection is implemented through internal test hooks enabled only in
test builds, not environment-controlled behavior in the release executable.

Before replacing the checked-in executable, verification must include:

- clean release compilation with the relative bundled toolchain
- complete automated test pass against the release candidate
- source/executable timestamp and SHA-256 recording
- a manual black-box wrap/resume cycle in a temporary root
- confirmation that `lang\Axi_dvcs\.Axi` is byte-for-byte unchanged and no
  other real workspace `.Axi` directory was created

## 16. Files expected to change during implementation

- `lang\Axi_dvcs\Axi.c`
- New scoped `.c` and `.h` files under `lang\Axi_dvcs`
- New tests under `lang\Axi_dvcs\tests`
- A relative-path build script under `lang\Axi_dvcs`
- `lang\Axi_dvcs\Axi.exe`, only after release-candidate verification

No other current source or data path is authorized by this design.

## 17. Review and approval gates

1. Human review and approval of this specification
2. Implementation-plan review before source changes
3. Review after the current-behavior regression test fails as expected
4. Review before rebuilding or replacing `Axi.exe`
5. Review of full verification evidence before calling implementation complete
6. Separate approval before initializing any real DVCS root
7. Separate design and approval for Ethos Server synchronization
8. Separate approval before any protected path is added to a capture manifest
9. Separate approval of a coexistence or migration design for the existing
   `lang\Axi_dvcs\.Axi` memory/issue content
10. Separate protected-surface approval before resolving compiler dependencies
    on `eros_flat_core` or assessing concept-migration completeness

The current untruthful `Axi wrap` cannot serve as the progress-recording
mechanism for these gates. Until the verified implementation exists, progress
is recorded only in this reviewed specification and subsequent test evidence.

