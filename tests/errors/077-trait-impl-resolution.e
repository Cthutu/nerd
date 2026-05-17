Display :: trait {
    show :: fn (Self) -> string
}

Point :: plex {
    x i32
}

impl Printable for Point {
    show :: fn (self: Self) => "point"
}

main :: fn () => 0
¬
{
    "message": "Type mismatch: expected `known trait`, found `Printable`",
    "source_file": "tests/errors/077-trait-impl-resolution.e",
    "primary_location": {
        "line": 9,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 9,
            "column": 1,
            "length": 4,
            "message": "This expression has type `Printable`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Display :: trait {
    show :: fn (Self) -> string
}

Point :: plex {
    x i32
}

impl Display for Point {
    show :: fn (self: Self) => "point"
}

bad :: fn [T] (value: T) -> string {
    return value.show()
}

main :: fn () {
    point := Point { x: 1 }
    _ := bad(point)
}
¬
{
    "message": "Type mismatch: expected `Display constraint`, found `T`",
    "source_file": "tests/errors/077-trait-impl-resolution.e",
    "primary_location": {
        "line": 14,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 14,
            "column": 12,
            "length": 5,
            "message": "This expression has type `T`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Display :: trait {
    show :: fn (Self) -> string
}

impl Display for Missing {
    show :: fn (self: Self) => "missing"
}

main :: fn () => 0
¬
{
    "message": "Unknown type `Missing`",
    "source_file": "tests/errors/077-trait-impl-resolution.e",
    "primary_location": {
        "line": 5,
        "column": 18
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 18,
            "length": 7,
            "message": "This type name is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Use a defined type name, or one of the built-in primitive types."
    ]
}
¬
Display :: trait {
    show :: fn (Self) -> string
}

Point :: plex {
    x i32
}

impl Display for Point {
    show :: fn (self: Self) => "first"
}

impl Display for Point {
    show :: fn (self: Self) => "second"
}

main :: fn () => 0
¬
{
    "message": "Duplicate binding for symbol `Display for Point`",
    "source_file": "tests/errors/077-trait-impl-resolution.e",
    "primary_location": {
        "line": 13,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 13,
            "column": 1,
            "length": 4,
            "message": "This binding redefines `Display for Point`"
        },
        {
            "kind": "secondary",
            "line": 9,
            "column": 1,
            "length": 4,
            "message": "Previous binding of `Display for Point` is here"
        }
    ],
    "notes": [],
    "help": [
        "Rename one of the bindings or remove the duplicate definition."
    ]
}
¬
Named :: trait {
    name :: fn (Self) -> string
}

Labelled :: trait {
    name :: fn (Self) -> string
}

Point :: plex {
    x i32
}

impl Named for Point {
    name :: fn (self: Self) => "named"
}

impl Labelled for Point {
    name :: fn (self: Self) => "labelled"
}

main :: fn () {
    point := Point { x: 1 }
    _ := point.name()
}
¬
{
    "message": "Ambiguous method call `name`",
    "source_file": "tests/errors/077-trait-impl-resolution.e",
    "primary_location": {
        "line": 23,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 23,
            "column": 16,
            "length": 4,
            "message": "Multiple trait implementations provide `name` for this receiver type"
        }
    ],
    "notes": [],
    "help": [
        "Call an inherent method with a unique name, or use an explicit trait call such as `Trait.member(value, ...)`."
    ]
}
¬
Display :: trait {
    show :: fn (Self) -> string
}

Point :: plex {
    x i32
}

impl Display for Point {
    show :: fn (self: Self) => "point"
}

main :: fn () {
    point := Point { x: 1 }
    _ := show(point)
}
¬
{
    "message": "Trait member `show` must be called explicitly",
    "source_file": "tests/errors/077-trait-impl-resolution.e",
    "primary_location": {
        "line": 15,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 15,
            "column": 10,
            "length": 4,
            "message": "`show` is a member of trait `Display`"
        }
    ],
    "notes": [],
    "help": [
        "Use receiver syntax, `Display.show(value, ...)`, or `Display[Type].show(...)`."
    ]
}
¬
Iterator :: trait [Item] {
    next :: fn (Self) -> Item
}

impl Iterator[i32] for i32 {
    next :: fn (self: Self) => self
}

main :: fn () => 0
¬
{
    "message": "Generics are not implemented for `generic trait implementation` yet",
    "source_file": "tests/errors/077-trait-impl-resolution.e",
    "primary_location": {
        "line": 5,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 14,
            "length": 1,
            "message": "This generic syntax is recognised but not lowered"
        }
    ],
    "notes": [
        "The parser and formatter understand the square-bracket generic syntax, but semantic instantiation is still in progress."
    ],
    "help": [
        "Use a concrete non-generic declaration for now."
    ]
}
¬
value :: fn [T] (input: T) -> T where T: Missing {
    return input
}

main :: fn () => 0
¬
{
    "message": "Type mismatch: expected `known trait`, found `Missing`",
    "source_file": "tests/errors/077-trait-impl-resolution.e",
    "primary_location": {
        "line": 1,
        "column": 42
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 42,
            "length": 7,
            "message": "This expression has type `Missing`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Display :: trait {
    show :: fn (Self) -> string
}

Plain :: plex {
    x i32
}

value :: fn [T] (input: T) -> T where T: Display {
    return input
}

main :: fn () {
    plain := Plain { x: 1 }
    _ := value(plain)
}
¬
{
    "message": "Type mismatch: expected `Display implementation`, found `Plain`",
    "source_file": "tests/errors/077-trait-impl-resolution.e",
    "primary_location": {
        "line": 15,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 15,
            "column": 16,
            "length": 5,
            "message": "This expression has type `Plain`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
