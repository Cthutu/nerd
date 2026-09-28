# Source-owned build requirements — draft, 2026-09-28

Status: discussion proposal, not accepted language syntax or implemented behavior.
Audited revision: `2bc6c8ae` on `vulkan`. The user's local Vulkan example edits
are outside this audit. No compiler, project environment or existing command
behavior is changed by this document.

## Principle

Put stable project requirements in source; leave invocation choices on the
command line and machine-specific locations/tool selection in the environment.
The fixed fact is "this project finds its Windows Vulkan library under
VULKAN_SDK/Lib", not a particular developer's absolute SDK installation path.
Source records that relationship, and the build environment supplies the value.
Defines remain invocation inputs, as requested by the user.

## Candidate syntax

Preferred starting point: a declarative, contextual top-level `build` block in
the root file's preamble, before imports and declarations:

```nerd
build {
    on "windows" {
        library_paths: [env_path("VULKAN_SDK", "Lib")]
    }
}

use std.frame
use std.vulkan
```

`build`, `library_paths` and `env_path` above are proposals, not existing APIs.
The braces, field-colon form, arrays and `on` guards follow familiar Nerd syntax.
Recognize `build {` contextually, so a function or variable named `build` remains
legal. Do not introduce general compile-time execution just to configure linking.
`env_path` is a restricted configuration operation: read a named environment
root and join literal path components using host path rules. It does not execute
a shell or change the process environment. Ordinary strings remain literal;
there is no implicit `$NAME`, `%NAME%`, tilde or shell-expression expansion.

A future module-search field could use the same shape:

```nerd
build {
    module_paths: ["vendor/modules"]
    on "windows" {
        library_paths: [env_path("VULKAN_SDK", "Lib")]
    }
}
```

Start implementation with `library_paths`; module roots require separate
resolution/precedence tests. Do not include defines, optimization or jobs fields
in the initial block. Library *names* remain with FFI declarations, using the
already-supported platform-selected `VULKAN_LIBRARY` constant in std.vulkan.

Alternatives worth deciding before implementation:

| Form | Benefit | Cost |
| --- | --- | --- |
| `build { ... }` | Clear grouping for root build requirements; contextual keyword can preserve identifiers | New top-level grammar and tooling support |
| `pragma build { ... }` | Reuses the existing directive introducer | Existing pragmas do not support blocks; still requires a grammar extension |
| `pragma library_path(...)` | Small incremental directive surface | Existing pragma arguments are literals only; environment/path expressions still need design, and many directives scatter configuration |

## Required semantics

- One optional block per root, before imports; reject duplicates and unknown
  fields. Configuration is not an ordinary symbol, exported module or runtime
  object. Source without a block keeps existing behavior.
- Only the selected root's block contributes settings. Imported modules do not
  change their consumer's search paths. Recommended rule: parse but leave an
  imported file's root configuration inactive, so a root may also be reused as a
  module; make this explicit in tooling rather than silently merging blocks.
- Configuration guards read platform/profile/CLI define inputs already known
  before imports. An inactive platform branch does not read its environment.
  The block cannot define a keyword that controls its own evaluation.
- Relative literal paths resolve against the root file's directory, never the
  invocation working directory. An SDK environment root must be absolute;
  missing/empty values get a diagnostic at the reference when that path is
  required. Inline snippets have no source directory: reject relative source
  paths there rather than quietly using CWD.
- Multiple active `library_paths` contributions append in source order; normalize
  and deduplicate without sorting. Do not globally lowercase paths on Unix.
  Explicit source library paths precede ambient linker defaults. Preserve the
  toolchain's default SDK/CRT search behavior; do not replace LIB or PATH.
- Build/link resolves required library paths and emits quoted `/libpath:` on
  Windows and `-L` on ELF targets. `--copts` must emit equivalent external-Clang
  search options. `--cgen` alone and object-only output need not require installed
  native libraries. Static archives likewise need no final external link.
- `check`, `format` and LSP syntax/semantic analysis must understand the block
  without requiring a link SDK. Validate syntax/unknown keys, but resolve linker
  dependencies only when needed; editor build-readiness diagnostics can be
  separate. Future module paths are different: analysis needs them for imports.
- Evaluate per root/build request, not by changing global compiler environment.
  LSP sessions can serve multiple roots with different settings and must not
  leak configuration between them. Imported documents need an associated root;
  ambiguous roots must not be selected from arbitrary filesystem traversal.
- Track the root configuration, active guards and referenced environment values
  as build inputs. Any cache must invalidate when they change. Verbose build
  output should show resolved paths and source provenance. Do not log unrelated
  environment variables.
- Runtime environment (for example Vulkan validation-layer discovery) is a
  separate concern. `library_paths` does not configure the Vulkan loader or
  silently alter the environment of programs launched with `nerd run`.
- Arbitrary compiler/linker argument strings and executable build hooks are
  outside the initial feature. Prefer typed settings with portable semantics.

## Current CLI audit

The authoritative surface is constructed in
[`src/nerd.c`](../../src/nerd.c), particularly `nerd_cli_schema`, the config
conversion functions, keyword extraction and program-argument splitting.
The corresponding structs are in
[`src/compiler/compiler.h`](../../src/compiler/compiler.h).

| Current setting | Recommended owner | Rationale / disposition |
| --- | --- | --- |
| Command selection: build/b, run/r, check, test/t, format/f, doctor, init, lsp, internal-test | CLI | Chooses the operation, not a project requirement |
| `--help` / `-h` | CLI | Requests usage information |
| Root file / build snippet; implicit main.n or directory-name source | CLI / source discovery | Selects which root and therefore which configuration to use |
| `-Dname`, legacy `-k:name` | CLI | Per-invocation feature selection; configuration may read these via guards but must not define them |
| `--release` / `-r` on build, run and check | CLI | Debug/release is a build choice, including analysis of conditional source |
| `--jobs` / `-j`, including auto (currently build only) | CLI | Host resources and workload choice; not project semantics |
| `--output` / `-o` on build/run | CLI | Artifact destination varies by developer/CI/package layout |
| `--obj` | CLI | Stop at object generation; an operation on the same source |
| `--lib`, `--dll` | CLI initially; possible source default later | A library root may have stable artifact intent, but static/shared can vary. Do not force this into source before agreeing on overrides |
| `--cgen` | CLI | Alternate generated representation, not a project dependency |
| `--copts` | CLI | Query external C compilation options; must include source-owned library paths |
| `--hir`, `--llvm` on build/run | CLI | Requested inspection artifacts |
| `--keep` / `-k` on run | CLI | Temporary artifact retention policy; unrelated to legacy `-k:name` defines |
| Runtime arguments after `--` | CLI | Inputs to the launched program |
| `--verbose` / `-v`, `--timing` | CLI | Diagnostic detail and measurement choices |
| Test `--filter`, `--list`, verbose listing | CLI | Which tests to execute/report |
| Format `--stdout`, `--output`, verbose progress | CLI | Editor/tool output routing |
| Init destination | CLI | Where to create a project |
| Global `--config` | Compatibility / local invocation profile | Currently loads env and defines; see migration below. Should not be necessary for fixed project dependency rules |

No public `-L`, `--library-path`, target-triple, compiler-path or custom-linker-flag
option exists in the audited CLI. They must not be described as flags being
migrated. Library paths are a new source capability filling the current gap.
Future target/architecture selection would be an invocation/toolchain choice;
source may declare target constraints separately. Cross compilation is not
implemented today.

## Existing source, environment and JSON audit

| Current setting / mechanism | Recommended owner | Notes |
| --- | --- | --- |
| `ffi` library names | Source, with bindings | Already source-owned; retain platform-selected string constants and imported FFI requirements |
| `pragma windowed` | Source | Existing fixed program behavior; currently discovered in active root/imported modules, including std.frame. Do not change that transitively imported behavior as a side effect of root configuration |
| Other compiler pragmas / intrinsic declarations | Source | Language/declaration semantics; not invocation preferences |
| Vulkan/custom dependency search directories | Source rule + environment location | `library_paths: [env_path("VULKAN_SDK", "Lib")]`; no hard-coded Scoop path or Vulkan-specific compiler special case |
| Vendored/project module roots | Source, future `module_paths` | Stable source layout; source-relative paths. Needs a deliberate precedence policy against existing library replacement behavior |
| `NERD_LIB_PATH` | Environment toolchain override | Currently replaces bundled Nerd module roots, not native linker libraries. Explicit empty disables library lookup; preserve this contract |
| `PATH` selecting opt/llc/linkers/archive tools | Machine environment | Compiler/toolchain installation, not project source. Version/feature requirements could be source metadata later |
| Windows `LIB` | Machine/developer environment | SDK/CRT and ambient libraries; source paths should add link arguments without overwriting it |
| `NERD_CRT_DIR`, `NERD_DYNAMIC_LINKER` | Machine/toolchain environment | Linux host startup objects/loader overrides, not ordinary project dependencies |
| `VULKAN_SDK`, `VK_SDK_PATH` | Machine environment | Scoop sets both; Nerd currently reads neither. Source explicitly selects the SDK root variable it needs |
| `VK_LAYER_PATH`, `VK_ADD_LAYER_PATH` | Runtime/development environment | Vulkan loader settings, not native linking inputs. A future launch block would be a separate proposal |
| `NERD_PROFILE`, `NERD_PROFILE_LOCKS`, `NERD_MEMORY_PROFILE` | Invocation environment | Compiler profiling and instrumentation; do not freeze in source |
| `NERD_DEBUG_LLVM_SIDECARS`, `NERD_DEBUG_KEEP_LINK_LLVM` | Invocation environment | Debugging intermediate output retention |
| `NERD_TEST_RENDER_SOURCE`, `NERD_ERROR_RENDER_TEST` | Internal test environment | Harness controls, not project configuration |
| `TMPDIR`, terminal `COLUMNS` | Process environment | Temporary storage and presentation |
| `CC`, SDK INCLUDE variables, build.py flags | Compiler bootstrap environment | Build Nerd itself; separate from building a Nerd program. Keep Clang/toolchain selection out of application source |
| nerd.json `env` | Compatibility/local profile | Currently mutates process environment, with paths relative to CWD and no environment interpolation. Replace checked-in project path recipes with typed source fields as supported |
| nerd.json `define` | Invocation profile during migration; eventual deprecation decision | Currently additive with CLI defines. Conflicts with a strict CLI-only defines policy; do not silently remove it |

Evidence:

- [`docs/configuration.md`](../../docs/configuration.md) and
  `nerd_apply_json_config`: only env/define are currently supported; the default
  file is read from CWD, not automatically from the root source directory.
- [`modules.c`](../../src/compiler/modules/modules.c), `module_library_path` and
  `module_resolve_path`: sibling imports, replacement library roots and CWD
  lookup. `module_paths` must not accidentally revive fallback to an unrelated
  installed compiler's modules.
- [`back.c`](../../src/compiler/build/back/back.c): native external libraries,
  tool invocation, Linux CRT/loader overrides and Windows linker construction.
- [`program.c`](../../src/compiler/build/front/program.c),
  `program_apply_pragmas`: active `pragma windowed` processing.
- [`build/build.py`](../../build/build.py): compiler bootstrap uses CC (default
  Clang), with profile-specific flags chosen by the build script.

One diagnostic mismatch also surfaced: Windows `nerd doctor` currently requires
nonempty LIB before its probe, even though this machine's linker successfully
locates SDK/CRT defaults without it. Source library paths should not reproduce
that assumption. Treat doctor readiness as a successful toolchain probe rather
than the presence of one particular environment variable.

## Migration and precedence

1. Add source-owned library search rules without removing existing flags or
   JSON support. Existing code remains unchanged; unresolved active environment
   references produce source-located link-configuration diagnostics.
2. Keep defines on the command line in new documentation/examples. Explain
   existing nerd.json defines as legacy/profile behavior; removing them or
   requiring explicit profile selection needs a separate compatibility decision.
3. Do not write a generic "CLI wins over everything" rule: defines/jobs/profile
   have no source counterpart initially, library paths are ordered additions,
   and the environment only supplies referenced locations. Current JSON env
   application happens first, so it supplies effective env_path values; identify
   that provenance in diagnostics until JSON env migration is decided.
4. Do not add module_paths until its order is specified. Recommended initial
   order: existing sibling-relative imports, explicit root project module paths,
   the selected NERD_LIB_PATH-or-bundled roots, then the existing CWD fallback.
   Project paths supplement rather than replace the selected standard library;
   define shadowing/core rules before implementation.
5. If source artifact-kind defaults are later added, explicit CLI output-kind
   flags override the default; `run` rejects non-executable effective output.
   Retain `-o` for destination selection. This is not part of the first change.

## Suggested implementation slice and gates

First agree on the block spelling, root-only behavior and env_path semantics.
Then implement only library_paths with platform/CLI-define guards, path joining,
source-relative resolution, native LLVM linking and `--copts` parity. Keep
`just run-example vktriangle` identical on Windows and Linux. No special Vulkan
lookup in the backend and no Windows task wrapper.

Tests should cover spaces/non-ASCII paths, different invocation CWDs, unset and
empty active environment variables, inactive platform branches, duplicate paths
and ordering, malformed/unknown fields, root-versus-import behavior, multiple
LSP roots, format idempotence and existing CLI/JSON compatibility. Verify that
checking/formatting/C-only emission work without the Vulkan SDK, while actual
linking diagnoses missing required library paths. Exercise successful direct
LLVM and external generated-C linking against the same native test library.
Future cache support must include configuration/env dependencies explicitly.

No tests were run for this audit-only document; no language feature is implemented.
