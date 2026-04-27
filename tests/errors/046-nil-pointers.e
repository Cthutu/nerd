main :: fn () {
    value: i32 = nil
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `pointer or slice type`, found `nil`",
    "source_file": "tests/errors/046-nil-pointers.e",
    "primary_location": {
        "line": 2,
        "column": 18
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 18,
            "length": 3,
            "message": "This expression has type `nil`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
