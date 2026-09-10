Thing :: plex { value i32 }
impl Thing { idle :: fn (self: ^Self) {} }
main :: fn () {}
¬
{
    "message": "Unused parameter `self`",
    "source_file": "tests/errors/113-unused-method-parameters.e",
    "primary_location": {
        "line": 2,
        "column": 26
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 26,
            "length": 4,
            "message": "This parameter is never read"
        }
    ],
    "notes": [
        "Assigning to a variable does not count as using it."
    ],
    "help": [
        "Remove `self` or prefix the name with `_` if it is deliberately unused."
    ]
}
¬
Render :: trait { render :: fn (self: ^Self) }
Thing :: plex { value i32 }
impl Render for Thing { render :: fn (self: ^Self) {} }
main :: fn () {}
¬
{
    "message": "Unused parameter `self`",
    "source_file": "tests/errors/113-unused-method-parameters.e",
    "primary_location": {
        "line": 3,
        "column": 39
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 39,
            "length": 4,
            "message": "This parameter is never read"
        }
    ],
    "notes": [
        "Assigning to a variable does not count as using it."
    ],
    "help": [
        "Remove `self` or prefix the name with `_` if it is deliberately unused."
    ]
}
¬
Thing :: plex { value i32 }
impl Thing { idle :: fn (receiver: ^Self) {} }
main :: fn () {}
¬
{
    "message": "Unused parameter `receiver`",
    "source_file": "tests/errors/113-unused-method-parameters.e",
    "primary_location": {
        "line": 2,
        "column": 26
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 26,
            "length": 8,
            "message": "This parameter is never read"
        }
    ],
    "notes": [
        "Assigning to a variable does not count as using it."
    ],
    "help": [
        "Remove `receiver` or prefix the name with `_` if it is deliberately unused."
    ]
}
¬
Thing :: plex { value i32 }
impl Thing { read :: fn (_self: ^Self) -> i32 { return _self.value } }
main :: fn () {}
¬
{
    "message": "Used parameter `_self` marked as unused",
    "source_file": "tests/errors/113-unused-method-parameters.e",
    "primary_location": {
        "line": 2,
        "column": 56
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 56,
            "length": 5,
            "message": "This read uses `_self`"
        },
        {
            "kind": "secondary",
            "line": 2,
            "column": 26,
            "length": 5,
            "message": "`_self` is marked unused by its leading `_`"
        }
    ],
    "notes": [
        "Leading `_` names are reserved for bindings that are deliberately unused."
    ],
    "help": [
        "Rename `_self` without the leading `_` now that it is used."
    ]
}
