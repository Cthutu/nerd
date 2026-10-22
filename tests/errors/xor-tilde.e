main :: fn () -> i32 {
    left := 7
    return left ^ 3
}

¬
{
    "message": "Type mismatch: expected `pointer`, found `i32`",
    "source_file": "tests/errors/xor-tilde.e",
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
            "message": "This expression has type `i32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
main :: fn () {
    bits := 7
    bits ^= 3
}

¬
{
    "message": "Type mismatch: expected `pointer`, found `i32`",
    "source_file": "tests/errors/xor-tilde.e",
    "primary_location": {
        "line": 3,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 10,
            "length": 1,
            "message": "This expression has type `i32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
main :: fn () -> f64 {
    return 1.5 ~ 2.0
}

¬
{
    "message": "Operator `~` requires matching integer operands, found `f64` and `f64`",
    "source_file": "tests/errors/xor-tilde.e",
    "primary_location": {
        "line": 2,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 16,
            "length": 1,
            "message": "These operands have types `f64` and `f64`"
        }
    ],
    "notes": [],
    "help": [
        "Use `~` only with matching integer operands."
    ]
}
