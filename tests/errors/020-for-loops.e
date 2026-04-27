use std.io

main :: fn () {
    for 1 {
        prn("bad")
    }
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `bool`, found `untyped integer`",
    "source_file": "tests/errors/020-for-loops.e",
    "primary_location": {
        "line": 4,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 9,
            "length": 1,
            "message": "This expression has type `untyped integer`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
use std.io

main :: fn () {
    for i := 0; i < 1; i += 1 {
    }
    prn(i)
}
¬
{
    "code": "0300",
    "message": "Unknown symbol `i`",
    "source_file": "tests/errors/020-for-loops.e",
    "primary_location": {
        "line": 6,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 9,
            "length": 1,
            "message": "This symbol is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Add a binding for `i` or fix the spelling."
    ]
}
¬
main :: fn () {
    break
}
¬
{
    "code": "0328",
    "message": "`break` can only be used inside a loop or expression block",
    "source_file": "tests/errors/020-for-loops.e",
    "primary_location": {
        "line": 2,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 5,
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
main :: fn () {
    text : []u8 = "a"[..]
    for ^c in text {
        c^ := c^
    }
}
¬
{
    "code": "0206",
    "message": "Invalid binding target before `:=`",
    "source_file": "tests/errors/020-for-loops.e",
    "primary_location": {
        "line": 4,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 12,
            "length": 1,
            "message": "`:=` starts a new binding here"
        }
    ],
    "notes": [
        "Bindings started with `:=` must begin with a symbol or supported destructuring pattern"
    ],
    "help": [
        "Use `=` to assign to an existing value such as a dereference or field access"
    ]
}
¬
main :: fn () {
    for {
        for on yes => break {
        }
    }
}
¬
{
    "code": "0203",
    "message": "Expected LeftBrace `{` but found RightBrace `}`",
    "source_file": "tests/errors/020-for-loops.e",
    "primary_location": {
        "line": 5,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 5,
            "length": 1,
            "message": "Found RightBrace `}` here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}
¬
main :: fn () {
    continue
}
¬
{
    "code": "0328",
    "message": "`continue` can only be used inside a loop",
    "source_file": "tests/errors/020-for-loops.e",
    "primary_location": {
        "line": 2,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 5,
            "length": 8,
            "message": "This `continue` is not inside a `for` loop"
        }
    ],
    "notes": [],
    "help": [
        "Move `continue` into a `for` loop body."
    ]
}
¬
main :: fn () {
    for {
        continue $missing
    }
}
¬
{
    "code": "0330",
    "message": "Unknown control label `$missing`",
    "source_file": "tests/errors/020-for-loops.e",
    "primary_location": {
        "line": 3,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 9,
            "length": 8,
            "message": "No enclosing expression block or loop has this label"
        }
    ],
    "notes": [],
    "help": [
        "Use the label from an enclosing `$label { ... }` block or `for ... $label { ... }` loop."
    ]
}
¬
main :: fn () {
    $block {
        for {
            continue $block
        }
    }
}
¬
{
    "code": "0331",
    "message": "`continue` label `$block` does not name a loop",
    "source_file": "tests/errors/020-for-loops.e",
    "primary_location": {
        "line": 4,
        "column": 13
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 13,
            "length": 8,
            "message": "This label names an expression block, not a `for` loop"
        }
    ],
    "notes": [],
    "help": [
        "Use `break $block` for an expression block, or `continue` to a `for ... $label { ... }` loop."
    ]
}
¬
main :: fn () {
    for on yes => break {
    }
}
¬
{
    "code": "0203",
    "message": "Expected LeftBrace `{` but found RightBrace `}`",
    "source_file": "tests/errors/020-for-loops.e",
    "primary_location": {
        "line": 4,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 1,
            "length": 1,
            "message": "Found RightBrace `}` here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}
