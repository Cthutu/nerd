main :: fn () {
    defer break
}
¬
{
    "code": "0328",
    "message": "`break` can only be used inside a loop or expression block",
    "source_file": "tests/errors/048-defer.e",
    "primary_location": {
        "line": 2,
        "column": 11
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 11,
            "length": 5,
            "message": "This `break` is not inside a `for` loop or expression block"
        }
    ],
    "notes": [],
    "help": [
        "Move `break` into a `for` loop or expression block."
    ]
}
¬
main :: fn () {
    defer continue
}
¬
{
    "code": "0328",
    "message": "`continue` can only be used inside a loop",
    "source_file": "tests/errors/048-defer.e",
    "primary_location": {
        "line": 2,
        "column": 11
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 11,
            "length": 8,
            "message": "This `continue` is not inside a `for` loop"
        }
    ],
    "notes": [],
    "help": [
        "Move `continue` into a `for` loop body."
    ]
}
