main :: fn () => mystery
¬
{
    "code": "0300",
    "message": "Unknown symbol `mystery`",
    "source_file": "tests/errors/004-semantics.e",
    "primary_location": {
        "line": 1,
        "column": 18
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 18,
            "length": 7,
            "message": "This symbol is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Add a binding for `mystery` or fix the spelling."
    ]
}
¬
answer :: 42
answer :: 7
¬
{
    "code": "0301",
    "message": "Duplicate binding for symbol `answer`",
    "source_file": "tests/errors/004-semantics.e",
    "primary_location": {
        "line": 2,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 1,
            "length": 6,
            "message": "This binding redefines `answer`"
        },
        {
            "kind": "secondary",
            "line": 1,
            "column": 1,
            "length": 6,
            "message": "Previous binding of `answer` is here"
        }
    ],
    "notes": [],
    "help": [
        "Rename one of the bindings or remove the duplicate definition."
    ]
}
¬
value: mystery: 1
¬
{
    "code": "0303",
    "message": "Unknown type `mystery`",
    "source_file": "tests/errors/004-semantics.e",
    "primary_location": {
        "line": 1,
        "column": 8
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 8,
            "length": 7,
            "message": "This type name is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Use one of the built-in primitive types supported by the current milestone."
    ]
}
¬
value: string: 1
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `string`, found `untyped integer`",
    "source_file": "tests/errors/004-semantics.e",
    "primary_location": {
        "line": 1,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 16,
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
value :: 1
main :: fn () {
    value = 2
    return value
}
¬
{
    "code": "0305",
    "message": "Cannot assign to `value`",
    "source_file": "tests/errors/004-semantics.e",
    "primary_location": {
        "line": 3,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 5,
            "length": 5,
            "message": "`value` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `value` as a variable with `:` or assign to a different mutable symbol."
    ]
}
¬
name: string
¬
{
    "code": "0306",
    "message": "Invalid variable type `string`",
    "source_file": "tests/errors/004-semantics.e",
    "primary_location": {
        "line": 1,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 1,
            "length": 4,
            "message": "This variable type is not supported yet"
        }
    ],
    "notes": [],
    "help": [
        "For this milestone, variables must use a concrete integer type."
    ]
}
