main :: fn () {
    names: [..]string
    names.push(123)
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `string`, found `untyped integer`",
    "source_file": "tests/errors/047-dynamic-arrays.e",
    "primary_location": {
        "line": 3,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 16,
            "length": 3,
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
    names: [..]string
    names.append(123)
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `[]string`, found `untyped integer`",
    "source_file": "tests/errors/047-dynamic-arrays.e",
    "primary_location": {
        "line": 3,
        "column": 18
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 18,
            "length": 3,
            "message": "This expression has type `untyped integer`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
