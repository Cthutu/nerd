main :: fn () {
    for 1 {
        prn("bad")
    }
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `bool`, found `untyped integer`",
    "source_file": "tests/errors/020-for-loops.e",
    "primary_location": {
        "line": 2,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 9,
            "length": 1,
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
    for i := 0; i < 1; i += 1 {
    }
    prn(i)
}
¬
{
    "code": "0300",
    "message": "Unknown symbol `i`",
    "source_file": "tests/errors/020-for-loops.e",
    "primary_location": {
        "line": 4,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 9,
            "length": 1,
            "message": "This symbol is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Add a binding for `i` or fix the spelling."
    ]
}
