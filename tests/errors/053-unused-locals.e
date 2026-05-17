main :: fn () -> i32 {
    value := 1
    return 0
}
¬
{
    "message": "Unused local variable `value`",
    "source_file": "tests/errors/053-unused-locals.e",
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
            "message": "This local variable is never read"
        }
    ],
    "notes": [
        "Assigning to a variable does not count as using it."
    ],
    "help": [
        "Remove `value` or prefix the name with `_` if it is deliberately unused."
    ]
}
¬
main :: fn () -> i32 {
    return helper(1)
}

helper :: fn (value: i32) -> i32 {
    return 0
}
¬
{
    "message": "Unused parameter `value`",
    "source_file": "tests/errors/053-unused-locals.e",
    "primary_location": {
        "line": 5,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 15,
            "length": 5,
            "message": "This parameter is never read"
        }
    ],
    "notes": [
        "Assigning to a variable does not count as using it."
    ],
    "help": [
        "Remove `value` or prefix the name with `_` if it is deliberately unused."
    ]
}
