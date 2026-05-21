pair :: fn () -> (i32, i32) {
    return (2, 3)
}

main :: fn () {
    a, b := pair()
}
¬
{
    "message": "Tuple destructuring bindings must be parenthesized",
    "source_file": "tests/errors/085-bare-tuple-destructuring.e",
    "primary_location": {
        "line": 6,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 5,
            "length": 4,
            "message": "bare tuple destructuring target starts here"
        }
    ],
    "notes": [
        "Comma-separated binding targets are parsed as destructuring only when they are wrapped in parentheses"
    ],
    "help": [
        "Use `(a, b) := value` instead of `a, b := value`"
    ]
}
