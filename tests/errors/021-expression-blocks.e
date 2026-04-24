value: i32: ${
    prn("fallthrough")
}
main :: fn () => value
¬
{
    "code": "0329",
    "message": "Missing `break` for expression block returning `i32`",
    "source_file": "tests/errors/021-expression-blocks.e",
    "primary_location": {
        "line": 1,
        "column": 13
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
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
    "code": "0304",
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
        continue
    }
    return 0
}
¬
{
    "code": "0328",
    "message": "`continue` can only be used inside a loop",
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
            "length": 8,
            "message": "This `continue` is not inside a `for` loop"
        }
    ],
    "notes": [],
    "help": [
        "Move `continue` into a `for` loop body."
    ]
}
