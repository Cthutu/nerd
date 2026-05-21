# Nerd Syntax

This document is generated from the current parser implementation, principally
`src/compiler/ast/parse.c` and `src/compiler/ast/expr.c`. Lexical tokens and
literal tokenisation are defined separately in [`lexer.md`](lexer.md).

The grammar below is BNF-like rather than a mechanically executable grammar.
The parser is Pratt-based for expressions, so operator precedence is documented
as a precedence table.

## Notation

| Form                                | Meaning                                                                                      |
| ----------------------------------- | -------------------------------------------------------------------------------------------- |
| `'text'`                            | Literal source text.                                                                         |
| `IDENT`                             | Non-keyword identifier token.                                                                |
| `INT`, `FLOAT`, `STRING`, `CSTRING` | Lexer literal tokens.                                                                        |
| `[ item ]`                          | Optional item.                                                                               |
| `{ item }`                          | Zero or more repetitions.                                                                    |
| `item | item`                       | Alternative forms.                                                                           |
| `item-list`                         | Comma-separated list of `item`, with a trailing comma accepted where the parser accepts one. |

## Program

```bnf
program         ::= { top-level-item }

top-level-item  ::= [ 'pub' ] top-level-public-item
                  | top-level-private-item

top-level-public-item
                ::= binding
                  | variable
                  | ffi-declaration
                  | use-declaration

top-level-private-item
                ::= top-level-public-item
                  | top-level-on
                  | top-level-assert-on
                  | pragma
                  | impl-block
                  | source-test

top-level-on    ::= 'on' [ '!' ] STRING '{' { top-level-item } '}'
top-level-assert-on
                ::= 'assert' 'on' [ '!' ] STRING
source-test     ::= 'test' STRING block
impl-block      ::= 'impl' type where-clause? '{' { top-level-item } '}'
                  | 'impl' type 'for' type where-clause? '{' { top-level-item } '}'
```

`pub` applies only to bindings, variables, FFI declarations, and `use`
declarations.

Top-level `assert on` is a file-level platform assertion. It fails compilation
when the platform key is not enabled; it is not a runtime statement.

## Declarations And Bindings

```bnf
binding         ::= IDENT '::' bind-value
                  | IDENT ':' type ':' bind-value

bind-value      ::= declaration
                  | expression
                  | type

variable        ::= IDENT ':=' expression
                  | IDENT ':' type [ '=' variable-initializer ]

variable-initializer
                ::= expression
                  | 'undefined'

declaration     ::= function-declaration
                  | ffi-declaration
                  | use-module-ref
                  | trait-declaration

function-declaration
                ::= 'fn' generic-params? '(' named-param-list? ')' [ '->' type ] where-clause? function-body

function-body   ::= '=>' expression
                  | block

trait-declaration
                ::= 'trait' generic-params? [ 'for' IDENT ] '{' { trait-member } '}'

trait-member    ::= IDENT '::' function-type

named-param     ::= IDENT ':' type [ '=' expression ]
named-param-list
                ::= named-param { ',' named-param }

generic-params  ::= '[' IDENT { ',' IDENT } ']'

where-clause    ::= 'where' where-constraint { ',' where-constraint }

where-constraint
                ::= IDENT ':' type
```

The binding operators are two adjacent tokens in the lexer: `::` is parsed as
`':' ':'`, and `:=` is parsed as `':' '='`.

Trait declarations are registered as semantic declarations, formatted, exposed
to LSP document symbols, and kept out of generated backend output. Trait
declarations may have generic parameter lists such as `trait [Item] { ... }`.
Generic trait declarations are parsed and formatted, but implementing generic
traits is future semantic work. Trait declarations may name the self type with
`trait for Value`; otherwise the self type is named `Self`. Trait
implementation blocks validate that all required member names are present, and
local trait implementations validate compatible member signatures after
self-type substitution. Impl members are callable through normal receiver
method syntax. Duplicate concrete implementations and overlapping generic
implementations for the same trait are rejected. Implementations may target
compound types and primitive built-in types. A trait implementation block is
the complete implementation for one trait/type pair; implementations are not
split or merged across multiple blocks. Generic implementation parameters are
inferred from the target type, for example
`impl Display for Box[T] where T: Display { ... }`.
Generic functions and generic impl blocks may carry `where` constraints. The
formatter places `where` on a new line, such as `where T: Display`, and aligns
multi-constraint clauses. Constraint clauses are parsed and formatted,
constraint trait names must resolve to known traits, and each concrete
instantiation must satisfy every constraint. Generic methods may also receive
explicit type arguments with
`value.method[T](...)`. Receiver method lookup prefers inherent impl methods
before trait impl methods. If multiple trait impl methods with the same name are
valid for the receiver type, the receiver call is ambiguous. A trait member may
be called explicitly with `<Trait>.<member>(value, ...)`, which restricts lookup
to implementations of that trait and uses the first argument as the receiver.
When the implementation type cannot be inferred from a receiver argument, use
`<Trait>[Type].<member>(...)` to select the implementation directly. Generic
trait members are not placed in the ordinary function namespace: a bare call
such as `show(value)` does not resolve to a trait member.

## Modules, FFI, And Pragmas

```bnf
use-declaration ::= 'use' module-path
                  | 'use' module-path '{' grouped-use-entry { grouped-use-entry } '}'
                  | 'use' expression

use-module-ref  ::= 'use' module-path

grouped-use-entry
                ::= module-path
                  | module-path '{' grouped-use-entry { grouped-use-entry } '}'

module-path     ::= IDENT { '.' IDENT }

ffi-declaration ::= 'ffi' expression ffi-entry
                  | 'ffi' expression '{' { [ 'pub' ] ffi-entry } '}'
                  | intrinsic-declaration

ffi-entry       ::= IDENT [ '::' IDENT ] '(' ffi-param-list? ')' [ '->' type ]

ffi-param       ::= [ IDENT ':' ] type
ffi-param-list  ::= ffi-param { ',' ffi-param } [ ',' '...' ]
                  | '...'

intrinsic-declaration
                ::= 'intrinsic' STRING '(' ffi-param-list? ')' [ '->' type ]

pragma          ::= 'pragma' IDENT [ '(' pragma-param-list? ')' ]
pragma-param    ::= INT | FLOAT | STRING | 'yes' | 'no'
pragma-param-list
                ::= pragma-param { ',' pragma-param }
```

The parser has storage for default expressions in FFI signatures, but FFI
defaults are not part of the language surface documented here.

## Blocks And Statements

```bnf
block           ::= '{' { statement } '}'

statement       ::= pragma
                  | ffi-declaration
                  | use-declaration
                  | 'defer' statement
                  | 'assert' expression [ ',' expression ]
                  | break-statement
                  | again-statement
                  | return-statement
                  | destructure-statement
                  | block
                  | for-statement
                  | binding
                  | variable
                  | expression

break-statement ::= 'break' [ label ] [ expression ]
                  | 'break' [ label ] 'on' expression '=>' expression

again-statement ::= 'again' [ label ]

return-statement
                ::= 'return' [ expression ]

label           ::= '$' IDENT
```

`again` resumes loop execution at the next iteration.

## Built-In Macros

Built-in macro expressions start with `@`.

```bnf
built-in-macro ::= '@file' | '@line'
```

`@file` expands to a `string` literal containing the current source filename.
`@line` expands to an untyped integer literal containing the current source line
number. Line numbers are 1-based.

When a built-in macro appears in a default parameter expression, it is evaluated
at the call site where that default argument is inserted.

## Destructuring

```bnf
destructure-statement
                ::= destructure-pattern '=' expression
                  | destructure-pattern '::' expression
                  | destructure-pattern ':=' expression
                  | destructure-pattern ':' type ':' expression
                  | destructure-pattern ':' type '=' expression

destructure-pattern
                ::= IDENT
                  | '_'
                  | '(' destructure-pattern-list? ')'
                  | '{' destructure-field-list? '}'
                  | pattern

destructure-field
                ::= IDENT [ ':' ( IDENT | '_' ) ]
                  | '_'
destructure-field-list
                ::= destructure-field { ',' destructure-field } [ ',' ]
destructure-pattern-list
                ::= destructure-pattern { ',' destructure-pattern } [ ',' ]
```

Tuple and plex destructuring patterns are restricted to symbol binds and `_`
ignores, with nested full patterns accepted only through the general `pattern`
fallback.

## Loops

All loop forms use the `for` keyword.

```bnf
for-statement   ::= 'for' for-form [ label ] block [ 'else' block ]

for-form        ::= empty-for
                  | condition-for
                  | for-in
                  | c-style-for

empty-for       ::= /* no clause before the loop body */
condition-for   ::= expression
for-in          ::= IDENT 'in' expression
                  | IDENT ',' IDENT 'in' expression

c-style-for     ::= [ for-item-list ] ';' [ expression ] ';' [ for-item-list ]
for-item        ::= binding
                  | variable
                  | expression
for-item-list   ::= for-item { ',' for-item }
```

The `for-in` form accepts only plain identifiers before `in`.

## Expressions

```bnf
expression      ::= assignment

assignment      ::= binary-expression [ assignment-operator expression ]

binary-expression
                ::= postfix-expression { binary-operator postfix-expression }

assignment-operator
                ::= '=' | '+=' | '-=' | '*=' | '/=' | '%='
                  | '&=' | '^=' | '|=' | '<<=' | '>>='
                  | '&&=' | '||='

binary-operator ::= '||' | '&&' | '|' | '^' | '&'
                  | '==' | '!=' | '<' | '<=' | '>' | '>='
                  | '<<' | '>>' | '+' | '-' | '*' | '/' | '%'

primary         ::= INT
                  | FLOAT
                  | STRING
                  | CSTRING
                  | interpolated-string
                  | 'yes'
                  | 'no'
                  | 'nil'
                  | built-in-macro
                  | IDENT
                  | '$' IDENT
                  | tuple-or-group
                  | array-or-range
                  | block
                  | function-value
                  | on-expression
                  | for-statement

prefix-expression
                ::= primary
                  | '-' prefix-expression
                  | '!' prefix-expression
                  | '^' prefix-expression

postfix-expression
                ::= prefix-expression { postfix }

postfix         ::= '(' call-arg-list? ')'
                  | '[' expression ']'
                  | generic-args
                  | '[' [ expression ] '..' [ expression ] ']'
                  | '.' IDENT
                  | '.' INT
                  | '.as' '(' type [ ',' expression ] ')'
                  | 'with' plex-literal-body
                  | plex-literal-body
                  | '^'

call-arg        ::= expression
                  | IDENT '=' expression
call-arg-list   ::= call-arg { ',' call-arg }
generic-args    ::= '[' type { ',' type } ']'

tuple-or-group  ::= '(' expression ')'
                  | '(' expression ',' expression-list? ')'

array-or-range  ::= '[' expression-list? ']'
                  | '[' expression '..' expression ']'
                  | '[' expression '..=' expression ']'

function-value  ::= 'fn' generic-params? '(' named-param-list? ')' [ '->' type ] function-body
```

The bracket postfix is intentionally ambiguous between indexing and explicit
generic arguments until semantic analysis knows whether the target is a generic
function or method.

`interpolated-string` starts with `$"` and is parsed from the token stream
described in [`lexer.md`](lexer.md#interpolated-strings). A `+"..."` string
continuation can be parsed after a string-like expression and is folded into the
same interpolated string expression.

### Operator Precedence

From lowest binding power to highest:

| Operators                                                                                                                 | Associativity |
| ------------------------------------------------------------------------------------------------------------------------- | ------------- |
| `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `^=`, `|=`, `<<=`, `>>=`, `&&=`, `||=`                                           | Right         |
| `||`                                                                                                                      | Left          |
| `&&`                                                                                                                      | Left          |
| `|`                                                                                                                       | Left          |
| `^`                                                                                                                       | Left          |
| `&`                                                                                                                       | Left          |
| `==`, `!=`                                                                                                                | Left          |
| `<`, `<=`, `>`, `>=`                                                                                                      | Left          |
| `<<`, `>>`                                                                                                                | Left          |
| `+`, `-`                                                                                                                  | Left          |
| `*`, `/`, `%`                                                                                                             | Left          |
| Calls, indexing, slicing, field access, tuple field access, casts, plex literals, `with` updates, postfix `^` dereference | Left/postfix  |
| Prefix `-`, `!`, `^` address-of                                                                                           | Prefix        |

## `on` Expressions

All conditional and pattern-branching forms use the `on` keyword.

```bnf
on-expression   ::= 'on' expression '=>' branch-expression 'else' branch-expression
                  | 'on' expression '=>' branch-expression
                  | 'on' '{' condition-on-branch-list '}'
                  | 'on' expression '{' pattern-on-branch-list '}'

condition-on-branch
                ::= expression '=>' branch-expression
                  | 'else' '=>' branch-expression

pattern-on-branch
                ::= pattern-list [ 'as' IDENT ] [ 'on' expression ] '=>' branch-expression
                  | 'else' [ 'as' IDENT ] '=>' branch-expression

pattern-list    ::= pattern { ',' pattern }

branch-expression
                ::= expression
                  | block
                  | return-statement
                  | break-statement
                  | again-statement
```

The single-line `on condition => value` form is valid where statement
boundaries permit it. The `else` branch is required for the boolean expression
form that produces a value.

Inside block-form `on`, branch expressions stop before the next branch head.
For comparison-pattern heads such as `< 0` or `>= 10`, a preceding newline is
one of the hints that the comparison starts a new branch rather than continuing
the previous branch expression.

## Patterns

```bnf
pattern         ::= IDENT
                  | 'for' expression
                  | comparison-pattern
                  | '_'
                  | tuple-pattern
                  | plex-pattern
                  | typed-plex-pattern
                  | enum-variant-pattern
                  | range-pattern
                  | expression

comparison-pattern
                ::= ('==' | '!=' | '<' | '<=' | '>' | '>=') expression

tuple-pattern   ::= '(' pattern-list? ')'

plex-pattern    ::= '{' plex-pattern-field-list? '}'
typed-plex-pattern
                ::= type plex-pattern

plex-pattern-field
                ::= IDENT
                  | IDENT ':' pattern
                  | '_'
plex-pattern-field-list
                ::= plex-pattern-field { ',' plex-pattern-field } [ ',' ]

enum-variant-pattern
                ::= module-path '(' pattern-list? ')'

range-pattern   ::= expression '..' expression
                  | expression '..=' expression

A bare identifier in a pattern binds the matched value unless it is resolved as
an enum variant by the expected enum type. Use `for expression` to compare with
an existing runtime value.
```

## Types

```bnf
type            ::= type-name
                  | '(' type ')'
                  | tuple-type
                  | slice-type
                  | array-type
                  | dynamic-array-type
                  | pointer-type
                  | plex-type
                  | union-type
                  | enum-type
                  | function-type

type-path       ::= IDENT { '.' IDENT }
type-application
                ::= '[' type-list? ']'
type-name       ::= type-path { type-application }
type-list       ::= type { ',' type }

tuple-type      ::= '(' type ',' type-list? ')'

slice-type      ::= '[]' type
array-type      ::= '[' expression ']' type
dynamic-array-type
                ::= '[..]' type
                  | '[' expression '..]' type

pointer-type    ::= '^' type

plex-type       ::= 'plex' generic-params? plex-annotation* '{' plex-field* '}'
union-type      ::= 'union' generic-params? '{' plex-field* '}'
plex-annotation ::= '#c' | '#packed'
plex-field      ::= IDENT type

enum-type       ::= 'enum' generic-params? '{' enum-variant-list? '}'
enum-variant    ::= IDENT [ '(' type-list? ')' ] [ '=' expression ]
enum-variant-list
                ::= enum-variant { [ ',' ] enum-variant } [ ',' ]

function-type   ::= 'fn' generic-params? '(' function-type-param-list? ')' [ '->' type ]
function-type-param
                ::= [ IDENT ':' ] type
function-type-param-list
                ::= function-type-param { ',' function-type-param } [ ',' ]
```

Plex field definitions use `field Type` with no colon. Colons are used in plex
literals and plex patterns.

## Literals And Aggregate Expressions

```bnf
expression-list ::= expression { ',' expression } [ ',' ]

plex-literal-body
                ::= '{' plex-literal-field-list? '}'

plex-literal-field
                ::= IDENT ':' expression
                  | IDENT
                  | '...'
plex-literal-field-list
                ::= plex-literal-field { ',' plex-literal-field } [ ',' ]
```

A plex literal body is attached as a postfix expression to a type or expression
target, for example `Point { x: 1, y: 2 }`. A `...` field marker requests
default initialisation for omitted fields. The `with` postfix applies a plex
update body to an existing expression.
