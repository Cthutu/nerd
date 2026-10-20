Problem :: plex { code i32 }
main :: fn () -> void\Problem { return }

¬
{
    "message": "Type mismatch: expected `Display implementation`, found `Problem`",
    "source_file": "tests/errors/153-main-results.e",
    "primary_location": {
        "line": 2,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 1,
            "length": 4,
            "message": "This expression has type `Problem`"
        },
        {
            "kind": "secondary",
            "source_file": "__REPO__/mods/core.n",
            "line": 198,
            "column": 5,
            "length": 7,
            "message": "Type `Display` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/153-main-results.e",
            "line": 1,
            "column": 1,
            "length": 7,
            "message": "Type `Problem` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
Problem :: plex { code i32 }
main :: fn () -> i32\Problem { return 0 }

¬
{
    "message": "Type mismatch: expected `Display implementation`, found `Problem`",
    "source_file": "tests/errors/153-main-results.e",
    "primary_location": {
        "line": 2,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 1,
            "length": 4,
            "message": "This expression has type `Problem`"
        },
        {
            "kind": "secondary",
            "source_file": "__REPO__/mods/core.n",
            "line": 198,
            "column": 5,
            "length": 7,
            "message": "Type `Display` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/153-main-results.e",
            "line": 1,
            "column": 1,
            "length": 7,
            "message": "Type `Problem` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
main :: fn () -> ?bool { return yes }

¬
{
    "message": "Invalid type for entry point `main`: found `fn () -> ?bool`",
    "source_file": "tests/errors/153-main-results.e",
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
            "message": "`main` must be a function with no parameters or one `[]string` parameter, returning an integer, no value, `?void`, `?i32`, or a result with `void`/`i32` success"
        }
    ],
    "notes": [],
    "help": [
        "Change `main` to `fn ()`, or to `fn (args: []string)` if the program needs command-line arguments."
    ]
}

¬
main :: fn () -> bool\string { return yes }

¬
{
    "message": "Invalid type for entry point `main`: found `fn () -> bool\\string`",
    "source_file": "tests/errors/153-main-results.e",
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
            "message": "`main` must be a function with no parameters or one `[]string` parameter, returning an integer, no value, `?void`, `?i32`, or a result with `void`/`i32` success"
        }
    ],
    "notes": [],
    "help": [
        "Change `main` to `fn ()`, or to `fn (args: []string)` if the program needs command-line arguments."
    ]
}

¬
main :: fn () -> i64\string { return 0 }

¬
{
    "message": "Invalid type for entry point `main`: found `fn () -> i64\\string`",
    "source_file": "tests/errors/153-main-results.e",
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
            "message": "`main` must be a function with no parameters or one `[]string` parameter, returning an integer, no value, `?void`, `?i32`, or a result with `void`/`i32` success"
        }
    ],
    "notes": [],
    "help": [
        "Change `main` to `fn ()`, or to `fn (args: []string)` if the program needs command-line arguments."
    ]
}

¬
main :: fn () -> ?i64 { return 0 }

¬
{
    "message": "Invalid type for entry point `main`: found `fn () -> ?i64`",
    "source_file": "tests/errors/153-main-results.e",
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
            "message": "`main` must be a function with no parameters or one `[]string` parameter, returning an integer, no value, `?void`, `?i32`, or a result with `void`/`i32` success"
        }
    ],
    "notes": [],
    "help": [
        "Change `main` to `fn ()`, or to `fn (args: []string)` if the program needs command-line arguments."
    ]
}

¬
main :: fn () -> void\void { return }

¬
{
    "message": "Type mismatch: expected `Display implementation`, found `void`",
    "source_file": "tests/errors/153-main-results.e",
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
            "message": "This expression has type `void`"
        },
        {
            "kind": "secondary",
            "source_file": "__REPO__/mods/core.n",
            "line": 198,
            "column": 5,
            "length": 7,
            "message": "Type `Display` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
main :: fn (_arg: i32) -> void\string { return }
¬
{
    "message": "Invalid type for entry point `main`: found `fn (i32) -> void\\string`",
    "source_file": "tests/errors/153-main-results.e",
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
            "message": "`main` must be a function with no parameters or one `[]string` parameter, returning an integer, no value, `?void`, `?i32`, or a result with `void`/`i32` success"
        }
    ],
    "notes": [],
    "help": [
        "Change `main` to `fn ()`, or to `fn (args: []string)` if the program needs command-line arguments."
    ]
}
