main :: fn () -> i32 {
    value: i32 = undefined
    return value
}
¬
{
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
¬
check :: fn (flag: bool) -> i32 {
    value: i32 = undefined
    on flag => value = 1
    return value
}

main :: fn () -> i32 {
    return check(yes)
}
¬
{
    "message": "Cannot read `value` before it has been assigned",
    "source_file": "tests/errors/052-definite-assignment.e",
    "primary_location": {
        "line": 4,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
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
¬
main :: fn () -> i32 {
    value: i32 = undefined
    for yes {
        value = 1
    }
    return value
}
¬
{
    "message": "Cannot read `value` before it has been assigned",
    "source_file": "tests/errors/052-definite-assignment.e",
    "primary_location": {
        "line": 6,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
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
