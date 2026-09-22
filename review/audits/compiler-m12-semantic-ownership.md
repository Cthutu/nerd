# M12: semantic ownership design

Status: design and mutation inventory, 2026-09-22. Implementation and adoption
are not complete. This is a separate experiment from M9–M11 and does not change
the default worker count or remove closure exclusion.

## What prevents shared-core concurrency

`program_check_discovered` in `src/compiler/build/front/program.c` claims the
entire transitive import closure, including implicit core, for each check task.
The task's `ProgramInfo` is a shallow view, not a copy of semantic storage.
Dropping core from the claim would introduce concurrent writes into shared
arrays and readers holding pointers across reallocations.

The relevant mutation paths in `src/compiler/sema/sema.c` are:

| Path | Shared writes / consequences |
| --- | --- |
| `sema_import_type` | Recursively interns types, parameters and symbols in the destination module; destination is sometimes an imported module |
| `sema_instantiate_imported_generic_function` | Imports argument types into the defining module, resolves constraints there, then instantiates the body there |
| `sema_emit_generic_function_instantiation` | Appends type parameter metadata, symbols, locals/scopes, specialization records and per-node tables; snapshots restore selected node tables, not all appended state |
| Trait-constraint and method selection | Imports receiver types into the defining module, resolves types/constraints and may instantiate a generic method or infer its signature |
| Constant evaluation through const signatures | `sema_try_eval_integer_constant` casts away constness to resolve enum/type expressions; returning a const imported view alone does not make this call tree immutable |
| HIR and LLVM consumers | Read specialization symbols, root scopes, function types and saved node tables from the owning semantic module |

`SemaGenericFnInstantiation` in `sema.h` contains ten copied node tables plus
indices into other owner arrays. A mutex around the specialization cache cannot
protect unrelated readers against those arrays moving. Copying `Sema` is also
insufficient: its arrays remain shared. Deep-copying it would require remapping
all appended type, symbol, local, scope and node-table references at publication.
The previous imported-generic bug demonstrates why a module-local type index
cannot be a cross-module identity.

## Proposed ownership boundary

1. Freeze dependency declarations and signatures after dependency checking.
   Expose read-only access through a dedicated imported-module interface. A
   const pointer to today's `Sema` does not establish this invariant.
2. Give each checking task its own type/symbol extension arena, inferred-node
   tables and specialization requests. Use explicit references distinguishing
   frozen module records from task-owned extensions; never overload a raw u32
   index to mean either without an owner tag.
3. Key specialization requests by defining module identity, template declaration
   identity and canonical argument types/compile-time values. Nominal types
   include defining module and declaration identity. Structural types include
   full shape, qualifiers, nested types and array lengths. Local numeric type
   IDs and symbol handles are excluded. Recursive types need cycle-safe keys.
4. A coordinator deduplicates and publishes requests in stable module/declaration
   and request-source order. Task completion order must not assign identities.
   Hash lookup requires full-key equality, not hash-only identity.
5. Checking a call often needs the specialized signature immediately. Introduce
   an explicit pending result and retry/continuation boundary; do not let every
   worker block waiting for a queued specialization while holding all worker
   slots. Dependencies among requests need cycle diagnostics, not a deadlock.
6. Check specialization bodies in isolated storage, then publish immutable
   records and resolve references before HIR starts. Preserve deterministic
   first-error behavior and the current serial fallback until independently
   proven equivalent. Failed tasks publish no partial records.

## Implementation sequence and gates

| Slice | Deliverable | Required evidence |
| --- | --- | --- |
| M12a | Imported read interface and mutation assertions, initially serial | Full suite; explicit/inferred/function-valued generic and trait fixtures; no hidden writes through casts |
| M12b | Canonical reference/key representation, still serial | Same keys across differing local type insertion order; distinct nominal types remain distinct; recursive types, values and hash collisions |
| M12c | Owned specialization requests and coordinator publication | Byte-identical LLVM/C/HIR, debug symbols and errors; shared diamond dedup; nested/reentrant requests; cancellation/failure cleanup |
| M12d | Concurrent checking against frozen dependencies | Randomized completion, ASan/TSan, native Windows; retained numeric serial reference; peak-memory bounds |
| M12e | Adoption measurement | Source-to-IR latency and CPU improvement on real inputs, whole-build benefit, no small-build regression |

M12a must precede changing task eligibility. The existing closure scheduler stays
in production while these interfaces are introduced. M12 cannot be called
complete on the strength of a design document or a thread-count change. Native
validation and implementation remain outstanding; there is no claimed speedup.
