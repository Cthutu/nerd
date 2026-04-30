main :: fn () -> i32 {
    value: i32 = undefined
    return value
}
¬
{
    "code": "0334",
    "message": "Cannot read `value` before it has been assigned",
    "source_file": "tests/errors/052-definite-assignment.e",
    "primary_location": {
        "line": 3,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 12,
            "length": 5,
            "message": "`value` is read here"
        },
        {
            "kind": "secondary",
            "line": 2,
            "column": 5,
            "length": 5,
            "message": "`value` is declared with `undefined` here"
        }
    ],
    "notes": [
        "Variables declared with `undefined` must be assigned before they are read."
    ],
    "help": [
        "Assign to `value` on every path before using its value."
    ]
}
