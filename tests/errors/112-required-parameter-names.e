bad :: fn (i32) {}
¬
{
    "message": "Expected declaration or expression but found Symbol",
    "source_file": "tests/errors/112-required-parameter-names.e",
    "primary_location": {
        "line": 1,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 12,
            "length": 3,
            "message": "Found Symbol here"
        }
    ],
    "notes": [],
    "help": [
        "Function parameters require names; write `name: Type`."
    ]
}
¬
Thing :: plex { value i32 }
impl Thing { bad :: fn (^Self) {} }
¬
{
    "message": "Expected declaration or expression but found Caret `^`",
    "source_file": "tests/errors/112-required-parameter-names.e",
    "primary_location": {
        "line": 2,
        "column": 25
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 25,
            "length": 1,
            "message": "Found Caret `^` here"
        }
    ],
    "notes": [],
    "help": [
        "Function parameters require names; write `name: Type`."
    ]
}
¬
Render :: trait { render :: fn (^Self) }
¬
{
    "message": "Expected declaration or expression but found Caret `^`",
    "source_file": "tests/errors/112-required-parameter-names.e",
    "primary_location": {
        "line": 1,
        "column": 33
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 33,
            "length": 1,
            "message": "Found Caret `^` here"
        }
    ],
    "notes": [],
    "help": [
        "Function parameters require names; write `name: Type`."
    ]
}
¬
Render :: trait { render :: fn (Self) }
¬
{
    "message": "Expected declaration or expression but found Symbol",
    "source_file": "tests/errors/112-required-parameter-names.e",
    "primary_location": {
        "line": 1,
        "column": 33
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 33,
            "length": 4,
            "message": "Found Symbol here"
        }
    ],
    "notes": [],
    "help": [
        "Function parameters require names; write `name: Type`."
    ]
}
¬
Render :: trait { render :: fn (self: Self, i32) }
¬
{
    "message": "Expected declaration or expression but found Symbol",
    "source_file": "tests/errors/112-required-parameter-names.e",
    "primary_location": {
        "line": 1,
        "column": 45
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 45,
            "length": 3,
            "message": "Found Symbol here"
        }
    ],
    "notes": [],
    "help": [
        "Function parameters require names; write `name: Type`."
    ]
}
¬
main :: fn () { _callback := fn (i32) {} }
¬
{
    "message": "Expected declaration or expression but found Symbol",
    "source_file": "tests/errors/112-required-parameter-names.e",
    "primary_location": {
        "line": 1,
        "column": 34
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 34,
            "length": 3,
            "message": "Found Symbol here"
        }
    ],
    "notes": [],
    "help": [
        "Function parameters require names; write `name: Type`."
    ]
}
¬
Functions :: plex #c { log fn (^u8, ^u8) }
¬
{
    "message": "Expected declaration or expression but found Caret `^`",
    "source_file": "tests/errors/112-required-parameter-names.e",
    "primary_location": {
        "line": 1,
        "column": 32
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 32,
            "length": 1,
            "message": "Found Caret `^` here"
        }
    ],
    "notes": [],
    "help": [
        "Function parameters require names; write `name: Type`."
    ]
}
¬
Callback :: fn (i32) -> i32
¬
{
    "message": "Expected declaration or expression but found Symbol",
    "source_file": "tests/errors/112-required-parameter-names.e",
    "primary_location": {
        "line": 1,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 17,
            "length": 3,
            "message": "Found Symbol here"
        }
    ],
    "notes": [],
    "help": [
        "Function parameters require names; write `name: Type`."
    ]
}
¬
Factory :: fn (value: i32) -> fn (^u8)
¬
{
    "message": "Expected declaration or expression but found Caret `^`",
    "source_file": "tests/errors/112-required-parameter-names.e",
    "primary_location": {
        "line": 1,
        "column": 35
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 35,
            "length": 1,
            "message": "Found Caret `^` here"
        }
    ],
    "notes": [],
    "help": [
        "Function parameters require names; write `name: Type`."
    ]
}
¬
apply :: fn (callback: fn (i32) -> i32) {}
¬
{
    "message": "Expected declaration or expression but found Symbol",
    "source_file": "tests/errors/112-required-parameter-names.e",
    "primary_location": {
        "line": 1,
        "column": 28
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 28,
            "length": 3,
            "message": "Found Symbol here"
        }
    ],
    "notes": [],
    "help": [
        "Function parameters require names; write `name: Type`."
    ]
}
¬
Functions :: plex { log fn (channel: ^u8, ^u8) }
¬
{
    "message": "Expected declaration or expression but found Caret `^`",
    "source_file": "tests/errors/112-required-parameter-names.e",
    "primary_location": {
        "line": 1,
        "column": 43
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 43,
            "length": 1,
            "message": "Found Caret `^` here"
        }
    ],
    "notes": [],
    "help": [
        "Function parameters require names; write `name: Type`."
    ]
}
¬
ffi "c" puts (^i8) -> i32
¬
{
    "message": "Expected declaration or expression but found Caret `^`",
    "source_file": "tests/errors/112-required-parameter-names.e",
    "primary_location": {
        "line": 1,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 15,
            "length": 1,
            "message": "Found Caret `^` here"
        }
    ],
    "notes": [],
    "help": [
        "Function parameters require names; write `name: Type`."
    ]
}
