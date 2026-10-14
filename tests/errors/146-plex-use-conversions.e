Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
main :: fn () { p: Point _r: Rect = p }
¬
{
    "message": "Type mismatch: expected `Rect`, found `Point`",
    "source_file": "tests/errors/146-plex-use-conversions.e",
    "primary_location": {
        "line": 3,
        "column": 37
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 37,
            "length": 1,
            "message": "This expression has type `Point`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/146-plex-use-conversions.e",
            "line": 2,
            "column": 1,
            "length": 4,
            "message": "Type `Rect` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/146-plex-use-conversions.e",
            "line": 1,
            "column": 1,
            "length": 5,
            "message": "Type `Point` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
main :: fn () { p: ^Point _r: ^Rect = p }
¬
{
    "message": "Type mismatch: expected `^Rect`, found `^Point`",
    "source_file": "tests/errors/146-plex-use-conversions.e",
    "primary_location": {
        "line": 3,
        "column": 39
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 39,
            "length": 1,
            "message": "This expression has type `^Point`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/146-plex-use-conversions.e",
            "line": 2,
            "column": 1,
            "length": 4,
            "message": "Type `Rect` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/146-plex-use-conversions.e",
            "line": 1,
            "column": 1,
            "length": 5,
            "message": "Type `Point` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
Plain :: plex { x f32 y f32 w f32 h f32 }
main :: fn () { r: Plain _p: Point = r }
¬
{
    "message": "Type mismatch: expected `Point`, found `Plain`",
    "source_file": "tests/errors/146-plex-use-conversions.e",
    "primary_location": {
        "line": 4,
        "column": 38
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 38,
            "length": 1,
            "message": "This expression has type `Plain`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/146-plex-use-conversions.e",
            "line": 1,
            "column": 1,
            "length": 5,
            "message": "Type `Point` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/146-plex-use-conversions.e",
            "line": 3,
            "column": 1,
            "length": 5,
            "message": "Type `Plain` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
main :: fn () { r: Rect _p: ^Point = r }
¬
{
    "message": "Type mismatch: expected `^Point`, found `Rect`",
    "source_file": "tests/errors/146-plex-use-conversions.e",
    "primary_location": {
        "line": 3,
        "column": 38
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 38,
            "length": 1,
            "message": "This expression has type `Rect`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/146-plex-use-conversions.e",
            "line": 1,
            "column": 1,
            "length": 5,
            "message": "Type `Point` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/146-plex-use-conversions.e",
            "line": 2,
            "column": 1,
            "length": 4,
            "message": "Type `Rect` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
main :: fn () { r: Rect _p := r.as(^Point) }
¬
{
    "message": "Cannot cast `Rect` to `^Point`",
    "source_file": "tests/errors/146-plex-use-conversions.e",
    "primary_location": {
        "line": 3,
        "column": 32
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 32,
            "length": 1,
            "message": "This cast is not supported"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/146-plex-use-conversions.e",
            "line": 2,
            "column": 1,
            "length": 4,
            "message": "Type `Rect` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/146-plex-use-conversions.e",
            "line": 1,
            "column": 1,
            "length": 5,
            "message": "Type `Point` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Use explicit casts only between compatible primitive types."
    ]
}
¬
Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
main :: fn () { r: []^Rect _p: []^Point = r }
¬
{
    "message": "Type mismatch: expected `[]^Point`, found `[]^Rect`",
    "source_file": "tests/errors/146-plex-use-conversions.e",
    "primary_location": {
        "line": 3,
        "column": 43
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 43,
            "length": 1,
            "message": "This expression has type `[]^Rect`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/146-plex-use-conversions.e",
            "line": 1,
            "column": 1,
            "length": 5,
            "message": "Type `Point` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/146-plex-use-conversions.e",
            "line": 2,
            "column": 1,
            "length": 4,
            "message": "Type `Rect` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
main :: fn () { r: ^^Rect _p: ^^Point = r }
¬
{
    "message": "Type mismatch: expected `^^Point`, found `^^Rect`",
    "source_file": "tests/errors/146-plex-use-conversions.e",
    "primary_location": {
        "line": 3,
        "column": 41
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 41,
            "length": 1,
            "message": "This expression has type `^^Rect`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/146-plex-use-conversions.e",
            "line": 1,
            "column": 1,
            "length": 5,
            "message": "Type `Point` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/146-plex-use-conversions.e",
            "line": 2,
            "column": 1,
            "length": 4,
            "message": "Type `Rect` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Pair :: plex [T] { first T }
IntPair :: plex { use Pair[i32] extra u8 }
main :: fn () { r: IntPair _p: Pair[f32] = r }
¬
{
    "message": "Type mismatch: expected `plex { f32 first }`, found `IntPair`",
    "source_file": "tests/errors/146-plex-use-conversions.e",
    "primary_location": {
        "line": 3,
        "column": 44
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 44,
            "length": 1,
            "message": "This expression has type `IntPair`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/146-plex-use-conversions.e",
            "line": 2,
            "column": 1,
            "length": 7,
            "message": "Type `IntPair` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
Other :: plex { colour u8 }
Both :: plex { use Point use Other }
a :: fn (_p: ^Point) -> i32 { return 1 }
b :: fn (_p: ^Other) -> i32 { return 2 }
choose :: fn { a b }
main :: fn () { r: Both _v := choose(^r) }
¬
{
    "message": "Type mismatch: expected `exactly one compatible compound function member`, found `ambiguous compound call`",
    "source_file": "tests/errors/146-plex-use-conversions.e",
    "primary_location": {
        "line": 8,
        "column": 37
    },
    "references": [
        {
            "kind": "primary",
            "line": 8,
            "column": 37,
            "length": 1,
            "message": "This expression has type `ambiguous compound call`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Empty :: plex {}
Left :: plex { left u8 use Empty }
Right :: plex { right u8 use Empty }
Both :: plex { use Left use Right }
main :: fn () { r: Both _p: ^Empty = ^r }
¬
{
    "message": "Type mismatch: expected `unique plex use path`, found `ambiguous plex use paths`",
    "source_file": "tests/errors/146-plex-use-conversions.e",
    "primary_location": {
        "line": 5,
        "column": 38
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 38,
            "length": 1,
            "message": "This expression has type `ambiguous plex use paths`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Empty :: plex {}
Left :: plex { left u8 use Empty }
Right :: plex { right u8 use Empty }
Both :: plex { use Left use Right }
main :: fn () { r: Both _p := (^r).as(^Empty) }
¬
{
    "message": "Type mismatch: expected `unique plex use path`, found `ambiguous plex use paths`",
    "source_file": "tests/errors/146-plex-use-conversions.e",
    "primary_location": {
        "line": 5,
        "column": 35
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 35,
            "length": 1,
            "message": "This expression has type `ambiguous plex use paths`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
rect_only :: fn (_r: ^Rect) {}
main :: fn () { _f: fn (p: ^Point) = rect_only }
¬
{
    "message": "Type mismatch: expected `fn (^Point) -> void`, found `fn (^Rect) -> void`",
    "source_file": "tests/errors/146-plex-use-conversions.e",
    "primary_location": {
        "line": 4,
        "column": 38
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 38,
            "length": 9,
            "message": "This expression has type `fn (^Rect) -> void`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/146-plex-use-conversions.e",
            "line": 1,
            "column": 1,
            "length": 5,
            "message": "Type `Point` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/146-plex-use-conversions.e",
            "line": 2,
            "column": 1,
            "length": 4,
            "message": "Type `Rect` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
