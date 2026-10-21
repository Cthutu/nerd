# Debugger status and follow-up

Nerd's VS Code extension builds the active source and starts a `type: "nerd"`
session through CodeLLDB. The [extension guide](syntax/nerd-vscode/README.md)
contains launch configuration and compiler lookup instructions; the
[editor support guide](docs/editor-support.md) describes integration ownership.

## Implemented behaviour

Normal non-release LLVM builds emit source debug metadata and use `llc -O0`.
Release builds omit Nerd debug metadata, optimise IR with `opt`, and use
`llc -O2`. The installed compiler invokes LLVM tools directly, not Clang.
Windows native links select DWARF debug information with `/debug:dwarf`.

Metadata describes compile units, module/part source paths, function names,
statement locations, implicit-exit anchors, lexical scopes, parameters, locals
and globals. Composite immutable locals have stack storage in debug builds so
LLDB can inspect strings, slices, fixed arrays, tuples and plexes. Source-facing
function names are kept distinct from compiler-generated implementation names.

The VS Code shim delegates process control to CodeLLDB and adds Nerd value
presentation: strings, slices, dynamic arrays, tuples/plexes and tagged enum
payloads. It retries dynamic-array `.count`, `.capacity` and `.data` watches
using runtime-header expressions and reports unsupported Nerd-only forms.
Native field access and indexing work where emitted types describe the storage.
This is a supported subset of watch expressions, not a general Nerd evaluator.

Linux smoke tools cover source breakpoints, imported modules and folder parts,
step-over, call stacks, locals and watches using CodeLLDB's bundled LLDB.
Historical observations are in [review/measurements](https://github.com/Cthutu/nerd/blob/fac0feae825d73a478466410956c1ca7749a2a8a/review/measurements/README.md).
A previous host/revision's pass does not establish current native validation.

## Remaining work

- Validate the complete Windows VS Code experience for the requested revision:
  breakpoints, stepping, call stacks, locals, watches and active-session artefact
  lifetime. Use the [Windows workflow](tests/windows/README.md) and record
  user-observed behaviour separately from automated probes.
- Keep the launch schema platform-aware if another native adapter is introduced.
- Extend general pointer dereference/indexing only with explicit runtime-layout
  support and debugger regression coverage.
- Decide whether release-with-debug-info is needed and what its CLI contract is.
- Validate macOS separately before claiming its debugger support.

## Maintenance and verification

Derive debug metadata from semantic/HIR facts. Keep source mapping stable across
CLI builds, installed compilers and editor launches. Keep implementation symbols
out of the normal user-facing view, and handle generated debug artefacts without
deleting files required by an active session.

For metadata or editor changes, use the relevant LLVM/command fixtures and:

```sh
python3 build/check_editor_integrations.py --nerd _bin/nerd-debug
python3 build/check_debugger_adapter_transforms.py
python3 build/check_debugger_smoke.py --nerd _bin/nerd-debug
python3 build/check_debugger_stepping.py --nerd _bin/nerd-debug
```

The last two commands are the Linux CodeLLDB smoke path. Record skipped native
checks explicitly; compilation alone does not verify an interactive debugger.
