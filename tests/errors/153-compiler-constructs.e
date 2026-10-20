main :: fn () {
    pair :: (1, 2)
    pair.0 = 3
}
¬
{
    "message": "Cannot assign to tuple field rooted in immutable `pair`",
    "source_file": "tests/errors/153-compiler-constructs.e",
    "primary_location": {
        "line": 3,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 10,
            "length": 1,
            "message": "This tuple field is part of immutable storage"
        }
    ],
    "notes": [],
    "help": [
        "Use a mutable binding or pass a pointer to mutable tuple storage."
    ]
}
¬
mutate :: fn (pair: (i32, i32)) { pair.0 += 1 }
main :: fn () { mutate((1, 2)) }
¬
{
    "message": "Cannot assign to tuple field rooted in immutable `pair`",
    "source_file": "tests/errors/153-compiler-constructs.e",
    "primary_location": {
        "line": 1,
        "column": 40
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 40,
            "length": 1,
            "message": "This tuple field is part of immutable storage"
        }
    ],
    "notes": [],
    "help": [
        "Use a mutable binding or pass a pointer to mutable tuple storage."
    ]
}
¬
pair :: fn [A, B] (a: A, b: B) -> A { _ := b
 return a }
main :: fn () { _ := pair[i32](1, 2) }
¬
{
    "message": "Argument count mismatch: expected 2, found 1",
    "source_file": "tests/errors/153-compiler-constructs.e",
    "primary_location": {
        "line": 1,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 9,
            "length": 2,
            "message": "This call uses the wrong arity"
        }
    ],
    "notes": [],
    "help": [
        "Pass exactly 2 arguments to match the function signature."
    ]
}
¬
pair :: fn [A, B] (a: A, b: B) -> A { _ := b
 return a }
main :: fn () { _ := pair[i32,](1, 2) }
¬
{
    "message": "Missing value before RightBracket `]`",
    "source_file": "tests/errors/153-compiler-constructs.e",
    "primary_location": {
        "line": 3,
        "column": 31
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 31,
            "length": 1,
            "message": "RightBracket `]` cannot appear here"
        }
    ],
    "notes": [],
    "help": [
        "Insert a literal, parenthesized expression, or unary operator"
    ]
}
¬
main :: fn () { values := [1, 2]
 _ := values[0, 1] }
¬
{
    "message": "Type mismatch: expected `integer index`, found `(i32, i32)`",
    "source_file": "tests/errors/153-compiler-constructs.e",
    "primary_location": {
        "line": 2,
        "column": 13
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 13,
            "length": 1,
            "message": "This expression has type `(i32, i32)`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Cell :: plex [T] { value T }
main :: fn () { _ := Cell[i32, string].init(1) }
¬
{
    "message": "Argument count mismatch: expected 1, found 2",
    "source_file": "tests/errors/153-compiler-constructs.e",
    "primary_location": {
        "line": 2,
        "column": 26
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 26,
            "length": 1,
            "message": "This call uses the wrong arity"
        }
    ],
    "notes": [],
    "help": [
        "Pass exactly 1 argument to match the function signature."
    ]
}
¬
Cell :: plex [T] { value T }
main :: fn () { _ := Cell[i32].missing(1) }
¬
{
    "message": "Unknown member `missing` for `plex { i32 value }`",
    "source_file": "tests/errors/153-compiler-constructs.e",
    "primary_location": {
        "line": 2,
        "column": 32
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 32,
            "length": 7,
            "message": "`plex { i32 value }` has no field or method named `missing`"
        }
    ],
    "notes": [],
    "help": [
        "Use a field or method that exists on `plex { i32 value }`."
    ]
}
¬
Point :: plex { x i32 }
main :: fn () { point := Point { x: 1 }
 { x: a, x: b } := point
 _ := a + b }
¬
{
    "message": "Type mismatch: expected `unique destructuring field`, found `x`",
    "source_file": "tests/errors/153-compiler-constructs.e",
    "primary_location": {
        "line": 3,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 10,
            "length": 1,
            "message": "This expression has type `x`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
not_display :: plex { value i32 }
describe :: fn [T] (v: T) -> string where T: Display { return v.show() }
main :: fn () { v: not_display
 _ := describe(v) }
¬
{
    "message": "Type mismatch: expected `Display implementation`, found `not_display`",
    "source_file": "tests/errors/153-compiler-constructs.e",
    "primary_location": {
        "line": 4,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 16,
            "length": 1,
            "message": "This expression has type `not_display`"
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
            "source_file": "tests/errors/153-compiler-constructs.e",
            "line": 1,
            "column": 1,
            "length": 11,
            "message": "Type `not_display` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () { value: ?i32 = 42
 _flag: bool = value }
¬
{
    "message": "Type mismatch: expected `bool`, found `?i32`",
    "source_file": "tests/errors/153-compiler-constructs.e",
    "primary_location": {
        "line": 2,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 16,
            "length": 5,
            "message": "This expression has type `?i32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
accept :: fn (_flag: bool) {}
main :: fn () { value: ?bool = no
 accept(value) }
¬
{
    "message": "Type mismatch: expected `bool`, found `?bool`",
    "source_file": "tests/errors/153-compiler-constructs.e",
    "primary_location": {
        "line": 3,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 9,
            "length": 5,
            "message": "This expression has type `?bool`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
