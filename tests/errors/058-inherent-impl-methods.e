Thing :: plex { value i32 }
impl Thing {
    bad :: 1
}
main :: fn () => 0
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `function method`, found `non-function binding`",
    "source_file": "tests/errors/058-inherent-impl-methods.e",
    "primary_location": {
        "line": 3,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 5,
            "length": 3,
            "message": "This expression has type `non-function binding`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
