+42
¬
{
    "message": "Missing value before Plus `+`",
    "source_file": "tests/errors/002-missing-values.e",
    "primary_location": {
        "line": 1,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 1,
            "length": 1,
            "message": "Plus `+` cannot appear here"
        }
    ],
    "notes": [],
    "help": [
        "Insert a literal, parenthesized expression, or unary operator"
    ]
}
¬
main :: fn () {
    arena ar
}
¬
{
    "message": "`arena` is a type, not a declaration keyword",
    "source_file": "tests/errors/002-missing-values.e",
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
            "message": "`arena` names the built-in arena type"
        },
        {
            "kind": "secondary",
            "line": 2,
            "column": 11,
            "length": 2,
            "message": "The variable name appears after the type"
        }
    ],
    "notes": [],
    "help": [
        "Write `ar: arena` to declare an arena, or `ar := arena(...)` to construct one."
    ]
}
¬
63+
¬
{
    "message": "Missing value before EOF",
    "source_file": "tests/errors/002-missing-values.e",
    "primary_location": {
        "line": 1,
        "column": 4
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 4,
            "length": 0,
            "message": "EOF cannot appear here"
        }
    ],
    "notes": [],
    "help": [
        "Insert a literal, parenthesized expression, or unary operator"
    ]
}
¬
28 29
¬
{
    "message": "Missing operator before Integer",
    "source_file": "tests/errors/002-missing-values.e",
    "primary_location": {
        "line": 1,
        "column": 4
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 4,
            "length": 2,
            "message": "Integer starts a new expression here"
        }
    ],
    "notes": [],
    "help": [
        "Insert an operator such as +, -, *, /, or %% between values"
    ]
}
¬
@
¬
{
    "message": "Expected Symbol but found EOF",
    "source_file": "tests/errors/002-missing-values.e",
    "primary_location": {
        "line": 1,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 1,
            "length": 1,
            "message": "Found EOF here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}
¬
main :: fn () {
    _numbers := [10 ..]i32
}
¬
{
    "message": "Type used where a value was expected",
    "source_file": "tests/errors/002-missing-values.e",
    "primary_location": {
        "line": 2,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 17,
            "length": 1,
            "message": "This begins type syntax"
        }
    ],
    "notes": [
        "Bindings declared with `:=` require a runtime initializer"
    ],
    "help": [
        "Use `_numbers: Type` to declare a default-initialized variable, or provide a runtime value after `:=`."
    ]
}
