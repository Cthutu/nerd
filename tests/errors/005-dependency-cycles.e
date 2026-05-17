a :: b + 1
b :: a + 1
¬
{
    "message": "Dependency cycle involving `b`",
    "source_file": "tests/errors/005-dependency-cycles.e",
    "primary_location": {
        "line": 2,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 1,
            "length": 1,
            "message": "This binding participates in a dependency cycle"
        },
        {
            "kind": "secondary",
            "line": 1,
            "column": 1,
            "length": 1,
            "message": "Cycle closes through `a` here"
        }
    ],
    "notes": [],
    "help": [
        "Break the cycle by rewriting one of the bindings so it no longer depends on the other."
    ]
}
¬
Price :: Cost
Cost :: Price
¬
{
    "message": "Type alias cycle involving `Cost`",
    "source_file": "tests/errors/005-dependency-cycles.e",
    "primary_location": {
        "line": 2,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 1,
            "length": 4,
            "message": "This alias participates in a type cycle"
        },
        {
            "kind": "secondary",
            "line": 1,
            "column": 1,
            "length": 5,
            "message": "Cycle closes through `Price` here"
        }
    ],
    "notes": [],
    "help": [
        "Break the cycle by rewriting one of the aliases so it resolves to a concrete type."
    ]
}
