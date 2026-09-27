main :: fn () { undo prn("invalid") }
¬
{
    "message": "Invalid `undo` statement",
    "source_file": "tests/errors/135-undo.e",
    "primary_location": {
        "line": 1,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 17,
            "length": 4,
            "message": "The enclosing function must return an optional or result"
        }
    ],
    "notes": [],
    "help": [
        "Use `undo` inside an optional- or result-returning function, with cleanup that does not return or propagate failure."
    ]
}
¬
main :: fn () -> i32 { undo prn("invalid")
return 0 }
¬
{
    "message": "Invalid `undo` statement",
    "source_file": "tests/errors/135-undo.e",
    "primary_location": {
        "line": 1,
        "column": 24
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 24,
            "length": 4,
            "message": "The enclosing function must return an optional or result"
        }
    ],
    "notes": [],
    "help": [
        "Use `undo` inside an optional- or result-returning function, with cleanup that does not return or propagate failure."
    ]
}
¬
main :: fn () -> ?void { undo return }
¬
{
    "message": "Invalid `undo` statement",
    "source_file": "tests/errors/135-undo.e",
    "primary_location": {
        "line": 1,
        "column": 31
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 31,
            "length": 6,
            "message": "An undo action cannot return, propagate failure, or register another undo"
        }
    ],
    "notes": [],
    "help": [
        "Use `undo` inside an optional- or result-returning function, with cleanup that does not return or propagate failure."
    ]
}
¬
fail :: fn () -> ?void { return nil }
main :: fn () -> ?void { undo fail()? }
¬
{
    "message": "Invalid `undo` statement",
    "source_file": "tests/errors/135-undo.e",
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
            "message": "An undo action cannot return, propagate failure, or register another undo"
        }
    ],
    "notes": [],
    "help": [
        "Use `undo` inside an optional- or result-returning function, with cleanup that does not return or propagate failure."
    ]
}
¬
main :: fn () -> ?void { undo { undo prn("nested") } }
¬
{
    "message": "Invalid `undo` statement",
    "source_file": "tests/errors/135-undo.e",
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
            "message": "An undo action cannot return, propagate failure, or register another undo"
        }
    ],
    "notes": [],
    "help": [
        "Use `undo` inside an optional- or result-returning function, with cleanup that does not return or propagate failure."
    ]
}
¬
main :: fn () -> ?void { defer { undo prn("late") } }
¬
{
    "message": "Invalid `undo` statement",
    "source_file": "tests/errors/135-undo.e",
    "primary_location": {
        "line": 1,
        "column": 34
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 34,
            "length": 4,
            "message": "An undo action cannot be registered from another cleanup action"
        }
    ],
    "notes": [],
    "help": [
        "Use `undo` inside an optional- or result-returning function, with cleanup that does not return or propagate failure."
    ]
}
¬
main :: fn () -> ?void { for yes { undo break
break } }
¬
{
    "message": "`break` can only be used inside a loop or expression block",
    "source_file": "tests/errors/135-undo.e",
    "primary_location": {
        "line": 1,
        "column": 41
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 41,
            "length": 5,
            "message": "This `break` is not inside a `for` loop or expression block"
        }
    ],
    "notes": [],
    "help": [
        "Move `break` into a `for` loop or expression block."
    ]
}
¬
main :: fn () -> ?void { for yes { undo again
break } }
¬
{
    "message": "`again` can only be used inside a loop",
    "source_file": "tests/errors/135-undo.e",
    "primary_location": {
        "line": 1,
        "column": 41
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 41,
            "length": 5,
            "message": "This `again` is not inside a `for` loop"
        }
    ],
    "notes": [],
    "help": [
        "Move `again` into a `for` loop body."
    ]
}
¬
main :: fn () -> ?i32 { return nil }
¬
{
    "message": "Invalid type for entry point `main`: found `fn () -> ?i32`",
    "source_file": "tests/errors/135-undo.e",
    "primary_location": {
        "line": 1,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 1,
            "length": 4,
            "message": "`main` must be a function with no parameters or one `[]string` parameter, returning an integer, `?void`, or no value"
        }
    ],
    "notes": [],
    "help": [
        "Change `main` to `fn ()`, or to `fn (args: []string)` if the program needs command-line arguments."
    ]
}
