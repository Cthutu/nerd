A :: plex { id u32 }
B :: plex { name string }
take_a :: fn (_items: []A) {}
take_b :: fn (_items: []B) {}
main :: fn () {
    items := [{ id: 42 }]
    take_a(items)
    take_b(items)
}
¬
{
    "message": "Type mismatch: expected `[1]A`, found `[1]B`",
    "source_file": "tests/errors/contextual-array-inference.e",
    "primary_location": {
        "line": 6,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 14,
            "length": 1,
            "message": "This expression has type `[1]B`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/contextual-array-inference.e",
            "line": 1,
            "column": 1,
            "length": 1,
            "message": "Type `A` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/contextual-array-inference.e",
            "line": 2,
            "column": 1,
            "length": 1,
            "message": "Type `B` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
A :: plex { id u32 }
B :: plex { name string }
take :: fn (_items: []B) {}
main :: fn () { items: [1]A = [{ id: 42 }]
    take(items)
}
¬
{
    "message": "Type mismatch: expected `[]B`, found `[1]A`",
    "source_file": "tests/errors/contextual-array-inference.e",
    "primary_location": {
        "line": 5,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 10,
            "length": 5,
            "message": "This expression has type `[1]A`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/contextual-array-inference.e",
            "line": 2,
            "column": 1,
            "length": 1,
            "message": "Type `B` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/contextual-array-inference.e",
            "line": 1,
            "column": 1,
            "length": 1,
            "message": "Type `A` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
take :: fn (_items: []u8) {}
main :: fn () { items := [256]
    take(items)
}
¬
{
    "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
    "source_file": "tests/errors/contextual-array-inference.e",
    "primary_location": {
        "line": 2,
        "column": 27
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 27,
            "length": 3,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
A :: plex { id u32 }
take :: fn (_items: [2]A) {}
main :: fn () { items := [{ id: 42 }]
    take(items)
}
¬
{
    "message": "Type mismatch: expected `[2]A`, found `[1]A`",
    "source_file": "tests/errors/contextual-array-inference.e",
    "primary_location": {
        "line": 4,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 10,
            "length": 5,
            "message": "This expression has type `[1]A`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/contextual-array-inference.e",
            "line": 1,
            "column": 1,
            "length": 1,
            "message": "Type `A` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
take :: fn (_items: []u8) {}
main :: fn () { items := [42.as(u64)]
    take(items)
}
¬
{
    "message": "Type mismatch: expected `u8`, found `u64`",
    "source_file": "tests/errors/contextual-array-inference.e",
    "primary_location": {
        "line": 2,
        "column": 29
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 29,
            "length": 1,
            "message": "This expression has type `u64`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
A :: plex { id u32 }
B :: plex { name string }
take :: fn (_items: []A) {}
main :: fn () { items := [B { name: "other" }]
    take(items)
}
¬
{
    "message": "Type mismatch: expected `A`, found `B`",
    "source_file": "tests/errors/contextual-array-inference.e",
    "primary_location": {
        "line": 4,
        "column": 29
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 29,
            "length": 1,
            "message": "This expression has type `B`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/contextual-array-inference.e",
            "line": 1,
            "column": 1,
            "length": 1,
            "message": "Type `A` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/contextual-array-inference.e",
            "line": 2,
            "column": 1,
            "length": 1,
            "message": "Type `B` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}