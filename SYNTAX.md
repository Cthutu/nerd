# Nerd Programming Language Syntax

## Comments

```nerd
-- This is a single-line comment
```

## Syntax tree

```
<program> := <binding>*
<binding> := <symbol> ':' ':' <type-or-expression>
<type-or-expression> := <type> | <expression>
<type> := <function-declaration>
<function-declaration> := 'fn' '(' ')' '=>' <expression>
```
