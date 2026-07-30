pub BLACK :: i32 = 0xff000000

main :: fn () {}
¬
{
    "message": "Invalid typed variable declaration",
    "source_file": "tests/errors/092-typed-variable-double-colon.e",
    "primary_location": {
        "line": 1,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 14,
            "length": 3,
            "message": "`i32` is a type, not an assignment target"
        }
    ],
    "notes": [
        "`::` starts a constant binding, so the following `=` is parsed as an assignment"
    ],
    "help": [
        "Use `name : i32 = value` for a typed variable declaration"
    ]
}
