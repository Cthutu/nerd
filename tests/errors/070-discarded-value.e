value :: fn () -> i32 {
    return 1
}

main :: fn () {
    value()
}
¬
{
    "message": "Expression result of type `i32` is not used",
    "source_file": "tests/errors/070-discarded-value.e",
    "primary_location": {
        "line": 6,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 5,
            "length": 5,
            "message": "This expression produces a value, but the value is discarded"
        }
    ],
    "notes": [
        "Only `void` expressions can be used as standalone statements."
    ],
    "help": [
        "Bind the result to `_` when the value is intentionally ignored."
    ]
}
