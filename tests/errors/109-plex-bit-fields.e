Header :: plex {
    i8 {
        version : 4
    }
}
¬
{
    "message": "Type mismatch: expected `unsigned integer bit-field storage type`, found `i8`",
    "source_file": "tests/errors/109-plex-bit-fields.e",
    "primary_location": {
        "line": 2,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 5,
            "length": 2,
            "message": "This expression has type `i8`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
TokenType :: enum { Eof Data(u8) }

Header :: plex {
    u8 {
        kind TokenType : 4
    }
}
¬
{
    "message": "Type mismatch: expected `payload-free enum whose discriminants fit the bit width`, found `TokenType`",
    "source_file": "tests/errors/109-plex-bit-fields.e",
    "primary_location": {
        "line": 5,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 9,
            "length": 4,
            "message": "This expression has type `TokenType`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
TokenType :: enum { Eof Last = 16 }

Header :: plex {
    u8 {
        kind TokenType : 4
    }
}
¬
{
    "message": "Type mismatch: expected `payload-free enum whose discriminants fit the bit width`, found `TokenType`",
    "source_file": "tests/errors/109-plex-bit-fields.e",
    "primary_location": {
        "line": 5,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 9,
            "length": 4,
            "message": "This expression has type `TokenType`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
TokenType :: enum { Eof }

Header :: plex {
    u8 {
        _ TokenType : 4
        kind        : 4
    }
}
¬
{
    "message": "Type mismatch: expected `untyped bit-field padding`, found `typed padding`",
    "source_file": "tests/errors/109-plex-bit-fields.e",
    "primary_location": {
        "line": 5,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 9,
            "length": 1,
            "message": "This expression has type `typed padding`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Header :: plex {
    u8 {
        version : 0
    }
}
¬
{
    "message": "Type mismatch: expected `positive compile-time bit width within storage`, found `invalid or overflowing bit width`",
    "source_file": "tests/errors/109-plex-bit-fields.e",
    "primary_location": {
        "line": 3,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 9,
            "length": 7,
            "message": "This expression has type `invalid or overflowing bit width`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Header :: plex {
    u8 {
        version : 5
        kind    : 4
    }
}
¬
{
    "message": "Type mismatch: expected `positive compile-time bit width within storage`, found `invalid or overflowing bit width`",
    "source_file": "tests/errors/109-plex-bit-fields.e",
    "primary_location": {
        "line": 4,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 9,
            "length": 4,
            "message": "This expression has type `invalid or overflowing bit width`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Header :: plex {
    u8 {
        _ : 8
    }
}
¬
{
    "message": "Type mismatch: expected `bit-field block with a named field`, found `padding-only bit-field block`",
    "source_file": "tests/errors/109-plex-bit-fields.e",
    "primary_location": {
        "line": 2,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 5,
            "length": 2,
            "message": "This expression has type `padding-only bit-field block`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Header :: plex {
    u8 {
        version : 4
        kind    : 4
    }
}

main :: fn () {
    _ := Header { version: 16 kind: 0 }
}
¬
{
    "message": "Type mismatch: expected `value fitting 4-bit field`, found `out-of-range integer constant`",
    "source_file": "tests/errors/109-plex-bit-fields.e",
    "primary_location": {
        "line": 9,
        "column": 28
    },
    "references": [
        {
            "kind": "primary",
            "line": 9,
            "column": 28,
            "length": 2,
            "message": "This expression has type `out-of-range integer constant`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
TokenType :: enum { Eof Data }

Header :: plex {
    u8 {
        kind TokenType : 4
    }
}

main :: fn () {
    header := Header { kind: TokenType.Eof }
    header.kind = 1
}
¬
{
    "message": "Type mismatch: expected `TokenType`, found `untyped integer`",
    "source_file": "tests/errors/109-plex-bit-fields.e",
    "primary_location": {
        "line": 11,
        "column": 19
    },
    "references": [
        {
            "kind": "primary",
            "line": 11,
            "column": 19,
            "length": 1,
            "message": "This expression has type `untyped integer`"
        }
    ],
    "notes": [
        "`kind` is a bitfield of type `TokenType`."
    ],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Header :: plex {
    u8 {
        version : 4
        kind    : 4
    }
}

main :: fn () {
    header := Header { version: 0 kind: 0 }
    header.version = 16
}
¬
{
    "message": "Type mismatch: expected `value fitting 4-bit field`, found `out-of-range integer constant`",
    "source_file": "tests/errors/109-plex-bit-fields.e",
    "primary_location": {
        "line": 10,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 10,
            "column": 12,
            "length": 7,
            "message": "This expression has type `out-of-range integer constant`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Header :: plex {
    u8 {
        version : 4
        kind    : 4
    }
}

main :: fn () {
    header := Header { version: 0 kind: 0 }
    _ := ^header.version
}
¬
{
    "message": "Type mismatch: expected `addressable value`, found `u8`",
    "source_file": "tests/errors/109-plex-bit-fields.e",
    "primary_location": {
        "line": 10,
        "column": 18
    },
    "references": [
        {
            "kind": "primary",
            "line": 10,
            "column": 18,
            "length": 7,
            "message": "This expression has type `u8`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
