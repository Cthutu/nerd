main :: fn () {
    value :: for i := 0; i < 1; i += 1 {
        break 7
    }
}
¬
{
    "message": "Missing `else` for loop expression returning `untyped integer`",
    "source_file": "tests/errors/022-loop-expressions.e",
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
    "source_file": "tests/errors/022-loop-expressions.e",
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
    "source_file": "tests/errors/022-loop-expressions.e",
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
