Point :: plex { x i32
 y i32 }
main :: fn () { p := Point { x: 1, y: 2 }
 on p { { x } => {} } }

¬
{
    "message": "Plex pattern is missing fields",
    "source_file": "tests/errors/152-plex-pattern-fields.e",
    "primary_location": {
        "line": 4,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 9,
            "length": 1,
            "message": "This pattern does not mention every field"
        }
    ],
    "notes": [
        "Missing fields: `y`"
    ],
    "help": [
        "Match or bind each field, use `field: _` to ignore a field, or add trailing `...` to ignore all remaining fields."
    ]
}

¬
Point :: plex { x i32
 y i32 }
main :: fn () { p := Point { x: 1, y: 2 }
 on p { {} => {} } }

¬
{
    "message": "Plex pattern is missing fields",
    "source_file": "tests/errors/152-plex-pattern-fields.e",
    "primary_location": {
        "line": 4,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 9,
            "length": 1,
            "message": "This pattern does not mention every field"
        }
    ],
    "notes": [
        "Missing fields: `x`, `y`"
    ],
    "help": [
        "Match or bind each field, use `field: _` to ignore a field, or add trailing `...` to ignore all remaining fields."
    ]
}

¬
Point :: plex { x i32
 y i32 }
main :: fn () { p := Point { x: 1, y: 2 }
 on p { { x: _, x: _, ... } => {} } }

¬
{
    "message": "Type mismatch: expected `unique plex pattern field`, found `x`",
    "source_file": "tests/errors/152-plex-pattern-fields.e",
    "primary_location": {
        "line": 4,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 17,
            "length": 1,
            "message": "This expression has type `x`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
Point :: plex { x i32
 y i32 }
main :: fn () { p := Point { x: 1, y: 2 }
 on p { { missing: _, ... } => {} } }

¬
{
    "message": "Unknown field `missing` in plex pattern",
    "source_file": "tests/errors/152-plex-pattern-fields.e",
    "primary_location": {
        "line": 4,
        "column": 11
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 11,
            "length": 7,
            "message": "No field named `missing` exists on the matched plex type"
        }
    ],
    "notes": [],
    "help": [
        "If you meant to bind an existing field to a differently named local, write `field: binding`. Pattern shorthand only works when the field and binding have the same name."
    ]
}

¬
Point :: plex { x i32
 y i32 }
main :: fn () { p := Point { x: 1, y: 2 }
 on p { { ..., x } => {} } }

¬
{
    "message": "Expected RightBrace `}` but found Symbol",
    "source_file": "tests/errors/152-plex-pattern-fields.e",
    "primary_location": {
        "line": 4,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 16,
            "length": 1,
            "message": "Found Symbol here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}

¬
Point :: plex { x i32
 y i32 }
main :: fn () { p := Point { x: 1, y: 2 }
 on p { { ..., ... } => {} } }

¬
{
    "message": "Expected RightBrace `}` but found Ellipsis `...`",
    "source_file": "tests/errors/152-plex-pattern-fields.e",
    "primary_location": {
        "line": 4,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 16,
            "length": 3,
            "message": "Found Ellipsis `...` here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}

¬
Point :: plex { x i32
 y i32 }
Outer :: plex { p Point }
main :: fn () { p := Outer { p: Point { x: 1, y: 2 } }
 on p { { p: { x } } => {} } }

¬
{
    "message": "Plex pattern is missing fields",
    "source_file": "tests/errors/152-plex-pattern-fields.e",
    "primary_location": {
        "line": 5,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 14,
            "length": 1,
            "message": "This pattern does not mention every field"
        }
    ],
    "notes": [
        "Missing fields: `y`"
    ],
    "help": [
        "Match or bind each field, use `field: _` to ignore a field, or add trailing `...` to ignore all remaining fields."
    ]
}

¬
Point :: plex { x i32
 y i32 }
main :: fn () { p := Point { ... }
 on p { { x: _ } => {} } }

¬
{
    "message": "Plex pattern is missing fields",
    "source_file": "tests/errors/152-plex-pattern-fields.e",
    "primary_location": {
        "line": 4,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 9,
            "length": 1,
            "message": "This pattern does not mention every field"
        }
    ],
    "notes": [
        "Missing fields: `y`"
    ],
    "help": [
        "Match or bind each field, use `field: _` to ignore a field, or add trailing `...` to ignore all remaining fields."
    ]
}
¬
Point :: plex { x i32
 y i32 }
Event :: enum { Moved(Point) }
main :: fn () { e := Event.Moved(Point { x: 1, y: 2 })
 on e { Moved { x } => {} } }

¬
{
    "message": "Plex pattern is missing fields",
    "source_file": "tests/errors/152-plex-pattern-fields.e",
    "primary_location": {
        "line": 5,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 15,
            "length": 1,
            "message": "This pattern does not mention every field"
        }
    ],
    "notes": [
        "Missing fields: `y`"
    ],
    "help": [
        "Match or bind each field, use `field: _` to ignore a field, or add trailing `...` to ignore all remaining fields."
    ]
}
