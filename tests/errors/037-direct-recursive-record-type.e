Node :: plex {
    next Node
}
¬
{
    "message": "Type alias cycle involving `Node`",
    "source_file": "tests/errors/037-direct-recursive-record-type.e",
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
            "message": "This alias participates in a type cycle"
        },
        {
            "kind": "secondary",
            "line": 1,
            "column": 1,
            "length": 4,
            "message": "Cycle closes through `Node` here"
        }
    ],
    "notes": [],
    "help": [
        "Break the cycle by rewriting one of the aliases so it resolves to a concrete type."
    ]
}
