value : i32 : 2.0
main :: fn () {}
¬
{
    "message": "Type mismatch: expected `i32`, found `untyped float`",
    "source_file": "tests/errors/147-numeric-literal-context.e",
    "primary_location": {
        "line": 1,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 15,
            "length": 3,
            "message": "This expression has type `untyped float`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
number :: 2.0
value : i32 : number
main :: fn () {}
¬
{
    "message": "Type mismatch: expected `i32`, found `untyped float`",
    "source_file": "tests/errors/147-numeric-literal-context.e",
    "primary_location": {
        "line": 2,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 15,
            "length": 6,
            "message": "This expression has type `untyped float`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
get_value :: fn () -> i32 { return 2.0 }
main :: fn () {}
¬
{
    "message": "Type mismatch: expected `i32`, found `untyped float`",
    "source_file": "tests/errors/147-numeric-literal-context.e",
    "primary_location": {
        "line": 1,
        "column": 36
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 36,
            "length": 3,
            "message": "This expression has type `untyped float`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
number : i32 : 2
value : f32 : number
main :: fn () {}
¬
{
    "message": "Type mismatch: expected `f32`, found `i32`",
    "source_file": "tests/errors/147-numeric-literal-context.e",
    "primary_location": {
        "line": 2,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 15,
            "length": 6,
            "message": "This expression has type `i32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

