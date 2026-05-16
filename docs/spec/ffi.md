# FFI

FFI syntax is parsed in `ast_parse_declaration` and `ast_parse_ffi_signature` in
`src/compiler/ast/parse.c`. Signature validation and library resolution are in
`src/compiler/sema/sema.c`. This spec complements the user-facing
[`../ffi.md`](../ffi.md) document.

## Syntax

```bnf
ffi-declaration ::= 'ffi' expression ffi-entry
                  | 'ffi' expression '{' { [ 'pub' ] ffi-entry } '}'

ffi-entry       ::= IDENT [ '::' IDENT ] '(' ffi-param-list? ')' [ '->' type ]
ffi-param       ::= [ IDENT ':' ] type
ffi-param-list  ::= ffi-param { ',' ffi-param } [ ',' '...' ]
                  | '...'
```

The expression after `ffi` must fold to a compile-time `string` naming the
library. Omitting `-> type` means the foreign function returns `void`.

In an FFI block, `local :: foreign (...)` gives the Nerd-visible name separately
from the foreign symbol. A block entry may be `pub` independently.

## Parameters

FFI parameters may be named or unnamed. Varargs are written with `...` at the
end of the parameter list.

The parser has a slot for default arguments in FFI signatures, but semantic
analysis rejects defaults on FFI parameters.

## Type Surface

Supported FFI-safe types are the scalar primitives, pointers, C-layout plexes,
packed plexes, raw unions, and function signatures that use FFI-safe parameter
and return types. Runtime `string`, ordinary non-C plexes, slices, dynamic
arrays, tuples, and enums are not documented here as FFI-safe values.

## Code/Manual Notes

No current contradiction was found between `docs/ffi.md` and the implementation
while adding this spec. Both agree that FFI defaults are rejected semantically.
