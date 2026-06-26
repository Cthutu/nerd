bad :: fn () -> ! {
    return 1
}

main :: fn () {}
¬
{
    "message": "Type mismatch: expected `!`, found `untyped integer`",
    "source_file": "tests/errors/087-never-type.e",
    "primary_location": {
        "line": 2,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 12,
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
bad :: fn () -> ! {
}

main :: fn () {}
¬
{
    "message": "Missing return for function returning `!`",
    "source_file": "tests/errors/087-never-type.e",
    "primary_location": {
        "line": 1,
        "column": 8
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 8,
            "length": 2,
            "message": "This function can reach the end without returning a `!` value"
        }
    ],
    "notes": [
        "Block-bodied functions with explicit return types must return a value before they end."
    ],
    "help": [
        "Add `return <expr>` to the function body or remove the explicit return type if the function should not produce a value."
    ]
}
