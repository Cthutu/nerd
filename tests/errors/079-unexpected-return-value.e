make_value :: fn () {
    return 1
}

main :: fn () {
    make_value()
}
¬
{
    "message": "Cannot return a value from a function with no return type",
    "source_file": "tests/errors/079-unexpected-return-value.e",
    "primary_location": {
        "line": 2,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 5,
            "length": 6,
            "message": "This `return` provides a value, but the function returns no value"
        }
    ],
    "notes": [
        "Functions written without `-> Type` return no value."
    ],
    "help": [
        "Remove the returned expression, or add `-> Type` to the function signature."
    ]
}
