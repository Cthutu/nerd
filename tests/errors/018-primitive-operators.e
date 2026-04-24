left: f64 = 1.5
right: f64 = 1.0
main :: fn () => left % right
¬
{
    "code": "0326",
    "message": "Operator `%` requires matching integer operands, found `f64` and `f64`",
    "source_file": "tests/errors/018-primitive-operators.e",
    "primary_location": {
        "line": 3,
        "column": 23
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 23,
            "length": 1,
            "message": "These operands have types `f64` and `f64`"
        }
    ],
    "notes": [],
    "help": [
        "Use `%` only with matching integer operands."
    ]
}
¬
left: f64 = 1.5
right: f64 = 1.0
main :: fn () => left & right
¬
{
    "code": "0326",
    "message": "Operator `&` requires matching integer operands, found `f64` and `f64`",
    "source_file": "tests/errors/018-primitive-operators.e",
    "primary_location": {
        "line": 3,
        "column": 23
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 23,
            "length": 1,
            "message": "These operands have types `f64` and `f64`"
        }
    ],
    "notes": [],
    "help": [
        "Use `&` only with matching integer operands."
    ]
}
¬
left: i32 = 1
right: i32 = 2
main :: fn () => left && right
¬
{
    "code": "0326",
    "message": "Operator `&&` requires matching bool operands, found `i32` and `i32`",
    "source_file": "tests/errors/018-primitive-operators.e",
    "primary_location": {
        "line": 3,
        "column": 23
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 23,
            "length": 2,
            "message": "These operands have types `i32` and `i32`"
        }
    ],
    "notes": [],
    "help": [
        "Use `&&` only with matching bool operands."
    ]
}
¬
left: string = "a"
right: string = "b"
main :: fn () => left < right
¬
{
    "code": "0326",
    "message": "Operator `<` requires matching numeric operands, found `string` and `string`",
    "source_file": "tests/errors/018-primitive-operators.e",
    "primary_location": {
        "line": 3,
        "column": 23
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 23,
            "length": 1,
            "message": "These operands have types `string` and `string`"
        }
    ],
    "notes": [],
    "help": [
        "Use `<` only with matching numeric operands."
    ]
}
