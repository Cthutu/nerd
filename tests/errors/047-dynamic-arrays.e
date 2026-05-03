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
¬
main :: fn () {
    names: [..]string
    names.pop("bad")
}
¬
{
    "code": "0313",
    "message": "Argument count mismatch: expected 0, found 1",
    "source_file": "tests/errors/047-dynamic-arrays.e",
    "primary_location": {
        "line": 3,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 14,
            "length": 1,
            "message": "This call uses the wrong arity"
        }
    ],
    "notes": [],
    "help": [
        "Pass exactly 0 arguments to match the function signature."
    ]
}
