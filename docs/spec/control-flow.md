# Control Flow

Control-flow syntax is parsed in `src/compiler/ast/parse.c`; value typing,
exhaustiveness, labels, and loop-control validation are checked in
`src/compiler/sema/sema.c`.

Nerd has two control-flow introducers for these families:

- All loop forms use the `for` keyword.
- All conditional and pattern-branching forms use the `on` keyword.

There are no separate `if`, `while`, `switch`, or `match` keywords in the
current lexer or parser.

## Loops

```bnf
for-statement ::= 'for' for-form [ '$' IDENT ] block [ 'else' block ]
for-form      ::= /* empty */
                | expression
                | IDENT 'in' expression
                | '^' IDENT 'in' expression
                | IDENT ',' IDENT 'in' expression
                | IDENT ',' '^' IDENT 'in' expression
                | [ for-item-list ] ';' [ expression ] ';' [ for-item-list ]
```

Every loop starts with `for`. Supported loop forms are infinite loops, condition
loops, `for-in` loops, and C-style loops. `for-in` binders are identifiers, or
`^` followed by an identifier when the iterated item type is a pointer and the
loop should bind the pointed-to value.

Built-in collection iteration over arrays, slices, strings, and dynamic arrays
binds pointer items. Range iteration binds integer values. User-defined
iterator iteration is selected when the iterable type has a concrete
`core.Iterator[Item]` implementation whose `next` method has type
`fn (^Iter) -> Option[Item]`; `None` ends the loop and `Some(value)` binds the
item.

`break` exits the nearest loop or the named labelled loop. `again` resumes the
nearest loop or the named labelled loop. `break` may carry a value when the loop
is used as an expression.

## Loop Values And Else

A loop can produce a value through `break value`. A finite value-producing loop
must make exhaustion explicit with an `else` block that produces a compatible
value. Infinite loops do not need an exhaustion value.

```nerd
result := for item in items {
    on item == target => break item
} else {
    break fallback
}
```

## `on`

Every branch form starts with `on`. Nerd has three `on` shapes:

```bnf
bool-on      ::= 'on' expression '=>' branch-expression [ 'else' branch-expression ]
condition-on ::= 'on' '{' { expression '=>' branch-expression | 'else' '=>' branch-expression } '}'
value-on     ::= 'on' expression '{' { pattern-list [ 'as' IDENT ] [ 'on' expression ] '=>' branch-expression | 'else' [ 'as' IDENT ] '=>' branch-expression } '}'
```

Condition branches and guards must type as `bool`. Value-producing block-form
`on` expressions must be exhaustive. Semantic analysis recognises exhaustive
`else`, boolean coverage (`yes`/`no` or `true`/`false`), and enum variant coverage.

## Branch Expressions

An `on` branch can be an expression, block, `return`, `break`, or `again`.
Statement-only branches type as `void`; value-producing branches must agree on a
common expected type.
