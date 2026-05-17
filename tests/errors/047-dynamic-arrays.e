main :: fn () {
    names: [..]string
    names.push(123)
}
¬
{
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
¬
main :: fn () {
    names: [..]string
    names.delete("bad")
}
¬
{
    "message": "Type mismatch: expected `usize`, found `string`",
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
            "length": 5,
            "message": "This expression has type `string`"
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
    names.swap_delete()
}
¬
{
    "message": "Argument count mismatch: expected 1, found 0",
    "source_file": "tests/errors/047-dynamic-arrays.e",
    "primary_location": {
        "line": 3,
        "column": 22
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 22,
            "length": 1,
            "message": "This call uses the wrong arity"
        }
    ],
    "notes": [],
    "help": [
        "Pass exactly 1 argument to match the function signature."
    ]
}
