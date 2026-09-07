impl [T] []T {
    contains :: fn (s: ^Self, value: T) -> bool {
        return s^[0] == value
    }
}
main :: fn () => 0
¬
{
    "message": "Generic parameter `T` requires an `Eq` constraint",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 3,
        "column": 22
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 22,
            "length": 2,
            "message": "This operation requires `T: Eq`"
        }
    ],
    "notes": [],
    "help": [
        "Add `where T: Eq` to the generic function or impl."
    ]
}
¬
same :: fn [T] (lhs: T, rhs: T) => lhs == rhs
main :: fn () => 0
¬
{
    "message": "Generic parameter `T` requires an `Eq` constraint",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 1,
        "column": 40
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 40,
            "length": 2,
            "message": "This operation requires `T: Eq`"
        }
    ],
    "notes": [],
    "help": [
        "Add `where T: Eq` to the generic function or impl."
    ]
}
¬
same :: fn [T] (lhs: []T, rhs: []T) -> bool { return lhs == rhs }
main :: fn () => 0
¬
{
    "message": "Generic parameter `T` requires an `Eq` constraint",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 1,
        "column": 58
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 58,
            "length": 2,
            "message": "This operation requires `T: Eq`"
        }
    ],
    "notes": [],
    "help": [
        "Add `where T: Eq` to the generic function or impl."
    ]
}
¬
same :: fn [T] (lhs: box[T], rhs: box[T]) -> bool { return lhs == rhs }
main :: fn () => 0
¬
{
    "message": "Generic parameter `T` requires an `Eq` constraint",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 1,
        "column": 64
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 64,
            "length": 2,
            "message": "This operation requires `T: Eq`"
        }
    ],
    "notes": [],
    "help": [
        "Add `where T: Eq` to the generic function or impl."
    ]
}
¬
same :: fn [T] (lhs: ^T, rhs: T) -> bool { return lhs^ == rhs }
main :: fn () => 0
¬
{
    "message": "Generic parameter `T` requires an `Eq` constraint",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 1,
        "column": 56
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 56,
            "length": 2,
            "message": "This operation requires `T: Eq`"
        }
    ],
    "notes": [],
    "help": [
        "Add `where T: Eq` to the generic function or impl."
    ]
}
¬
impl [T] []T {
    before :: fn (s: ^Self, value: T) -> bool { return s^[0] < value }
}
main :: fn () => 0
¬
{
    "message": "Generic parameter `T` requires an `Order` constraint",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 2,
        "column": 62
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 62,
            "length": 1,
            "message": "This operation requires `T: Order`"
        }
    ],
    "notes": [],
    "help": [
        "Add `where T: Order` to the generic function or impl."
    ]
}
¬
main :: fn () {
    lhs: arena
    rhs: arena
    assert lhs == rhs
}
¬
{
    "message": "Operator `==` requires matching operands that support Eq, found `arena` and `arena`",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 4,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 16,
            "length": 2,
            "message": "These operands have types `arena` and `arena`"
        }
    ],
    "notes": [],
    "help": [
        "Use `==` only with matching operands that support Eq."
    ]
}
¬
Bits :: union { integer i32 real f32 }
main :: fn () {
    lhs: Bits
    rhs: Bits
    assert lhs == rhs
}
¬
{
    "message": "Operator `==` requires matching operands that support Eq, found `Bits` and `Bits`",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 5,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 16,
            "length": 2,
            "message": "These operands have types `Bits` and `Bits`"
        }
    ],
    "notes": [],
    "help": [
        "Use `==` only with matching operands that support Eq."
    ]
}
¬
one :: fn () => 1
main :: fn () { assert one == one }
¬
{
    "message": "Operator `==` requires matching operands that support Eq, found `fn () -> i32` and `fn () -> i32`",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 2,
        "column": 28
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 28,
            "length": 2,
            "message": "These operands have types `fn () -> i32` and `fn () -> i32`"
        }
    ],
    "notes": [],
    "help": [
        "Use `==` only with matching operands that support Eq."
    ]
}
¬
Value :: plex { x i32 }
main :: fn () {
    lhs: []Value
    rhs: []Value
    assert lhs == rhs
}
¬
{
    "message": "Operator `==` requires matching operands that support Eq, found `[]Value` and `[]Value`",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 5,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 16,
            "length": 2,
            "message": "These operands have types `[]Value` and `[]Value`"
        }
    ],
    "notes": [],
    "help": [
        "Use `==` only with matching operands that support Eq."
    ]
}
¬
main :: fn () {
    lhs: []arena
    rhs: []arena
    assert lhs == rhs
}
¬
{
    "message": "Operator `==` requires matching operands that support Eq, found `[]arena` and `[]arena`",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 4,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 16,
            "length": 2,
            "message": "These operands have types `[]arena` and `[]arena`"
        }
    ],
    "notes": [],
    "help": [
        "Use `==` only with matching operands that support Eq."
    ]
}
¬
Bits :: union { integer i32 real f32 }
main :: fn () {
    lhs: []Bits
    rhs: []Bits
    assert lhs == rhs
}
¬
{
    "message": "Operator `==` requires matching operands that support Eq, found `[]Bits` and `[]Bits`",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 5,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 16,
            "length": 2,
            "message": "These operands have types `[]Bits` and `[]Bits`"
        }
    ],
    "notes": [],
    "help": [
        "Use `==` only with matching operands that support Eq."
    ]
}
¬
Callback :: fn () -> i32
main :: fn () {
    lhs: []Callback
    rhs: []Callback
    assert lhs == rhs
}
¬
{
    "message": "Operator `==` requires matching operands that support Eq, found `[]fn () -> i32` and `[]fn () -> i32`",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 5,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 16,
            "length": 2,
            "message": "These operands have types `[]fn () -> i32` and `[]fn () -> i32`"
        }
    ],
    "notes": [],
    "help": [
        "Use `==` only with matching operands that support Eq."
    ]
}
¬
main :: fn () {
    lhs: box[arena]
    rhs: box[arena]
    assert lhs == rhs
}
¬
{
    "message": "Operator `==` requires matching operands that support Eq, found `box[arena]` and `box[arena]`",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 4,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 16,
            "length": 2,
            "message": "These operands have types `box[arena]` and `box[arena]`"
        }
    ],
    "notes": [],
    "help": [
        "Use `==` only with matching operands that support Eq."
    ]
}
¬
Bits :: union { integer i32 real f32 }
main :: fn () {
    lhs: box[Bits]
    rhs: box[Bits]
    assert lhs == rhs
}
¬
{
    "message": "Operator `==` requires matching operands that support Eq, found `box[Bits]` and `box[Bits]`",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 5,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 16,
            "length": 2,
            "message": "These operands have types `box[Bits]` and `box[Bits]`"
        }
    ],
    "notes": [],
    "help": [
        "Use `==` only with matching operands that support Eq."
    ]
}
¬
Callback :: fn () -> i32
main :: fn () {
    lhs: box[Callback]
    rhs: box[Callback]
    assert lhs == rhs
}
¬
{
    "message": "Operator `==` requires matching operands that support Eq, found `box[fn () -> i32]` and `box[fn () -> i32]`",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 5,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 16,
            "length": 2,
            "message": "These operands have types `box[fn () -> i32]` and `box[fn () -> i32]`"
        }
    ],
    "notes": [],
    "help": [
        "Use `==` only with matching operands that support Eq."
    ]
}
¬
same :: fn [T] (lhs: T, rhs: T) -> bool
where T: Eq { return lhs == rhs }
main :: fn () {
    lhs: []arena
    rhs: []arena
    assert same(lhs, rhs)
}
¬
{
    "message": "Type mismatch: expected `Eq implementation`, found `[]arena`",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 6,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 17,
            "length": 3,
            "message": "This expression has type `[]arena`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
same :: fn [T] (lhs: T, rhs: T) -> bool
where T: Eq { return lhs == rhs }
main :: fn () {
    lhs: box[arena]
    rhs: box[arena]
    assert same(lhs, rhs)
}
¬
{
    "message": "Type mismatch: expected `Eq implementation`, found `box[arena]`",
    "source_file": "tests/errors/129-content-equality.e",
    "primary_location": {
        "line": 6,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 17,
            "length": 3,
            "message": "This expression has type `box[arena]`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
