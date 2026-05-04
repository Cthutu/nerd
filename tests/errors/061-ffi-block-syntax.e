ffi "c" {
    stdin :: 0.as(^void)
}

main :: fn () {}
¬
{
    "code": "0203",
    "message": "Expected LeftParen `(` but found Colon `:`",
    "source_file": "tests/errors/061-ffi-block-syntax.e",
    "primary_location": {
        "line": 2,
        "column": 11
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 11,
            "length": 1,
            "message": "Found Colon `:` here"
        }
    ],
    "notes": [
        "FFI blocks only contain foreign function signatures of the form `name (...) -> T`."
    ],
    "help": [
        "Move constants and Nerd bindings outside the `ffi` block."
    ]
}
¬
ffi "c" {
    bad () => i32
}

main :: fn () {}
¬
{
    "code": "0203",
    "message": "Expected ThinArrow `->` but found FatArrow `=>`",
    "source_file": "tests/errors/061-ffi-block-syntax.e",
    "primary_location": {
        "line": 2,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 12,
            "length": 2,
            "message": "Found FatArrow `=>` here"
        }
    ],
    "notes": [
        "FFI signatures use `->` for return types; `=>` starts an expression function body."
    ],
    "help": [
        "Write `name (...) -> T` inside an `ffi` block."
    ]
}
