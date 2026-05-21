# Patterns

Pattern syntax is parsed in `src/compiler/ast/parse.c`; pattern typing,
coverage, guards, and binders are checked in `src/compiler/sema/sema.c`.

## Pattern Forms

```bnf
pattern ::= IDENT
          | 'for' expression
          | ('==' | '!=' | '<' | '<=' | '>' | '>=') expression
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
    Point { x: 0, y } => y
    _ => 0
}
```

Field shorthand parses as a nested pattern for the same field name. `field:
pattern` uses an explicit nested pattern.

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

## Ranges And Comparisons

Range patterns support exclusive `..` and inclusive `..=` bounds. Empty integer
range patterns are reported as semantic errors. Comparison patterns apply the
comparison operator to the scrutinee and the expression value.

## Restrictions

Block-form `on` wildcard matching must use `else`; `_ =>` is diagnosed as an
invalid wildcard branch. Interpolated strings inside patterns are validated so
runtime temporaries cannot escape.
