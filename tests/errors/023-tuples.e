main :: fn () {
    value :: 1
    return value.0
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `tuple`, found `untyped integer`",
    "source_file": "tests/errors/023-tuples.e",
    "primary_location": {
        "line": 3,
        "column": 18
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 18,
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
main :: fn () {
    pair :: (1, "one")
    return pair.2
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `valid tuple field`, found `(i32, string)`",
    "source_file": "tests/errors/023-tuples.e",
    "primary_location": {
        "line": 3,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 17,
            "length": 1,
            "message": "This expression has type `(i32, string)`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
pair: (i32, string): (1, 2)
main :: fn () => pair.0
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `string`, found `untyped integer`",
    "source_file": "tests/errors/023-tuples.e",
    "primary_location": {
        "line": 1,
        "column": 26
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 26,
            "length": 1,
            "message": "This expression has type `untyped integer`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
