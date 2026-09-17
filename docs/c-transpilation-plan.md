# C transpilation implementation plan

The `feature/c-transpilation` branch adds `nerd build --genc`: checked HIR
becomes one C translation unit at the normal output location, with the output
extension replaced by `.c`. No binary or external compiler invocation is needed
for generation. Clang compiles the translation unit, including the embedded Nerd
runtime. External libraries retain their normal link requirements.

## Milestones

1. Add the HIR-to-C emitter, explicit type/declaration ordering, module linkage,
   runtime embedding, entry-point glue, CLI selection, and output path policy.
2. Complete expression, control-flow, aggregate, ownership, string, and runtime
   lowering, using the LLVM backend as the behaviour reference. Preserve source
   evaluation order and cleanup on every control-flow exit.
3. Add differential coverage: generate C, compile with Clang, and compare exit
   status and output with the existing language fixtures. Cover CLI paths,
   imported modules, release builds, and installed-compiler generation.
4. Update pipeline and user documentation, run the full test/format/install
   workflow, and push each tested implementation milestone to this branch.

## Design constraints

- Consume HIR and semantic types directly; do not restore the retired IR.
- Keep LLVM as the default backend.
- Use generated names for module internals and preserve foreign symbol names.
- Emit forward declarations and dependency-ordered aggregate definitions.
- Materialise temporaries to preserve Nerd evaluation order in C.
- Emit runtime source in the file so installed compilers need no source checkout.
- Diagnose unsupported lowering explicitly; never silently emit a default value.
- Target the same host 64-bit Clang contract as the existing backend.
