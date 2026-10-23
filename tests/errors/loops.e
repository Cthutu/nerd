use std.io

main :: fn () {
    for 1 {
        prn("bad")
    }
}
¬
{
  "message": "Type mismatch: expected `bool, box, optional, or result`, found `untyped integer`",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 4,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 9,
      "length": 1,
      "message": "This expression has type `untyped integer`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
use std.io

main :: fn () {
    for i := 0; i < 1; i += 1 {
    }
    prn(i)
}
¬
{
  "message": "Unknown symbol `i`",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 6,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 9,
      "length": 1,
      "message": "This symbol is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Add a binding for `i` or fix the spelling."
  ]
}

¬
main :: fn () {
    break
}
¬
{
  "message": "`break` can only be used inside a loop or expression block",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 2,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 5,
      "length": 5,
      "message": "This `break` is not inside a `for` loop or expression block"
    }
  ],
  "notes": [],
  "help": [
    "Move `break` into a `for` loop or expression block."
  ]
}

¬
main :: fn () {
    text : []u8 = "a"[..]
    for c in text {
        c^ := c^
    }
}
¬
{
  "message": "Invalid binding target before `:=`",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 4,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 12,
      "length": 1,
      "message": "`:=` starts a new binding here"
    }
  ],
  "notes": [
    "Bindings started with `:=` must begin with a symbol or supported destructuring pattern"
  ],
  "help": [
    "Use `=` to assign to an existing value such as a dereference or field access"
  ]
}

¬
main :: fn () {
    for {
        for on yes => break {
        }
    }
}
¬
{
  "message": "Expected LeftBrace `{` but found RightBrace `}`",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 5,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 5,
      "length": 1,
      "message": "Found RightBrace `}` here"
    }
  ],
  "notes": [],
  "help": [
    "Check for a missing closing delimiter or misplaced operator"
  ]
}

¬
main :: fn () {
    values: [3]i32 = [1, 2, 3]
    for ^value in values {
    }
}
¬
{
  "message": "Expected LeftParen `(` but found Keyword `in`",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 3,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 16,
      "length": 2,
      "message": "Found Keyword `in` here"
    }
  ],
  "notes": [
    "A top-level `in` in a condition loop would be ambiguous with `for item in iterable`."
  ],
  "help": [
    "Parenthesise the membership condition: `for (value in [start .. end]) { ... }`"
  ]
}

¬
main :: fn () {
    again
}
¬
{
  "message": "`again` can only be used inside a loop",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 2,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 5,
      "length": 5,
      "message": "This `again` is not inside a `for` loop"
    }
  ],
  "notes": [],
  "help": [
    "Move `again` into a `for` loop body."
  ]
}

¬
main :: fn () {
    for {
        again $missing
    }
}
¬
{
  "message": "Unknown control label `$missing`",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 3,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 9,
      "length": 5,
      "message": "No enclosing expression block or loop has this label"
    }
  ],
  "notes": [],
  "help": [
    "Use the label from an enclosing `$label { ... }` block or `for ... $label { ... }` loop."
  ]
}

¬
main :: fn () {
    $block {
        for {
            again $block
        }
    }
}
¬
{
  "message": "`again` label `$block` does not name a loop",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 4,
    "column": 13
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 13,
      "length": 5,
      "message": "This label names an expression block, not a `for` loop"
    }
  ],
  "notes": [],
  "help": [
    "Use `break $block` for an expression block, or `again` to a `for ... $label { ... }` loop."
  ]
}

¬
main :: fn () {
    for on yes => break {
    }
}
¬
{
  "message": "Expected LeftBrace `{` but found RightBrace `}`",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 4,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 1,
      "length": 1,
      "message": "Found RightBrace `}` here"
    }
  ],
  "notes": [],
  "help": [
    "Check for a missing closing delimiter or misplaced operator"
  ]
}

¬
main :: fn () {
    value :: for i := 0; i < 1; i += 1 {
        break 7
    }
}
¬
{
  "message": "Missing `else` for loop expression returning `untyped integer`",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 2,
    "column": 14
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 14,
      "length": 3,
      "message": "This loop can finish normally without breaking with a `untyped integer` value"
    }
  ],
  "notes": [
    "Finite value-producing loop expressions must make normal loop exhaustion explicit with `else { break <expr> }`."
  ],
  "help": [
    "Add an `else` block that breaks with the loop result, or make the loop infinite if normal exhaustion is impossible."
  ]
}

¬
main :: fn () {
    value :: for i := 0; i < 1; i += 1 {
        break 7
    } else {
        break "nope"
    }
}
¬
{
  "message": "Type mismatch: expected `untyped integer`, found `string`",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 5,
    "column": 15
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 15,
      "length": 6,
      "message": "This expression has type `string`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    for i := 0; i < 1; i += 1 {
    } else {
        break 0
    }
}
¬
{
  "message": "Loop `else` requires a value-producing loop expression",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 3,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 12,
      "length": 1,
      "message": "This `else` block has no matching value-producing `break` in the loop"
    }
  ],
  "notes": [],
  "help": [
    "Use `else` only on loops that return a value with `break <expr>`, or remove the `else` block."
  ]
}

¬
main :: fn () {
    for {
        break
    } else {
    }
}
¬
{
  "message": "Loop `else` cannot run on an infinite loop",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 4,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 12,
      "length": 1,
      "message": "This `else` block is unreachable because the loop has no exhaustion condition"
    }
  ],
  "notes": [
    "Loop `else` blocks run only when a finite loop exhausts without `break`."
  ],
  "help": [
    "Remove the `else` block, add a loop condition, or use `break` to handle the exit path explicitly."
  ]
}

¬
main :: fn () {
    for i in 0 .. 10 {
    }
}
¬
{
  "message": "Expected LeftBrace `{` but found Range `..`",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 2,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 16,
      "length": 2,
      "message": "Found Range `..` here"
    }
  ],
  "notes": [
    "Range expressions in `for` loops must be enclosed in square brackets"
  ],
  "help": [
    "Add square brackets around the range: `for item in [start .. end] { ... }`"
  ]
}

¬
main :: fn () {
    value :: for yes $outer {
        for { break $outer 7 }
    }
}
¬
{
  "message": "Missing `else` for loop expression returning `untyped integer`",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 2,
    "column": 14
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 14,
      "length": 3,
      "message": "This loop can finish normally without breaking with a `untyped integer` value"
    }
  ],
  "notes": [
    "Finite value-producing loop expressions must make normal loop exhaustion explicit with `else { break <expr> }`."
  ],
  "help": [
    "Add an `else` block that breaks with the loop result, or make the loop infinite if normal exhaustion is impossible."
  ]
}

¬
main :: fn () {
    value :: for $outer {
        for { break $outer 7 }
        break $outer "nope"
    }
}
¬
{
  "message": "Type mismatch: expected `untyped integer`, found `string`",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 4,
    "column": 22
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 22,
      "length": 6,
      "message": "This expression has type `string`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    value :: for no $outer {
        for { break 7 }
    } else {
        break 0
    }
}
¬
{
  "message": "Loop `else` requires a value-producing loop expression",
  "source_file": "tests/errors/loops.e",
  "primary_location": {
    "line": 4,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 12,
      "length": 1,
      "message": "This `else` block has no matching value-producing `break` in the loop"
    }
  ],
  "notes": [],
  "help": [
    "Use `else` only on loops that return a value with `break <expr>`, or remove the `else` block."
  ]
}
