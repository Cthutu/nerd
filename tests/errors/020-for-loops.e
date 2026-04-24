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
¬
main :: fn () {
    break
}
¬
{
    "code": "0328",
    "message": "`break` can only be used inside a loop",
    "source_file": "tests/errors/020-for-loops.e",
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
            "message": "This `break` is not inside a `for` loop"
        }
    ],
    "notes": [],
    "help": [
        "Move `break` into a `for` loop body."
    ]
}
¬
main :: fn () {
    continue
}
¬
{
    "code": "0328",
    "message": "`continue` can only be used inside a loop",
    "source_file": "tests/errors/020-for-loops.e",
    "primary_location": {
        "line": 2,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 5,
            "length": 8,
            "message": "This `continue` is not inside a `for` loop"
        }
    ],
    "notes": [],
    "help": [
        "Move `continue` into a `for` loop body."
    ]
}
