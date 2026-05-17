use std.io

value: i32: ${
    prn("fallthrough")
}
main :: fn () => value
¬
{
    "message": "Missing `break` for expression block returning `i32`",
    "source_file": "tests/errors/021-expression-blocks.e",
    "primary_location": {
        "line": 3,
        "column": 13
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 13,
            "length": 1,
            "message": "This expression block can reach the end without breaking with a `i32` value"
        }
    ],
    "notes": [
        "Value-producing expression blocks must exit every reachable path with `break <expr>`."
    ],
    "help": [
        "Add `break <expr>` before the block ends or use a void context if the block should not produce a value."
    ]
}
¬
value: i32: ${
    break "bad"
}
main :: fn () => value
¬
{
    "message": "Type mismatch: expected `i32`, found `string`",
    "source_file": "tests/errors/021-expression-blocks.e",
    "primary_location": {
        "line": 2,
        "column": 11
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 11,
            "length": 5,
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
    value :: ${
        again
    }
    return 0
}
¬
{
    "message": "`again` can only be used inside a loop",
    "source_file": "tests/errors/021-expression-blocks.e",
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
    break
}
¬
{
    "message": "`break` can only be used inside a loop or expression block",
    "source_file": "tests/errors/021-expression-blocks.e",
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
    value :: $answer {
        break $missing 7
    }
    return value
}
¬
{
    "message": "Unknown control label `$missing`",
    "source_file": "tests/errors/021-expression-blocks.e",
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
