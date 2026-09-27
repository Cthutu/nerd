Location :: plex {
    description string
    tag         string
}

Object :: plex {
    description string
    tag         string
    location    ^Location
}

objs: []Object = [
    { description: "x", tag: "y", location: ^objs[0] },
]
¬
{
    "message": "Type mismatch: expected `^Location`, found `^Object`",
    "source_file": "tests/errors/036-named-record-type-mismatch.e",
    "primary_location": {
        "line": 13,
        "column": 45
    },
    "references": [
        {
            "kind": "primary",
            "line": 13,
            "column": 45,
            "length": 1,
            "message": "This expression has type `^Object`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/036-named-record-type-mismatch.e",
            "line": 1,
            "column": 1,
            "length": 8,
            "message": "Type `Location` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/036-named-record-type-mismatch.e",
            "line": 6,
            "column": 1,
            "length": 6,
            "message": "Type `Object` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
