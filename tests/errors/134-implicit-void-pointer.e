main :: fn () {
    opaque: ^void = nil
    _typed: ^i32 = opaque
}
¬
{
    "message": "Type mismatch: expected `^i32`, found `^void`",
    "source_file": "tests/errors/134-implicit-void-pointer.e",
    "primary_location": {
        "line": 3,
        "column": 20
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 20,
            "length": 6,
            "message": "This expression has type `^void`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
    value: i32 = 1
    _wrong: ^u32 = ^value
}
¬
{
    "message": "Type mismatch: expected `^u32`, found `^i32`",
    "source_file": "tests/errors/134-implicit-void-pointer.e",
    "primary_location": {
        "line": 3,
        "column": 20
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 20,
            "length": 1,
            "message": "This expression has type `^i32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
callback :: fn () {}
main :: fn () {
    _opaque: ^void = callback
}
¬
{
    "message": "Type mismatch: expected `^void`, found `fn () -> void`",
    "source_file": "tests/errors/134-implicit-void-pointer.e",
    "primary_location": {
        "line": 3,
        "column": 22
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 22,
            "length": 8,
            "message": "This expression has type `fn () -> void`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
    bytes: [2]u8 = [1, 2]
    _opaque: ^void = bytes[..]
}
¬
{
    "message": "Type mismatch: expected `^void`, found `[]u8`",
    "source_file": "tests/errors/134-implicit-void-pointer.e",
    "primary_location": {
        "line": 3,
        "column": 27
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 27,
            "length": 1,
            "message": "This expression has type `[]u8`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
    pointer: ^i32 = nil
    _wrong: ^^void = ^pointer
}
¬
{
    "message": "Type mismatch: expected `^^void`, found `^^i32`",
    "source_file": "tests/errors/134-implicit-void-pointer.e",
    "primary_location": {
        "line": 3,
        "column": 22
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 22,
            "length": 1,
            "message": "This expression has type `^^i32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
