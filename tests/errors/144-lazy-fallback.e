main :: fn () { value := 7 \ 42 }
¬
{
    "message": "Type mismatch: expected `optional or result value`, found `untyped integer`",
    "source_file": "tests/errors/144-lazy-fallback.e",
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
        "The left operand of `\\` must have type `?T` or `T\\E`."
    ]
}
¬
main :: fn () {
    value : ?i32
    recovered := value \ "wrong"
}
¬
{
    "message": "Type mismatch: expected `i32`, found `string`",
    "source_file": "tests/errors/144-lazy-fallback.e",
    "primary_location": {
        "line": 3,
        "column": 26
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 26,
            "length": 7,
            "message": "This expression has type `string`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
    value : ?u8
    recovered := value \ 256
}
¬
{
    "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
    "source_file": "tests/errors/144-lazy-fallback.e",
    "primary_location": {
        "line": 3,
        "column": 26
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 26,
            "length": 3,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
    value : i32\string
    other : ?i32
    recovered := value \ other
}
¬
{
    "message": "Type mismatch: expected `i32`, found `?i32`",
    "source_file": "tests/errors/144-lazy-fallback.e",
    "primary_location": {
        "line": 4,
        "column": 26
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 26,
            "length": 5,
            "message": "This expression has type `?i32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
    value : ?i32
    recovered := value \ }
¬
{
    "message": "Missing value before RightBrace `}`",
    "source_file": "tests/errors/144-lazy-fallback.e",
    "primary_location": {
        "line": 3,
        "column": 26
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 26,
            "length": 1,
            "message": "RightBrace `}` cannot appear here"
        }
    ],
    "notes": [],
    "help": [
        "Insert a literal, parenthesized expression, or unary operator"
    ]
}
¬
main :: fn () {
    value : ?void
    value \ 42
}
¬
{
    "message": "Type mismatch: expected `void`, found `untyped integer`",
    "source_file": "tests/errors/144-lazy-fallback.e",
    "primary_location": {
        "line": 3,
        "column": 13
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 13,
            "length": 2,
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
    value : ?i32
    value \ 42
}
¬
{
    "message": "Expression result of type `i32` is not used",
    "source_file": "tests/errors/144-lazy-fallback.e",
    "primary_location": {
        "line": 3,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 5,
            "length": 5,
            "message": "This expression produces a value, but the value is discarded"
        }
    ],
    "notes": [
        "Only `void` expressions can be used as standalone statements."
    ],
    "help": [
        "Bind the result to `_` when the value is intentionally ignored."
    ]
}
