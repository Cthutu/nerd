# Implicit box borrows — Linux validation, 20 October 2026

Tested on `std-library`, based on `a3a43467`. `sources.sha256` records the changed
compiler and regression sources. The worktree was clean before this change.

`box[T] -> ^T` was already accepted. This revision adds `box[T] -> []T` in both
backends, using the box's allocation count without moving ownership. Generic
pointer/slice arguments infer the element type from a box. Nil boxes yield null
pointers/empty slices. Element types must match exactly; boxed arrays do not
implicitly flatten.

The regression also repairs LLVM ownership handling for assignments: consuming
the original box-typed expression after storing a borrowed pointer/slice cleared
the owner. Consumption now follows the converted destination type. Optional
slice wrapping converts the box to a complete slice before packing its payload.

The language regression covers typed initialisation and reassignment, pointer
and slice aliases, mutation, returned borrows from an external owner, record
fields, optional wrapping, local/imported generic functions, generic methods,
nil/zero-length allocations, single elements, padded records, array elements,
ownership transfer after borrowing and explicit cleanup. The diagnostic fixture
rejects mismatched pointee/element types and implicit flattening.

The full debug gate passed **1,208 fixtures, zero failures and nine existing
Linux skips**. Its auxiliary gates passed, including **307 C differential
fixtures at O0/O2 with zero platform skips**. See [just-test.log](just-test.log).

The full release gate also passed **1,208 fixtures, zero failures and nine
existing Linux skips**, plus all applicable auxiliary checks; see
[just-test-release.log](just-test-release.log).

Validation commands:

```sh
just test
just test-release
python3 validation/linux/results/20261020-box-borrows/memory-probe.py
```

The [memory probe](memory-probe.py) compiles the language regression through C
in debug/release modes with AddressSanitizer, UndefinedBehaviorSanitizer and leak
detection. Both runs pass; see [memory.log](memory.log).

Borrowing does not extend allocation lifetime. The compiler does not establish
that a returned/stored pointer or slice outlives its owner; using a borrow after
box cleanup is invalid. Native Windows execution remains unvalidated here.
