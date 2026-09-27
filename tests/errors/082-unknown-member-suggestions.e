Counter :: plex {
    window_count i32
}

main :: fn () {
    c := Counter { window_count: 1 }
    _ := c.windows_count
}
¬
{
    "message": "Unknown member `windows_count` for `Counter`",
    "source_file": "tests/errors/082-unknown-member-suggestions.e",
    "primary_location": {
        "line": 7,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 7,
            "column": 12,
            "length": 13,
            "message": "`Counter` has no field or method named `windows_count`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/082-unknown-member-suggestions.e",
            "line": 1,
            "column": 1,
            "length": 7,
            "message": "Type `Counter` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Did you mean `.window_count`?"
    ]
}
¬
Point :: plex {
    x i32
}

impl Point {
    describe :: fn (self: Self) => $"{self.x}"
}

main :: fn () {
    p := Point { x: 1 }
    _ := p.describ()
}
¬
{
    "message": "Unknown member `describ` for `Point`",
    "source_file": "tests/errors/082-unknown-member-suggestions.e",
    "primary_location": {
        "line": 11,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 11,
            "column": 12,
            "length": 7,
            "message": "`Point` has no field or method named `describ`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/082-unknown-member-suggestions.e",
            "line": 1,
            "column": 1,
            "length": 5,
            "message": "Type `Point` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Did you mean `.describe`?"
    ]
}
