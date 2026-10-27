# Patterns

Pattern syntax is parsed in `src/compiler/ast/parse.c`; pattern typing,
coverage, guards, and binders are checked in `src/compiler/sema/sema.c`.

## Pattern Forms

```bnf
pattern ::= IDENT
          | 'for' expression
          | ('==' | '!=' | '<' | '<=' | '>' | '>=') expression
          | 'in' '[' expression '..' expression ']'
          | 'in' '[' expression '..=' expression ']'
          | '_'
          | '(' pattern-list? ')'
          | '{' plex-pattern-field-list? '}'
          | type '{' plex-pattern-field-list? '}'
          | module-path '(' pattern-list? ')'
          | expression '..' expression
          | expression '..=' expression
          | expression
```

`_` ignores a value. A bare name binds the matched value. `for expression`
matches against a runtime value or expression instead of binding a new name.
Branch-level `as name` binds the scrutinee for that branch.

## Structural Patterns

Tuple patterns match tuple arity. Plex patterns match named fields. Typed plex
patterns qualify the record type before the field pattern body:

```nerd
on value {
    { x: 0, y, ... } => y
    else => 0
}
```

Field shorthand parses as a nested pattern for the same field name. `field:
pattern` uses an explicit nested pattern.

A typed plex pattern is valid directly as a branch head as well as inside
another pattern. The subject's type disambiguates a typed record head from the
same braced syntax used for an enum payload. The record qualifier must resolve
to the subject's record type.

Every field must appear exactly once unless the pattern ends with `...`.
The trailing marker ignores remaining fields; it can be followed by a comma,
but no further fields or markers. Unknown and duplicate fields are errors even
when `...` is present. These rules apply recursively to nested plex patterns
and plex enum payloads. Field order is unrestricted.

- `field: 42` matches a value; nested patterns work in the same position.
- `field: for wanted` compares with an existing variable or expression.
- `field: bound` binds the field to `bound`; `field` is shorthand for
  `field: field`.
- `field: _` explicitly ignores one field.
- `{ ... }` explicitly ignores all fields; `{}` is complete only for an empty plex.
- `{ field: for wanted, ... } as matched` also binds the whole subject.

Ignoring a field does not compare it with its default value. Plex literal `...`
initialises omitted fields; pattern `...` leaves them unconstrained.


## Enum Patterns

Enum variant patterns use call-like syntax:

```nerd
on maybe {
    Some(value) => value
    None() => 0
}
```

The parser accepts qualified variants such as `module.Type.Variant(...)` when
the dotted path resolves semantically.

Unit enum variants can be written bare when the scrutinee type provides the
enum context, such as `None => 0`.

Built-in optional and result types do not have enum variant patterns. Match
their payload domains with the `on value { ... } else { ... }` forms described
in `control-flow.md`, or use boolean extraction with payload binders.

Postfix `!` on a pattern selects the error payload `U` of a result subject
`T\U`. It requires a result subject; ordinary values and optionals have no
error channel. When matching an already extracted error value of type `U`,
write the pattern without `!`. A separate error pattern table likewise
requires a result subject.

## Ranges And Comparisons

Range patterns support exclusive `..` and inclusive `..=` bounds. Empty integer
range patterns are reported as semantic errors. Comparison patterns apply the
comparison operator to the scrutinee and the expression value.

The explicit bracketed forms `in [start..end]` and `in [start..=end]` have the
same exclusive and inclusive bound rules. They make the membership operation
visible at the start of a branch while retaining support for the existing bare
range patterns.

## Restrictions

Block-form `on` wildcard matching must use `else`; `_ =>` is diagnosed as an
invalid wildcard branch. Interpolated strings inside patterns are validated so
runtime temporaries cannot escape.
