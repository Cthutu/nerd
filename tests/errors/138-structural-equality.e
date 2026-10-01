Bad :: plex { resource arena }
main :: fn () { a: Bad b: Bad assert a == b }
¬
{
    "message": "Operator `==` requires matching operands that support Eq, found `Bad` and `Bad`",
    "source_file": "tests/errors/138-structural-equality.e",
    "primary_location": {
        "line": 2,
        "column": 40
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 40,
            "length": 2,
            "message": "These operands have types `Bad` and `Bad`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/138-structural-equality.e",
            "line": 1,
            "column": 1,
            "length": 3,
            "message": "Type `Bad` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Use `==` only with matching operands that support Eq."
    ]
}
¬
Bad :: enum { Empty Resource(arena) }
main :: fn () { a: Bad = Empty b: Bad = Empty assert a == b }
¬
{
    "message": "Operator `==` requires matching operands that support Eq, found `Bad` and `Bad`",
    "source_file": "tests/errors/138-structural-equality.e",
    "primary_location": {
        "line": 2,
        "column": 56
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 56,
            "length": 2,
            "message": "These operands have types `Bad` and `Bad`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/138-structural-equality.e",
            "line": 1,
            "column": 1,
            "length": 3,
            "message": "Type `Bad` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Use `==` only with matching operands that support Eq."
    ]
}
¬
Bad :: enum { Empty Callback(fn () -> i32) }
same :: fn [T] (a:T,b:T) -> bool where T: Eq { return a == b }
main :: fn () { a: Bad = Empty assert same(a,a) }
¬
{
    "message": "Type mismatch: expected `Eq implementation`, found `Bad`",
    "source_file": "tests/errors/138-structural-equality.e",
    "primary_location": {
        "line": 3,
        "column": 44
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 44,
            "length": 1,
            "message": "This expression has type `Bad`"
        },
        {
            "kind": "secondary",
            "source_file": "__REPO__/mods/core.n",
            "line": 204,
            "column": 5,
            "length": 2,
            "message": "Type `Eq` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/138-structural-equality.e",
            "line": 1,
            "column": 1,
            "length": 3,
            "message": "Type `Bad` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
