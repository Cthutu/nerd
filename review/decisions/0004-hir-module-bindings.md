# 0004: HIR Module Bindings

Status: accepted
Date: 2026-05-10

## Context

HIR now separates nameless entities from Nerd bindings. Module boundaries need
the same treatment: an imported function, type, value, or module alias is not a
local HIR entity, but it still participates in source-level binding and export
semantics.

This matters for LLVM lowering because module-visible names can become aliases,
exports, or metadata while the underlying implementation keeps generated
entity names.

## Decision

HIR records module dependencies, imported bindings, module aliases, and exports
explicitly:

```text
module module.0(root)
import module.1(test.folder_pub_use)
import import.0 child_answer from module.1(test.folder_pub_use).decl.1: fn () -> i32
bind child_answer = import.0
bind folder = module.1
export bind.0(child_answer)
```

Local Nerd bindings still point at local HIR entities such as `fn.N`,
`type.N`, and `value.N`. Imported Nerd bindings point at `import.N`, and module
aliases point at `module.N`.

## Consequences

HIR references can resolve to explicit bindings for imported declarations
instead of falling back to raw semantic declaration ids. That keeps the HIR
closer to Nerd's binding model and gives the LLVM backend a clear place to map
module exports/imports to target symbols.

The textual representation remains derived from the HIR data structures. It is
shown only when a module has imports, imported bindings, or exports, so simple
single-file snapshots stay focused on local entity lowering.

## Follow-up

1. Use these records in HIR-to-LLVM lowering instead of C-generation-specific
   dependency ordering.
2. Decide whether whole-program HIR dumps should render all loaded modules or
   continue saving only the root module snapshot.
