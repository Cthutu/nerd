Left :: plex { x i32 }
Right :: plex { x bool }
main :: fn () {
    a: Left
    _b: Right = a
}
¬
{
    "message": "Type mismatch: expected `Right`, found `Left`",
    "source_file": "tests/errors/140-type-definition-notes.e",
    "primary_location": {
        "line": 5,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 17,
            "length": 1,
            "message": "This expression has type `Left`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/140-type-definition-notes.e",
            "line": 2,
            "column": 1,
            "length": 5,
            "message": "Type `Right` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/140-type-definition-notes.e",
            "line": 1,
            "column": 1,
            "length": 4,
            "message": "Type `Left` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Item :: plex { x i32 }
main :: fn () {
    a: Item
    _sum := a + a
}
¬
{
    "message": "Operator `+` requires matching numeric operands, found `Item` and `Item`",
    "source_file": "tests/errors/140-type-definition-notes.e",
    "primary_location": {
        "line": 4,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 15,
            "length": 1,
            "message": "These operands have types `Item` and `Item`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/140-type-definition-notes.e",
            "line": 1,
            "column": 1,
            "length": 4,
            "message": "Type `Item` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Use `+` only with matching numeric operands."
    ]
}
¬
Left :: enum { One }
Right :: enum { Two }
main :: fn () {
    a: Left = One
    _b: Right = a
}
¬
{
    "message": "Type mismatch: expected `Right`, found `Left`",
    "source_file": "tests/errors/140-type-definition-notes.e",
    "primary_location": {
        "line": 5,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 17,
            "length": 1,
            "message": "This expression has type `Left`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/140-type-definition-notes.e",
            "line": 2,
            "column": 1,
            "length": 5,
            "message": "Type `Right` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/140-type-definition-notes.e",
            "line": 1,
            "column": 1,
            "length": 4,
            "message": "Type `Left` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Left :: plex { x i32 }
Right :: plex { y bool }
main :: fn () {
    a: Left
    _b: ^Right = ^a
}
¬
{
    "message": "Type mismatch: expected `^Right`, found `^Left`",
    "source_file": "tests/errors/140-type-definition-notes.e",
    "primary_location": {
        "line": 5,
        "column": 18
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 18,
            "length": 1,
            "message": "This expression has type `^Left`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/140-type-definition-notes.e",
            "line": 2,
            "column": 1,
            "length": 5,
            "message": "Type `Right` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/140-type-definition-notes.e",
            "line": 1,
            "column": 1,
            "length": 4,
            "message": "Type `Left` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
    _b: bool = 3
}
¬
{
    "message": "Type mismatch: expected `bool`, found `untyped integer`",
    "source_file": "tests/errors/140-type-definition-notes.e",
    "primary_location": {
        "line": 2,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
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
