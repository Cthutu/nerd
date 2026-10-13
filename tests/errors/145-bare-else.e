main :: fn () { value := on 7 else { return } }
¬
{
    "message": "Unexpected Keyword `else`",
    "source_file": "tests/errors/145-bare-else.e",
    "primary_location": {
        "line": 1,
        "column": 31
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 31,
            "length": 4,
            "message": "Keyword `else` cannot appear here"
        }
    ],
    "notes": [
        "The omitted-success `on ... else` forms have been removed"
    ],
    "help": [
        "Use `expression else [error] { ... }`, or keep `on expression => [value] { ... } else [error] { ... }` for full extraction"
    ]
}
¬
work :: fn () -> i32\string { return 7 }
main :: fn () { value := on work() else? {} }
¬
{
    "message": "Unexpected Keyword `else`",
    "source_file": "tests/errors/145-bare-else.e",
    "primary_location": {
        "line": 2,
        "column": 36
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 36,
            "length": 4,
            "message": "Keyword `else` cannot appear here"
        }
    ],
    "notes": [
        "The omitted-success `on ... else` forms have been removed"
    ],
    "help": [
        "Use `expression else [error] { ... }`, or keep `on expression => [value] { ... } else [error] { ... }` for full extraction"
    ]
}
¬
work :: fn () -> i32\string { return 7 }
main :: fn () { value := work() else? {} }
¬
{
    "message": "Expected LeftBrace `{` but found Question `?`",
    "source_file": "tests/errors/145-bare-else.e",
    "primary_location": {
        "line": 2,
        "column": 37
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 37,
            "length": 1,
            "message": "Found Question `?` here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}
¬
main :: fn () { value := 7 else {} }
¬
{
    "message": "Type mismatch: expected `optional or result value`, found `untyped integer`",
    "source_file": "tests/errors/145-bare-else.e",
    "primary_location": {
        "line": 1,
        "column": 26
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 26,
            "length": 1,
            "message": "This expression has type `untyped integer`"
        }
    ],
    "notes": [],
    "help": [
        "The subject before `else` must have type `?T` or `T\\E`."
    ]
}
¬
main :: fn () {
    value : ?i32
    found := value else [error] {}
}
¬
{
    "message": "Type mismatch: expected `optional else branch without a payload binder`, found `payload binder`",
    "source_file": "tests/errors/145-bare-else.e",
    "primary_location": {
        "line": 3,
        "column": 20
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 20,
            "length": 4,
            "message": "This expression has type `payload binder`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
work :: fn () -> i32\string { return 7 }
main :: fn () -> i32 { value := work() else {} return value }
¬
{
    "message": "Cannot propagate `string` from a function returning `i32`",
    "source_file": "tests/errors/145-bare-else.e",
    "primary_location": {
        "line": 2,
        "column": 40
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 40,
            "length": 4,
            "message": "This `?` can return `string` to the caller"
        }
    ],
    "notes": [
        "The enclosing function has no compatible failure channel"
    ],
    "help": [
        "Handle the failure here, or change the function return type to `i32\\string`"
    ]
}
¬
work :: fn () -> ?i32 { return 7 }
main :: fn () -> i32 { value := work() else {} return value }
¬
{
    "message": "Cannot propagate `absence` from a function returning `i32`",
    "source_file": "tests/errors/145-bare-else.e",
    "primary_location": {
        "line": 2,
        "column": 40
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 40,
            "length": 4,
            "message": "This `?` can return `absence` to the caller"
        }
    ],
    "notes": [
        "The enclosing function has no compatible failure channel"
    ],
    "help": [
        "Handle the failure here, or change the function return type to `?i32`"
    ]
}
¬
work :: fn () -> i32\string { return 7 }
main :: fn () { value := work() else ${ break 42 } }
¬
{
    "message": "Expected LeftBrace `{` but found Dollar `$`",
    "source_file": "tests/errors/145-bare-else.e",
    "primary_location": {
        "line": 2,
        "column": 38
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 38,
            "length": 1,
            "message": "Found Dollar `$` here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}
¬
work :: fn () -> ?i32 { return 7 }
main :: fn () { value := work() else return }
¬
{
    "message": "Expected LeftBrace `{` but found Keyword `return`",
    "source_file": "tests/errors/145-bare-else.e",
    "primary_location": {
        "line": 2,
        "column": 38
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 38,
            "length": 6,
            "message": "Found Keyword `return` here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}
