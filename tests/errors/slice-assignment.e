main :: fn () {
    xs :: [1, 2]
    xs[..] = 3
}
¬
{
    "message": "Cannot assign to `xs`",
    "source_file": "tests/errors/slice-assignment.e",
    "primary_location": {
        "line": 3,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 5,
            "length": 2,
            "message": "`xs` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `xs` as a variable with `:` or assign to a different mutable symbol."
    ]
}
¬
main :: fn () {
    text := "hello"
    text[1..3] = "x"
}
¬
{
    "message": "Type mismatch: expected `writable array or slice range`, found `string`",
    "source_file": "tests/errors/slice-assignment.e",
    "primary_location": {
        "line": 3,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 9,
            "length": 1,
            "message": "This expression has type `string`"
        }
    ],
    "notes": [
        "Strings do not support range assignment."
    ],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
    xs: [2]i32
    ys: [2]u8
    xs[..] = ys[..]
}
¬
{
    "message": "Type mismatch: expected `[]i32`, found `[]u8`",
    "source_file": "tests/errors/slice-assignment.e",
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
    xs: [2]box[i32]
    xs[..] = nil
}
¬
{
    "message": "Type mismatch: expected `copyable range element`, found `box[i32]`",
    "source_file": "tests/errors/slice-assignment.e",
    "primary_location": {
        "line": 3,
        "column": 7
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 7,
            "length": 1,
            "message": "This expression has type `box[i32]`"
        }
    ],
    "notes": [
        "Range assignment duplicates element values. Owning values and atomic storage require individual operations."
    ],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Owned :: plex { owner arena }
main :: fn () {
    xs: [2]Owned
    xs[..] = xs[0]
}
¬
{
    "message": "Type mismatch: expected `copyable range element`, found `Owned`",
    "source_file": "tests/errors/slice-assignment.e",
    "primary_location": {
        "line": 4,
        "column": 7
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 7,
            "length": 1,
            "message": "This expression has type `Owned`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/slice-assignment.e",
            "line": 1,
            "column": 1,
            "length": 5,
            "message": "Type `Owned` is defined here"
        }
    ],
    "notes": [
        "Range assignment duplicates element values. Owning values and atomic storage require individual operations."
    ],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Owned :: plex { owner box[i32] }
main :: fn () {
    xs: [2]Owned
    xs[..] = xs[..]
}
¬
{
    "message": "Type mismatch: expected `copyable range element`, found `Owned`",
    "source_file": "tests/errors/slice-assignment.e",
    "primary_location": {
        "line": 4,
        "column": 7
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 7,
            "length": 1,
            "message": "This expression has type `Owned`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/slice-assignment.e",
            "line": 1,
            "column": 1,
            "length": 5,
            "message": "Type `Owned` is defined here"
        }
    ],
    "notes": [
        "Range assignment duplicates element values. Owning values and atomic storage require individual operations."
    ],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
fill :: fn [T] (xs: []T, value: T) { xs[..] = value }
main :: fn () {
    xs: [2]box[i32]
    fill(xs[..], nil)
}
¬
{
    "message": "Type mismatch: expected `copyable range element`, found `box[i32]`",
    "source_file": "tests/errors/slice-assignment.e",
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
            "message": "This expression has type `box[i32]`"
        }
    ],
    "notes": [
        "Range assignment duplicates element values. Owning values and atomic storage require individual operations."
    ],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
    xs: [2]i32
    xs[..] += 1
}
¬
{
    "message": "Expected Equal `=` but found PlusEqual `+=`",
    "source_file": "tests/errors/slice-assignment.e",
    "primary_location": {
        "line": 3,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 12,
            "length": 2,
            "message": "Found PlusEqual `+=` here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}
