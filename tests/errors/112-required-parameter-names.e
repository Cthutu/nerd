bad :: fn (i32) {}
¬
{
    "message": "Expected Colon `:` but found RightParen `)`",
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
            "message": "Found RightParen `)` here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}
¬
Thing :: plex { value i32 }
impl Thing { bad :: fn (^Self) {} }
¬
{
    "message": "Expected Symbol but found Caret `^`",
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
        "Check for a missing closing delimiter or misplaced operator"
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
        "Trait method parameters require names; write `name: Type`."
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
        "Trait method parameters require names; write `name: Type`."
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
        "Trait method parameters require names; write `name: Type`."
    ]
}
¬
main :: fn () { _callback := fn (i32) {} }
¬
{
    "message": "Expected Colon `:` but found RightParen `)`",
    "source_file": "tests/errors/112-required-parameter-names.e",
    "primary_location": {
        "line": 1,
        "column": 37
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 37,
            "length": 1,
            "message": "Found RightParen `)` here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}
