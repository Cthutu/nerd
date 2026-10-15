Point :: plex { x i32 }
main :: fn () { p :: Point { x: 20 } p.x = 42 }
¬
{
    "message": "Cannot assign to `p`",
    "source_file": "tests/errors/149-immutable-aggregate-bindings.e",
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
            "message": "`p` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `p` as a variable with `:` or assign to a different mutable symbol."
    ]
}
¬
Point :: plex { x i32 }
main :: fn () { p :: Point { x: 20 } p.x += 1 }
¬
{
    "message": "Cannot assign to `p`",
    "source_file": "tests/errors/149-immutable-aggregate-bindings.e",
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
            "message": "`p` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `p` as a variable with `:` or assign to a different mutable symbol."
    ]
}
¬
Point :: plex { x i32 }
p :: Point { x: 20 }
main :: fn () { p.x = 42 }
¬
{
    "message": "Cannot assign to `p`",
    "source_file": "tests/errors/149-immutable-aggregate-bindings.e",
    "primary_location": {
        "line": 3,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 17,
            "length": 1,
            "message": "`p` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `p` as a variable with `:` or assign to a different mutable symbol."
    ]
}
¬
Point :: plex { x i32 }
p: Point: Point { x: 20 }
main :: fn () { p.x += 1 }
¬
{
    "message": "Cannot assign to `p`",
    "source_file": "tests/errors/149-immutable-aggregate-bindings.e",
    "primary_location": {
        "line": 3,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 17,
            "length": 1,
            "message": "`p` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `p` as a variable with `:` or assign to a different mutable symbol."
    ]
}
¬
Point :: plex { x i32 }
Holder :: plex { point Point }
main :: fn () { p :: Holder { point: Point { x: 20 } } p.point.x = 42 }
¬
{
    "message": "Cannot assign to `p`",
    "source_file": "tests/errors/149-immutable-aggregate-bindings.e",
    "primary_location": {
        "line": 3,
        "column": 56
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 56,
            "length": 1,
            "message": "`p` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `p` as a variable with `:` or assign to a different mutable symbol."
    ]
}
¬
Point :: plex { x i32 }
main :: fn () { p :: [Point { x: 20 }] p[0].x = 42 }
¬
{
    "message": "Cannot assign to `p`",
    "source_file": "tests/errors/149-immutable-aggregate-bindings.e",
    "primary_location": {
        "line": 2,
        "column": 40
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 40,
            "length": 1,
            "message": "`p` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `p` as a variable with `:` or assign to a different mutable symbol."
    ]
}
¬
Point :: plex { x i32 }
main :: fn () { original :: Point { x: 20 } p :: original p.x = 42 }
¬
{
    "message": "Cannot assign to `p`",
    "source_file": "tests/errors/149-immutable-aggregate-bindings.e",
    "primary_location": {
        "line": 2,
        "column": 59
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 59,
            "length": 1,
            "message": "`p` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `p` as a variable with `:` or assign to a different mutable symbol."
    ]
}
¬
Value :: union { integer i32 real f32 }
main :: fn () { value :: Value { integer: 20 } value.integer = 42 }
¬
{
    "message": "Cannot assign to `value`",
    "source_file": "tests/errors/149-immutable-aggregate-bindings.e",
    "primary_location": {
        "line": 2,
        "column": 48
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 48,
            "length": 5,
            "message": "`value` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `value` as a variable with `:` or assign to a different mutable symbol."
    ]
}
¬
Header :: plex { u8 { count: 4 _: 4 } }
main :: fn () { header :: Header { count: 1 } header.count += 1 }
¬
{
    "message": "Cannot assign to `header`",
    "source_file": "tests/errors/149-immutable-aggregate-bindings.e",
    "primary_location": {
        "line": 2,
        "column": 47
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 47,
            "length": 6,
            "message": "`header` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `header` as a variable with `:` or assign to a different mutable symbol."
    ]
}
¬
support :: use tests.mods.immutable_aggregate_bindings
main :: fn () { support.point.x = 42 }
¬
{
    "message": "Cannot assign to `point`",
    "source_file": "tests/errors/149-immutable-aggregate-bindings.e",
    "primary_location": {
        "line": 2,
        "column": 25
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 25,
            "length": 5,
            "message": "`point` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `point` as a variable with `:` or assign to a different mutable symbol."
    ]
}
¬
use tests.mods.immutable_aggregate_bindings
main :: fn () { point.x = 42 }
¬
{
    "message": "Cannot assign to `point`",
    "source_file": "tests/errors/149-immutable-aggregate-bindings.e",
    "primary_location": {
        "line": 2,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 17,
            "length": 5,
            "message": "`point` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `point` as a variable with `:` or assign to a different mutable symbol."
    ]
}
