bad :: fn (value: i32) -> i32 {
    return value?
}

main :: fn () {}
¬
{
    "message": "Type mismatch: expected `optional or result value`, found `i32`",
    "source_file": "tests/errors/089-optional-result.e",
    "primary_location": {
        "line": 2,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 17,
            "length": 1,
            "message": "This expression has type `i32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
bad :: fn () -> i32 {
    return "failed"!
}

main :: fn () {}
¬
{
    "message": "Type mismatch: expected `contextual result type T\\E`, found `error injection without an expected result type`",
    "source_file": "tests/errors/089-optional-result.e",
    "primary_location": {
        "line": 2,
        "column": 20
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 20,
            "length": 1,
            "message": "This expression has type `error injection without an expected result type`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
