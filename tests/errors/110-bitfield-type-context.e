Token :: plex {
    u32 {
        offset : 24
        _      : 8
    }
}

main :: fn () {
    start := 0.as(usize)
    _ := Token { offset: start }
}
¬
{
    "message": "Type mismatch: expected `u32`, found `usize`",
    "source_file": "tests/errors/110-bitfield-type-context.e",
    "primary_location": {
        "line": 10,
        "column": 26
    },
    "references": [
        {
            "kind": "primary",
            "line": 10,
            "column": 26,
            "length": 5,
            "message": "This expression has type `usize`"
        }
    ],
    "notes": [
        "`offset` is a bitfield of type `u32`."
    ],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Token :: plex {
    u32 {
        offset : 24
        _      : 8
    }
}

main :: fn () {
    start := 0.as(usize)
    token := Token { offset: 0 }
    token.offset = start
}
¬
{
    "message": "Type mismatch: expected `u32`, found `usize`",
    "source_file": "tests/errors/110-bitfield-type-context.e",
    "primary_location": {
        "line": 11,
        "column": 20
    },
    "references": [
        {
            "kind": "primary",
            "line": 11,
            "column": 20,
            "length": 5,
            "message": "This expression has type `usize`"
        }
    ],
    "notes": [
        "`offset` is a bitfield of type `u32`."
    ],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
