a :: b
b :: a
¬
{
    "code": "0302",
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
