# Debugger Roadmap

This document records the plan for making Nerd programs debuggable from VS Code.
It follows the same role as `ROADMAP.md`: current direction, active milestones,
and implementation rules. Update it as decisions are made and as implementation
facts replace assumptions.

## Goal

The first useful debugger experience should let a Nerd programmer:

- start a Nerd program from VS Code
- set source breakpoints in `.n` files
- step through Nerd source lines
- inspect locals and parameters
- use the VS Code Watch panel for ordinary Nerd expressions where practical
- see call stacks in terms of Nerd functions and source files

Breakpoints and watches are the minimum user-visible bar. A debugger that only
starts the native executable is not enough.

## Current State

- The VS Code extension owns `.n` file registration, TextMate grammar wiring,
  LSP launch, formatting, indentation, and `nerd.restartLanguageServer`.
- The VS Code extension can build the active Nerd file and start a Nerd
  `type: "nerd"` debug session that delegates to CodeLLDB through
  `nerd.debugActiveFileWithCodeLLDB`.
- The compiler backend lowers HIR to textual LLVM IR and invokes clang.
- Non-release executable builds emit Nerd source debug metadata and pass
  `-g -O0` to clang.
- Linux debug executables now produce usable DWARF line tables for a larger
  multi-module program such as `examples/dungeon/dungeon.n`.
- Release builds currently omit the Nerd debug metadata product and pass `-O2`
  to clang.
- Generated LLVM now has an initial debug metadata contract for compile units,
  source files, function subprograms, and statement line locations.
- Generated LLVM now emits first-pass structure debug types for stack locals
  such as `string`, slices, tuples, and plexes so native debugger watches can
  inspect ordinary fields.
- Debug builds materialize composite `let` locals into stack slots so values
  introduced with `:=`, such as strings, slices, plexes, tuples, and fixed-size
  arrays, can be inspected by LLDB.
- CodeLLDB's bundled LLDB has been validated against a small watch program:
  it can stop on a Nerd source line, show a `string` local as `data/count`, show
  a plex local by field name, evaluate a field watch such as `point.x`, and
  index through typed pointer data such as `message.data[0]`, display and
  index fixed-size stack arrays such as `values[1]`, and inspect/index
  stack-backed slices such as `slice.data[1]`, and index dynamic-array data
  pointers such as `numbers[1]`.
- Linux debug executables expose Nerd globals by source name through DWARF
  global-variable metadata.
- The Nerd VS Code debug shim retries failed dynamic-array `.count`,
  `.capacity`, and `.data` watches with runtime-header expressions.
- Function parameters emitted with `dbg.value` are marked as DWARF arguments so
  CodeLLDB exposes ordinary arguments such as `std.random.random_seed(seed)`.
- Nerd-visible function bindings are emitted as `$` aliases while generated
  function bodies use compiler-internal names such as `@fn.N`.
- The runtime object is compiled from `data/nrt.c` and linked into generated
  executables.
- Command tests already treat `.pdb` files as generated debug artefacts on
  platforms that produce them.

## Working Principles

- Treat source-level debugging as a compiler feature, not only an editor
  feature.
- Preserve the current backend contract: textual LLVM IR plus clang until
  measurement or debugger requirements justify changing it.
- Keep debug metadata derived from semantic/HIR facts, not parser guesses.
- Make line tables work before variable inspection, then make variable
  inspection work before expression watches.
- Keep generated implementation symbols stable enough for debugger mapping, but
  do not expose generated names as the user-facing debugging surface.
- Add command and editor smoke tests for each public debugger workflow.
- Keep Linux/LLDB-style DWARF support as the first validation path unless a
  task explicitly targets Windows first.
- Treat Linux and Windows 11 as required debugger targets. Linux lands first
  because it is the current development host; Windows 11 follows as soon as the
  debug metadata and VS Code launch shape are proven.
- Do not claim broader cross-platform debugging until Linux, Windows 11, and
  any later supported hosts have platform-specific verification.

## Proposed Architecture

### Compiler Debug Product

Add an explicit debug-information product to the backend. In debug builds, LLVM
generation should be able to emit source-level metadata for:

- compile unit and source file paths
- function declarations and lexical scopes
- statement/source line locations
- parameters and local variables
- primitive, pointer, string, slice, dynamic-array, tuple, plex, union, and enum
  type descriptions as support lands

Debug information is part of the normal non-release build contract. There
should not be a separate user-facing flag to request source-level debug
metadata. Release builds may omit debugger metadata unless a later explicit
release-with-debug-info mode is designed.

On Linux, debug information should be emitted into the executable produced by
`nerd build` or by a kept `nerd run --keep` binary. Do not require separate
sidecar debug files for the first Linux path.

### Debug Launch Surface

Add a Nerd-specific VS Code debug configuration that builds the current Nerd
program, then launches it through a native debug adapter.

Initial proposed shape:

```json
{
    "type": "nerd",
    "request": "launch",
    "name": "Debug Nerd Program",
    "program": "${file}",
    "args": [],
    "cwd": "${workspaceFolder}"
}
```

The extension can translate that into:

1. run `nerd build <program> --output <workspace debug path>`
2. start the platform debugger adapter against the produced executable
3. map VS Code breakpoints in `.n` files to native source breakpoints

The open design question is whether `type: "nerd"` should:

- wrap an existing native debug adapter such as LLDB/GDB from the extension, or
- implement the Debug Adapter Protocol directly and drive LLDB/GDB/MI under the
  hood.

Prefer wrapping an existing native adapter for the first proof if source
breakpoints, stepping, locals, and basic watches work through emitted debug
metadata. Build a Nerd-owned adapter only when expression watches, value
formatting, or source mapping cannot be made good enough through native adapter
configuration.

On Linux, use CodeLLDB as the first VS Code validation path. The first proof
should make CodeLLDB stop on a Nerd source line using debug information emitted
by the compiler into the executable. Keep a Nerd-owned Debug Adapter Protocol
adapter out of the first slice unless CodeLLDB cannot consume the generated
metadata well enough.

The current Linux-first bridge is a small VS Code debug adapter shim.
`Nerd: Debug Active File with CodeLLDB` builds the active `.n` file to
`_tmp/debug/` and starts a Nerd `type: "nerd"` launch that delegates to
CodeLLDB. A checked-in `launch.json` can also use `"program":
"${command:nerd.buildActiveFileForDebug}"` with `type: "nerd"`. This keeps the
user workflow usable while preserving the option to grow into a fuller Nerd
adapter later.

### Watch Expressions

Watch support has two levels:

- Native watches: let the underlying debugger evaluate names and simple C-like
  expressions against emitted debug symbols.
- Nerd watches: parse Nerd expressions, resolve them against the current frame,
  and translate the supported subset to native debugger expressions or runtime
  inspection requests.

The first milestone should support locals, parameters, field access, pointer
dereference, indexes, and simple arithmetic when the underlying debugger can
evaluate them. Full Nerd expression evaluation is deferred until the compiler
has a reusable expression evaluator or interpreter.

The first proven subset is native CodeLLDB evaluation of in-scope locals,
parameters, and simple field access backed by emitted DWARF type metadata. This
currently covers primitive locals, `string` and slice values, plex/tuple
fields, fixed-size stack array indexing, and dynamic-array data pointer
indexing, including composite locals introduced with `:=`. Typed pointer
indexing through `string.data` and `slice.data` is proven. Dynamic-array
`.count`/`.capacity` rendering, general pointer dereference/indexing, and
Nerd-owned expression parsing remain open.

### Value Rendering

Native debuggers will not naturally know how to display higher-level Nerd
values. Add value-rendering support in layers:

- make primitive locals and parameters visible
- expose strings and slices as pointer/count values
- add VS Code/debugger pretty rendering for `string`, slices, dynamic arrays,
  enums, tuples, and plexes
- render tagged enum values with both the symbolic variant tag and integer
  discriminant, and interpret the payload using the active tag's payload type
- keep raw pointers and raw unions honest rather than hiding their low-level
  nature

Pretty rendering may live in the VS Code extension, debugger adapter, LLDB/GDB
scripts, or runtime helper metadata. The location is an open decision.

## Implementation Start

The first vertical slice is deliberately narrow:

1. build one single-file Nerd program in normal non-release mode
2. inspect the executable with Linux debug-info tools
3. open the executable in LLDB or CodeLLDB
4. set a breakpoint in the `.n` source file
5. stop on the expected Nerd source line

Do this before adding VS Code automation. If command-line LLDB cannot see the
right source file and line, the extension cannot make the experience reliable.

Initial Linux validation commands should include:

```sh
./_bin/nerd-debug build _tmp/debug_smoke.n -o _tmp/debug_smoke --llvm
readelf --debug-dump=decodedline _tmp/debug_smoke
lldb _tmp/debug_smoke
```

The expected first failure is that `-g -O0` is already passed to clang, but the
generated LLVM lacks Nerd source-level `!dbg` metadata. MS1 starts by filling
that gap.

## Active Milestones

### MS1: Debug Metadata Proof

- [x] Add a tiny single-file smoke program for manual debugger validation.
- [x] Capture the current baseline with `readelf` and LLDB/CodeLLDB notes.
- [x] Make normal non-release builds emit Nerd source debug metadata.
- [x] Keep release builds free to omit debug metadata under the current
  `--release` contract.
- [x] Emit LLVM compile-unit metadata for the root module and imported modules.
- [x] Emit source locations for function entry, statements, and expression
  lowering points that can carry stable line information.
- [x] Emit function metadata that names Nerd functions, not only generated
  `fn.N` implementation symbols.
- [x] Verify breakpoints bind in VS Code or a command-line native debugger for a
  single-file program.
- [x] Verify CodeLLDB stops on the expected `.n` line on Linux.
- [x] Verify CodeLLDB stops on the expected `.n` line in
  `examples/dungeon/dungeon.n`.
- [x] Add a command-level regression that proves debug builds keep the expected
  binary when requested and that Linux debug information is present in that
  executable.
- [x] Document exactly which Linux debugger and VS Code adapter were validated.

### MS2: Locals, Parameters, And Call Stack

- [x] Emit parameter debug metadata.
- [x] Emit local-variable debug metadata for HIR locals and mutable storage.
- [ ] Preserve lexical scopes enough for shadowed locals to display correctly.
- [x] Verify call stacks show Nerd function names and source locations.
- [x] Verify locals and parameters display at breakpoints.
- [x] Verify locals and parameters remain coherent while stepping.
- [x] Add focused compiler tests for generated LLVM debug metadata shape.

### MS3: VS Code Launch Integration

- [x] Add VS Code `debuggers` contribution metadata for `type: "nerd"`.
- [x] Add a documented CodeLLDB `launch.json` configuration for the active
  `.n` file.
- [x] Build the Nerd program before launching the debugger.
- [x] Surface compiler diagnostics in VS Code when the debug build fails.
- [x] Resolve the compiler executable/tool path using workspace, user install,
  and PATH rules similar to the language-server lookup.
- [x] Add editor integration checks for debugger contribution metadata.
- [ ] Keep the launch configuration schema platform-aware so Linux and Windows
  11 can use different native debugger adapters without changing the user-facing
  Nerd launch type.

### MS4: Breakpoint Quality

- [x] Validate breakpoints in root files and imported module root files.
- [x] Validate debug line-table source paths in folder-module part files.
- [x] Validate breakpoints in folder-module part files from CodeLLDB's bundled
  LLDB.
- [x] Handle breakpoints on declarations, blank lines, comments, and lowered
  multi-statement lines predictably.
- [x] Make rejected or moved breakpoints understandable in VS Code.
- [x] Add a Linux CodeLLDB-bundled LLDB smoke check for source step-over
  staying in Nerd source on a simple function.
- [x] Verify step-over does not stop primarily in generated runtime glue on a
  larger program such as `examples/dungeon/dungeon.n`.
- [ ] Keep source paths stable when the program is built from inside VS Code,
  from the command line, and from installed `nerd`.

### MS5: Watch Expressions

- [x] Define the supported first watch-expression subset.
- [x] Support locals and parameters by name.
- [x] Support field access for plexes, tuples, strings, and slices.
- [x] Support dynamic-array `.count` and `.capacity` field rendering.
- [x] Support native collection indexing for strings, slices, fixed arrays, and
  dynamic-array data pointers where the runtime representation is known.
- [ ] Support general pointer dereference and indexing where the runtime
  representation is not already described by emitted debug metadata.
- [ ] Reject unsupported watch expressions with clear debugger UI messages.
- [x] Add tests or scripted debugger smoke checks for watch evaluation.

### MS6: Nerd Value Rendering

- [x] Render `string` values as text summaries with expandable data/count.
- [x] Render dynamic arrays declared in Nerd source as expandable indexed
  collections in the VS Code Variables tree.
- [ ] Render slices as expandable indexed collections.
- [x] Render tagged enums with both symbolic variant names and integer
  discriminants, and decode `payload` according to the active tag's payload
  type instead of showing only the raw storage integer.
- [x] Render tuples and plexes as expandable structured values.
- [ ] Render raw unions and pointers in a low-level form.
- [ ] Decide whether pretty rendering belongs in extension TypeScript, adapter
  code, debugger scripts, or emitted metadata.

### MS7: Windows 11 Debugging

- [ ] Validate the backend's non-release debug artefacts on Windows 11.
- [ ] Decide whether the Windows path uses CodeView/PDB, DWARF, or another
  clang-supported debug format for the selected host toolchain.
- [ ] Validate breakpoints, stepping, call stacks, locals, and watches from VS
  Code on Windows 11.
- [ ] Make generated artefact cleanup handle Windows debug outputs such as PDBs
  without deleting files needed by an active debug session.
- [ ] Document Windows debugger prerequisites and path lookup.
- [ ] Add Windows-capable smoke checks where practical.

### MS8: Additional Hosts

- [ ] Validate macOS with the system debugger constraints if macOS becomes a
  supported target for this feature.
- [ ] Document unsupported combinations explicitly.
- [ ] Add install smoke checks for debugger launch prerequisites where practical.

## Open Decisions

- Should a future release-with-debug-info mode exist, and what should it be
  called?
- How should the extension discover and delegate to CodeLLDB on Linux?
- Which native debugger/debug format should Windows 11 use?
- Does the VS Code extension depend on an existing debugger extension, discover
  an installed native adapter, or ship a small Nerd adapter?
- How much Nerd watch-expression evaluation should be implemented before a
  reusable compile-time/runtime expression evaluator exists?
- Should debug metadata use generated function body names, Nerd-visible aliases,
  or both?
- How should source locations map for folder-module part expansion and generated
  runtime wrappers?

## Testing Strategy

- Use LLVM snapshot tests for metadata that must stay stable.
- Use command tests for non-release debug metadata and artefact cleanup.
- Use `python3 build/check_debugger_smoke.py --nerd _bin/nerd-debug` for the
  Linux CodeLLDB-bundled LLDB breakpoint/watch smoke path.
- Use `python3 build/check_debugger_stepping.py --nerd _bin/nerd-debug` for
  the Linux CodeLLDB-bundled LLDB source step-over, call-stack, and
  locals/parameters smoke path.
- Extend `build/check_editor_integrations.py` to verify VS Code debugger
  contribution metadata and launch configuration snippets.
- Use `python3 build/check_debugger_adapter_transforms.py` for adapter-side
  value-rendering transform coverage.
- Keep manual testing notes in this file until they can be automated.

## Commit Plan

Keep these commits small and reviewable. Mark each item when the commit lands.

### Commit 1: Debugger Roadmap

- [x] Add this debugger roadmap with Linux-first and Windows 11-required scope.
- [x] Record CodeLLDB as the first Linux VS Code validation path.
- [x] Record normal non-release builds as the debug-info path.

Verification:

- [x] Documentation-only change reviewed locally.

### Commit 2: Linux Baseline Smoke

- [x] Add a minimal debugger smoke source program or fixture.
- [x] Add notes or a script showing the current `readelf` and LLDB baseline.
- [x] Confirm current executables do not yet expose usable Nerd source lines.

Verification:

- [x] `./_bin/nerd-debug build <smoke.n> -o <smoke-binary> --llvm`
- [x] `readelf --debug-dump=decodedline <smoke-binary>`
- [x] command-line LLDB breakpoint attempt recorded.

### Commit 3: Minimal Line Tables

- [x] Emit LLVM module debug flags.
- [x] Emit compile-unit and file metadata for root and imported modules.
- [x] Emit function `DISubprogram` metadata.
- [x] Attach `DILocation` metadata to enough instructions for source
  breakpoints.

Verification:

- [x] Focused LLVM snapshot test for debug metadata shape.
- [x] `readelf --debug-dump=decodedline <smoke-binary>` shows `.n` source
  lines.
- [x] LLDB can bind and stop at one smoke-program breakpoint.

### Commit 4: CodeLLDB Manual Proof

- [x] Add a documented manual CodeLLDB launch configuration for the smoke
  binary.
- [x] Verify CodeLLDB stops on a Nerd source line.
- [x] Record the CodeLLDB version and Linux host details used for validation.

Verification:

- [x] Manual VS Code CodeLLDB breakpoint proof completed.
- [x] Notes added to this document or a linked debugger validation note.

### Commit 5: Locals And Parameters

- [x] Emit basic debug type metadata for primitive integers, bools, and
  pointers.
- [x] Emit parameter metadata.
- [x] Emit local-variable metadata and `llvm.dbg.declare` for stack-backed
  locals.
- [ ] Preserve lexical scope metadata enough for ordinary nested blocks.

Verification:

- [x] LLVM snapshot tests for local/parameter metadata.
- [x] LLDB and CodeLLDB show smoke-program locals and parameters at a
  breakpoint.

### Commit 6: VS Code CodeLLDB Launch Bridge

- [x] Add VS Code `debuggers` contribution metadata for `type: "nerd"`.
- [x] Add a CodeLLDB launch configuration path that builds the active Nerd file.
- [x] Delegate Linux debug sessions to CodeLLDB after the build succeeds.
- [x] Surface build failures through VS Code output or diagnostics.

Verification:

- [x] `npm run compile` in `syntax/nerd-vscode`.
- [x] `python3 build/check_editor_integrations.py --nerd _bin/nerd-debug`.
- [x] Manual VS Code launch starts a Nerd debug session and stops at a
  breakpoint.

### Commit 6A: Debug Build Contract Regression

- [x] Gate LLVM debug metadata emission to normal non-release builds.
- [x] Add a Linux command test proving normal builds embed Nerd debug line
  information in the executable.
- [x] Add a Linux command test proving release builds omit Nerd debug line
  information.

Verification:

- [x] `python3 build/test.py --filter build-debug-info-linux`
- [x] `python3 build/test.py --filter build-release-no-debug-info-linux`

### Commit 7: Breakpoint Quality

- [x] Validate debug line-table source paths in imported module root files.
- [x] Validate breakpoints in imported modules and folder-module part files
  with CodeLLDB's bundled LLDB.
- [x] Improve source path handling for generated folder-module source mapping.
- [x] Add first source step-over smoke coverage.
- [x] Manual CodeLLDB step-over proof for `examples/dungeon/dungeon.n` main
  startup stays in Nerd source.

Verification:

- [x] Linux command test for root file, imported module, and folder-module part
  debug line entries.
- [x] Debugger smoke cases for root file, imported module, and folder-module
  part file.
- [x] Debugger stepping smoke for simple source step-over.
- [x] Manual CodeLLDB step-over in `examples/dungeon/dungeon.n` visited
  `dungeon.n:587`, `588`, `589`, `590`, and `593` without stopping in runtime
  glue.
- [ ] Manual CodeLLDB checks for moved or rejected breakpoint behaviour.

### Commit 8: Watch And Value First Pass

- [x] Define the first supported watch-expression subset.
- [x] Support ordinary local and parameter watches through CodeLLDB.
- [x] Add first-pass rendering notes for strings, slices, and dynamic arrays.
- [ ] Decide whether a Nerd adapter or pretty-printer layer is needed before
  going beyond native CodeLLDB evaluation.
- [ ] Plan tagged enum rendering so watches and Variables can show the active
  tag as a symbol plus integer and switch payload children based on that tag.

Verification:

- [x] Manual CodeLLDB watch checks for locals and simple fields.
- [x] Manual CodeLLDB watch checks for typed pointer indexes.
- [x] Manual CodeLLDB watch checks for collection indexes.
- [x] Any automated debugger smoke checks added by this point pass.

### Commit 9: Windows 11 Plan To Implementation

- [ ] Validate clang debug output format on Windows 11.
- [ ] Choose the Windows native debugger adapter and debug format.
- [ ] Update VS Code launch routing for Windows 11.
- [ ] Validate breakpoints, stepping, locals, and basic watches on Windows 11.

Verification:

- [ ] Windows 11 manual VS Code debugger proof.
- [ ] Windows-capable editor integration checks where practical.

## Documentation Updates Needed

- `README.md`: mention the debugger roadmap once the first milestone starts.
- `docs/compiler-pipeline.md`: describe debug metadata emission and debug-build
  artefacts.
- `docs/editor-support.md`: add VS Code debugger ownership and verification.
- `syntax/nerd-vscode/README.md`: document launch configuration and debugger
  path lookup.
- `tests/README.md` and `docs/testing.md`: document debugger smoke tests once
  they exist.
