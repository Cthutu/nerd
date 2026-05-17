main :: () {
    return
}
¬
{
    "message": "Expected Keyword `fn` but found LeftParen `(`",
    "source_file": "tests/errors/065-missing-function-keyword.e",
    "primary_location": {
        "line": 1,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 9,
            "length": 1,
            "message": "Found LeftParen `(` here"
        }
    ],
    "notes": [
        "Function values after `::` start with `fn` before the parameter list."
    ],
    "help": [
        "Did you forget `fn` before the parameter list? Write `:: fn (...) { ... }`."
    ]
}
¬
main :: fn () {
    local := () {
        return
    }
}
¬
{
    "message": "Expected Keyword `fn` but found LeftParen `(`",
    "source_file": "tests/errors/065-missing-function-keyword.e",
    "primary_location": {
        "line": 2,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 14,
            "length": 1,
            "message": "Found LeftParen `(` here"
        }
    ],
    "notes": [
        "Function values after `:=` start with `fn` before the parameter list."
    ],
    "help": [
        "Did you forget `fn` before the parameter list? Write `:= fn (...) { ... }`."
    ]
}
¬
main :: f () {
    return
}
¬
{
    "message": "Expected Keyword `fn` but found Symbol",
    "source_file": "tests/errors/065-missing-function-keyword.e",
    "primary_location": {
        "line": 1,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 9,
            "length": 1,
            "message": "Found Symbol here"
        }
    ],
    "notes": [
        "Function values after `::` start with `fn` before the parameter list."
    ],
    "help": [
        "Did you forget `fn` before the parameter list? Write `:: fn (...) { ... }`."
    ]
}
