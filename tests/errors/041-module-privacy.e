box :: use test.reexport

main :: fn() {
    box.secret()
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `known module export`, found `secret`",
    "source_file": "tests/errors/041-module-privacy.e",
    "primary_location": {
        "line": 4,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 9,
            "length": 6,
            "message": "This expression has type `secret`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
forty_two :: 0
use test.reexport

main :: fn() => forty_two
¬
{
    "code": "0301",
    "message": "Duplicate binding for symbol `forty_two`",
    "source_file": "tests/errors/041-module-privacy.e",
    "primary_location": {
        "line": 2,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 1,
            "length": 3,
            "message": "This binding redefines `forty_two`"
        },
        {
            "kind": "secondary",
            "line": 1,
            "column": 1,
            "length": 9,
            "message": "Previous binding of `forty_two` is here"
        }
    ],
    "notes": [],
    "help": [
        "Rename one of the bindings or remove the duplicate definition."
    ]
}
