# Source-owned build requirements — accepted, 2026-09-28

Implemented syntax and semantics are documented in
[configuration](../../docs/configuration.md#source-build-settings). This replaces
the initial draft's root-only, array and `env_path` proposals following review.

Stable requirements belong in source; invocation choices belong on the command
line; machine-specific locations and tool selection belong in the environment.
Imported modules contribute repeated `library_path` entries, importers precede
dependencies, and scalar `windowed` settings override inherited values. Source
`define` is additive and module-local, evaluated top to bottom for build guards.
The source feature deliberately does not add raw linker flags, executable build
hooks, module search paths, or runtime environment mutation.

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
| `-Dname`, legacy `-k:name` | CLI | Per-invocation global feature selection; source `define` adds module-local development features |
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
| Global `--config` | Compatibility / local invocation profile | Currently loads env and defines. Should not be necessary for fixed project dependency rules |

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
| `build { windowed: yes }` | Source | Existing fixed program behavior; currently discovered in active root/imported modules, including std.frame. Do not change that transitively imported behavior as a side effect of root configuration |
| Other compiler pragmas / intrinsic declarations | Source | Language/declaration semantics; not invocation preferences |
| Vulkan/custom dependency search directories | Source rule + environment location | `library_path: $VULKAN_SDK/Lib`; no hard-coded Scoop path or Vulkan-specific compiler special case |
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
| nerd.json `define` | Invocation profile during migration; eventual deprecation decision | Additive global invocation defines; source defines are module-local |

Evidence:

- [`docs/configuration.md`](../../docs/configuration.md) and
  `nerd_apply_json_config`: the JSON fields env/define are supported; the default
  file is read from CWD, not automatically from the root source directory.
- [`modules.c`](../../src/compiler/modules/modules.c), `module_library_path` and
  `module_resolve_path`: sibling imports, replacement library roots and CWD
  lookup. `module_paths` must not accidentally revive fallback to an unrelated
  installed compiler's modules.
- [`back.c`](../../src/compiler/build/back/back.c): native external libraries,
  tool invocation, Linux CRT/loader overrides and Windows linker construction.
- [`program.c`](../../src/compiler/build/front/program.c),
  `program_collect_build_entries` and `program_compose_build_settings`: active `build { windowed: yes }` processing.
- [`build/build.py`](../../build/build.py): compiler bootstrap uses CC (default
  Clang), with profile-specific flags chosen by the build script.

One diagnostic mismatch also surfaced: Windows `nerd doctor` currently requires
nonempty LIB before its probe, even though this machine's linker successfully
locates SDK/CRT defaults without it. Source library paths should not reproduce
that assumption. Treat doctor readiness as a successful toolchain probe rather
than the presence of one particular environment variable.
