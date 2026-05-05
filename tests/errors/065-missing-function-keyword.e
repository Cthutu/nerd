main :: f () {
    return
}
¬
{
    "code": "0203",
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
        "Function declarations after `::` start with `fn`; a bare symbol starts an expression instead."
    ],
    "help": [
        "Write `fn` before the parameter list, such as `name :: fn (...) { ... }`."
    ]
}
