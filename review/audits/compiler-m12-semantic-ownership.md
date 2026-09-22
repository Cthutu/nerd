# M12: semantic ownership design

Status: Linux design/feasibility investigation complete, 2026-09-22. The
module-level ownership implementation is not adopted after the headroom analysis
below. This is a separate experiment from M9–M11 and does not change
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

## Feasibility result: module-level redesign does not meet the target

Before changing ownership, `build/semantic_headroom.py` models ideal module
checking from saved jobs=1 profiles. It uses unlimited workers, fixed measured
per-module semantic weights and only the recorded import dependencies. It
ignores shared-core exclusion, scheduling/transfer costs and unrecorded implicit
edges, making it deliberately optimistic. This is a model from single profiled
samples, not measured parallel execution or a bound on every possible redesign.
Known independent/chain/diamond graphs and invalid graphs test the analysis.

| Workload, debug target | Semantic sum | Dependency critical path | Optimistic whole-profile gain |
| --- | ---: | ---: | ---: |
| Pixels, policy-B sweep serial sample | 62.50 ms | 60.07 ms | 1.09% |
| Quill, policy-B sweep serial sample | 6.87 ms | 4.51 ms | 2.98% |
| Dungeon, full sweep serial sample | 62.51 ms | 60.89 ms | 0.09% |
| Wide synthetic, 48 functions/module | 5.32 ms | 0.57 ms | 6.38% |
| Deep synthetic, 48 functions/module | 5.57 ms | 5.32 ms | 0.33% |
| Large single module | 47.61 ms | 47.39 ms | 0.19% |

The same model reports source-to-IR headroom separately: Pixels debug 2.0%,
Quill 10.4%, wide synthetic 16.7%, deep 0.9%, and one large module 0.3%. This
supports focusing on the heavy dependency chain or finer task granularity rather
than treating all source-to-IR workloads as equally parallelizable.

Pixels' expensive frame → OpenGL → gfx checks lie on one dependency chain.
Removing the implicit-core conflict cannot parallelize that chain. The full
sweep's independent profile gives the same conclusion (Pixels 0.88%, Quill
3.45%, Dungeon 0.09%). Release whole-build headroom is smaller for the real
inputs. Raw model outputs are `semantic-headroom*.json` in
`review/measurements/compiler-m10-adaptive/`.

Decision: do not undertake the above ownership implementation solely to meet the
15% whole-build target with the existing module task granularity. The M12
feasibility/design investigation is complete on the measured Linux inputs; its
implementation is deliberately not adopted. This does not assert that semantic
parallelism is impossible. Splitting declaration/signature preparation from
function-body checking changes the dependency graph and requires a separate
measurement/design experiment. Single-core improvements in the heavy modules
can also save time without introducing that ownership model. The implementation
sequence above is retained as a design, not reported as completed code.

Reproduce the model with:

```sh
python3 build/test_semantic_headroom.py
python3 build/semantic_headroom.py review/measurements/compiler-m10-adaptive/m10-policy-b.json.gz --output /tmp/semantic-headroom.json
```
