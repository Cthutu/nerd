use inferred_fill
use structural_eq
main :: fn () {
    count := 2
    items: [count]Item
    items.fill_context(^{ missing: 1, ... })
}
¬
{
    "message": "Unknown field `missing` in plex literal of type `Item`",
    "source_file": "tests/errors/139-record-literal-expected-type.e",
    "primary_location": {
        "line": 6,
        "column": 27
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 27,
            "length": 7,
            "message": "The target plex type has no field named `missing`"
        }
    ],
    "notes": [],
    "help": [
        "Use a field declared by the target plex type."
    ]
}
¬
use inferred_fill
Record :: plex { value i32 }
main :: fn () {
    items: [2]Record
    fill(items, ^{ missing: 1, ... })
}
¬
{
    "message": "Unknown field `missing` in plex literal of type `Record`",
    "source_file": "tests/errors/139-record-literal-expected-type.e",
    "primary_location": {
        "line": 5,
        "column": 20
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 20,
            "length": 7,
            "message": "The target plex type has no field named `missing`"
        }
    ],
    "notes": [],
    "help": [
        "Use a field declared by the target plex type."
    ]
}
