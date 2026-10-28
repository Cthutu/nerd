# Teaching-example verification

`python3 build/test_docs.py --nerd _bin/nerd-debug` checks the learning guide's
local links and examples selected in `cases.json`. `just test-docs` builds the
debug compiler and runs these checks. Both normal debug and release gates run
them too.

The selection preserves the existing `nerd-docs/_verify` chapter checks:
command-line arguments, never-returning functions, destructuring, array bytes,
boxes, source tests, macros and compile-time parameters, atomics, advanced FFI,
and build settings. Additional checks exercise hello world, contextual array
inference, plex patterns, borrowed Display interpolation and a greeting with
command-line arguments. This is selected executable coverage, not a claim that all
Markdown fences are independent programs.

Cases identify a chapter, section heading and one-based Nerd fence number
within that section. `run` checks exact stdout, stderr and exit status; `test`
executes source tests; `check` checks declarations that need no runnable entry;
`artifacts` builds nonempty object, static and shared libraries. ANSI colour is
removed from test output. `{source}` and `{executable}` in expected stdout
stand for the generated absolute paths. Runtime cases may provide `args`.

The harness supplies an empty `main` for declaration-only snippets, a sibling
`greeting.n` module and the documented `message.txt` asset. Other code comes
directly from the guide. Temporary directories are removed on success and
retained on failure. A failure must be investigated against the intended
teaching contract before changing an example or its expectation.

The old local issue probes are already represented by feature regressions in
`tests/language` and `tests/errors` (default initialisation, tuple mutation,
generics, Display, scientific literals and truthiness). Generated executables,
historical JSON results and machine-specific probe launchers are not imported.
