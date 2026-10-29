# Test suite

Run `just test` for the unified debug/release correctness gate and full-screen
terminal dashboard. Success exits automatically; interactive failures stay for
review. `--plain` streams text, and non-terminal/CI runs never pause. Use
`just test-debug`, `just test-release`, `just test-stdlib`, or `just test --suite
NAME` for focused work. `just test --filter maths` selects matching fixtures.
Network/Nexus and installed editor/debugger integrations remain opt-in.

`tests/run.py` is the entry point. `harness/` owns the shared catalogue, process
supervision, results and dashboard, including subprocess and pseudo-terminal
regressions. [Testing](../docs/testing.md) documents navigation, selection,
reports and cancellation. Native Windows evidence collection reuses the same
catalogue and retains its separate benchmark/manual workflow.

[Feature inventory and teaching-doc gaps](../docs/test-features.md) explains
where coverage belongs. [Harness formats and commands](../docs/testing.md)
describes each regression family.

- `language/*.t`: feature programs with labelled stdout and result assertions.
  Independent programs within a feature use `=== case: scenario-name ===`.
  Keep `main`, imports, canonical trait names and source locations intact when
  they are part of the regression. Source/result/stdout/HIR/LLVM sections use
  `¬`; an optional sixth section supplies stdin.
- `sema/*.check`: named positive semantic checks for programs that intentionally
  cannot link or run, such as declarations of external callback-test libraries.
- `errors/*.e`: feature groups of source/JSON diagnostic pairs. Line, column,
  references, help and notes remain checked.
- `format/*.f`: feature groups of source/formatted-source pairs.
- `lsp/*.lsp`: editor-operation groups of source/requests/expected triples.
  Each triple starts a fresh server. Windows-only cases stay separate.
- `hir/*.hir`, `llvm/*.ll`: focused lowering assertions; expected lines are
  ordered subsequences, with source and snapshot separated by `¬`.
- `commands/*.cmd`: CLI syntax, argument passing, output artifacts, exit and
  shutdown behaviour, allocator diagnostics and diagnostic rendering.
- `cgen/`: additional C-specific source probes. `build/test_cgen.py` also
  discovers language programs and selected process/diagnostic command contracts.
- `install/`, `ffi/`, `mods/`: installation, host-code and imported source fixtures.
- `docs/`: teaching-example expectations; the runner extracts source directly
  from `docs/learn` and checks behaviour in both debug and release gates.
- `stdlib-capabilities/`: executable language/atomic and native-layout probes,
  included in the default debug/release gates.
- `stdlib-system/`: desktop-provider fixtures; `build/test_std_system.py` checks
  Linux launch and dialog contracts with isolated providers, without displaying UI.
- `windows/`: native Windows matrix runner, portable runner checks, desktop
  checklist and current handoff. Generated results remain ignored.

The Python fixture harness is distinct from `nerd test`, which executes unit
tests declared in Nerd sources. Default recipes retain stdlib unit tests,
Raptor and native thread/sync contracts. Generated trial inputs, logs and
measurements are local evidence; do not commit them as replacement fixtures.

`frame/` contains the Linux fixed-size fullscreen lifecycle probe.
`build/test_frame.py` runs it against an isolated Xlib peer in `just test`,
without requiring a display server. Interactive desktop checks remain separate.
