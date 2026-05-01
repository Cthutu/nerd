bad : fn() -> i32 {
    return 1
}
¬
{
    "code": "0203",
    "message": "Expected Equal `=` but found LeftBrace `{`",
    "source_file": "tests/errors/055-function-definition-type-annotation.e",
    "primary_location": {
        "line": 1,
        "column": 19
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 19,
            "length": 1,
            "message": "Found LeftBrace `{` here"
        }
    ],
    "notes": [
        "A function type annotation cannot include a function body."
    ],
    "help": [
        "Function definitions use `::`; did you mean to write `::` instead of `:`?"
    ]
}
¬
bad : fn() -> i32 => 1
¬
{
    "code": "0203",
    "message": "Expected Equal `=` but found FatArrow `=>`",
    "source_file": "tests/errors/055-function-definition-type-annotation.e",
    "primary_location": {
        "line": 1,
        "column": 19
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 19,
            "length": 2,
            "message": "Found FatArrow `=>` here"
        }
    ],
    "notes": [
        "A function type annotation cannot include a function body."
    ],
    "help": [
        "Function definitions use `::`; did you mean to write `::` instead of `:`?"
    ]
}
