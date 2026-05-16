# Expressions

Expression parsing is Pratt-based in `src/compiler/ast/expr.c`; semantic typing
and lvalue checks are in `src/compiler/sema/sema.c`.

## Primary Expressions

Primary expression starts include literals, identifiers, module labels, grouping,
arrays, blocks, function values, `on`, `for`, and interpolated strings.

```bnf
primary ::= INT | FLOAT | STRING | CSTRING | interpolated-string
          | 'yes' | 'no' | 'nil'
          | IDENT | '$' IDENT
          | '(' expression ')' | tuple
          | array-literal | range-literal
          | block | function-value | on-expression | for-statement
```

Blocks and loops are expression-capable. If their result is used, semantic
analysis requires compatible value-producing control flow.

## Operators

From lowest binding power to highest:

| Operators                                                                       | Notes                               |
| ------------------------------------------------------------------------------- | ----------------------------------- |
| `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `^=`, `|=`, `<<=`, `>>=`, `&&=`, `||=` | Assignment and compound assignment. |
| `||`                                                                            | Boolean OR.                         |
| `&&`                                                                            | Boolean AND.                        |
| `|`, `^`, `&`                                                                   | Bitwise operators.                  |
| `==`, `!=`                                                                      | Equality.                           |
| `<`, `<=`, `>`, `>=`                                                            | Ordering comparisons.               |
| `<<`, `>>`                                                                      | Shifts.                             |
| `+`, `-`                                                                        | Additive operators.                 |
| `*`, `/`, `%`                                                                   | Multiplicative operators.           |
| calls, indexing, slicing, fields, casts, plex literals, `with`, postfix `^`     | Postfix operators.                  |
| prefix `-`, `!`, `^`                                                            | Negation, logical not, address-of.  |

Assignments require assignable targets. Compound assignments lower through the
corresponding binary operation before assigning.

## Calls And Fields

```bnf
call        ::= expression '(' call-arg-list? ')'
call-arg    ::= expression | IDENT '=' expression
field       ::= expression '.' IDENT
tuple-field ::= expression '.' INT
cast        ::= expression '.as' '(' type [ ',' expression ] ')'
```

Named call arguments are parser-supported and checked against the resolved
function signature. Tuple fields use integer selectors.

## Indexing And Slicing

```bnf
index ::= expression '[' expression ']'
slice ::= expression '[' [ expression ] '..' [ expression ] ']'
```

Indexing is supported for arrays, slices, dynamic arrays, strings, and pointers.
Slicing with `..` is parser-supported. Inclusive `..=` range syntax is used in
array range literals and patterns, but slice postfix parsing currently accepts
only `..`.

## Aggregate Expressions

```bnf
tuple         ::= '(' expression ',' expression-list? ')'
array-literal ::= '[' expression-list? ']'
range-literal ::= '[' expression '..' expression ']'
                | '[' expression '..=' expression ']'
plex-literal  ::= target '{' plex-literal-field-list? '}'
plex-update   ::= expression 'with' '{' plex-literal-field-list? '}'
plex-field    ::= IDENT ':' expression | IDENT | '...'
```

Plex literal fields can be explicit (`x: value`) or shorthand (`x`) when the
field name is also the local expression name. Missing plex literal fields are
default-initialised by semantic analysis only when the literal includes `...`.

## Interpolated Strings

Interpolated strings type as `string`. Literal-only interpolations can fold to a
compile-time string. Runtime interpolations are temporary values and are rejected
in places that would let them escape their statement lifetime.
