# 0003: HIR Entities And Bindings

Status: accepted
Date: 2026-05-09

## Context

Nerd's binding model separates values from names. A function, type definition,
or value can exist without a source-level name, and a Nerd binding gives that
entity a name. This matters for function values and anonymous functions:

```nerd
main :: fn() { ... }
call(fn(x i32) -> i32 { return x + 1 })
```

The first HIR implementation still let top-level HIR functions, type records,
and globals directly carry the Nerd source name. That was convenient for early
snapshots, but it made the HIR look as if a function's identity was its binding.

## Decision

HIR represents functions, type definitions, and stored values as nameless
entities with stable HIR-local ids. Nerd-visible names are represented by
explicit binding records that point at those entities.

For example:

```text
bind main = fn.0
func fn.0() -> i32 {
  return i32 0
}
```

The same model applies to top-level type and value bindings:

```text
bind Point = type.0
type type.0 = Point

bind Counter = value.0
global value.0: i32 = untyped integer 1
```

## Consequences

This better matches Nerd semantics and makes function literals a natural HIR
extension: an anonymous function expression can create or reference a function
entity without inventing a source binding.

LSP features should continue to reason about source bindings and references.
Renaming a Nerd symbol changes binding/reference facts, not the identity of the
underlying HIR function or type entity.

Backends can choose generated internal names for entities and map Nerd-visible
bindings to exported symbols, aliases, debug names, or metadata. The LLVM
backend should still preserve the `$` convention for Nerd-visible bindings.

## Follow-up

1. Add HIR expression support for function literals that reference function
   entities directly.
2. Replace HIR expression symbol references to top-level values/functions with
   binding or entity references.
3. Add module/import/export binding records so whole-program HIR can distinguish
   local entity identity from exported Nerd names.
