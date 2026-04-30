# Standard Library Notes

This document tracks the current standard library surface separately from the
language manual.

The standard library is still in development, so this file should be treated as
a working reference rather than a stable contract. The language manual may use
standard library functions in small examples, but it should not duplicate this
API reference.

## Current Modules

The current standard modules live under [mods/std](/home/matt/nerd/mods/std).

- `std.io`
  Basic input and output helpers.
- `std.mem`
  Low-level allocation wrappers.
- `std.arena`
  Arena allocation helpers.
- `std.string`
  String utilities.

The repository also contains early `std.random` source work. Treat it as
experimental until its dependencies and syntax surface are covered by the
roadmap's horizontal feature policy.

## Documentation Policy

- Keep core language rules in [manual.md](/home/matt/nerd/docs/manual.md).
- Keep implementation notes in the existing compiler documentation.
- Keep public standard library signatures, ownership notes, and examples here.
- Mark unstable or experimental APIs clearly when this document becomes a fuller
  reference.

## Known Ownership-Sensitive APIs

Dynamic arrays and memory helpers should document ownership explicitly.

Examples:

- whether the caller must call `.free()`
- whether a returned slice borrows existing storage
- whether a function may reallocate a dynamic array
- whether a pointer-to-slice cast such as `p.as([]T, count)` creates a borrowed
  view rather than owned storage

## Initial API Inventory

This inventory is intentionally brief until the standard library settles.

### `std.io`

- `pr(text: string) -> void`
- `prn(text: string) -> void`
- `input(prompt: string) -> string`

### `std.mem`

Low-level allocation helpers backed by C allocation functions. These APIs should
be documented with exact ownership and lifetime rules before being presented as
stable user-facing library functions.

### `std.arena`

Arena helpers built on top of `std.mem`.

### `std.string`

- `split(s: string, sep: string) -> [..]string`

`split` returns a dynamic array and the caller is responsible for freeing that
array when it is no longer needed.

### Experimental `std.random`

The tracked `mods/std/random.n` file is not part of the stable standard-library
inventory yet. Before documenting it as supported, make sure the module builds
through the normal module pipeline and that its required language features,
supporting modules, tests, and manual notes have landed together.
