pair :: fn [A, B] (a: A, b: B) => a
main :: fn () => pair[i32](1, "two")
¬
{
    "message": "Argument count mismatch: expected 2, found 1",
    "source_file": "tests/errors/057-generics.e",
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
unused :: fn [T] (value: T) -> T {
    return missing_value(value)
}

main :: fn () => 0
¬
{
    "message": "Unknown symbol `missing_value`",
    "source_file": "tests/errors/057-generics.e",
    "primary_location": {
        "line": 2,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 12,
            "length": 13,
            "message": "This symbol is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Add a binding for `missing_value` or fix the spelling."
    ]
}
¬
bad_eq :: fn [T] (lhs: T, rhs: T) -> bool {
    return lhs == rhs
}

main :: fn () => 0
¬
{
    "message": "Type mismatch: expected `Eq constraint`, found `T`",
    "source_file": "tests/errors/057-generics.e",
    "primary_location": {
        "line": 2,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 12,
            "length": 3,
            "message": "This expression has type `T`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
bad_order :: fn [T] (lhs: T, rhs: T) -> bool {
    return lhs < rhs
}

main :: fn () => 0
¬
{
    "message": "Type mismatch: expected `Order constraint`, found `T`",
    "source_file": "tests/errors/057-generics.e",
    "primary_location": {
        "line": 2,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 12,
            "length": 3,
            "message": "This expression has type `T`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
add :: fn [T] (lhs: T, rhs: T) -> T {
    return lhs + rhs
}

main :: fn () {
    _ := add("a", "b")
}
¬
{
    "message": "Operator `+` requires matching numeric operands, found `string` and `string`",
    "source_file": "tests/errors/057-generics.e",
    "primary_location": {
        "line": 2,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 16,
            "length": 1,
            "message": "These operands have types `string` and `string`"
        }
    ],
    "notes": [],
    "help": [
        "Use `+` only with matching numeric operands."
    ]
}
¬
field :: fn [T] (value: T) -> i32 {
    return value.x
}

main :: fn () -> i32 {
    return field(42)
}
¬
{
    "message": "Type mismatch: expected `array, slice, string, dynamic array, module, plex, union, enum, or pointer to memberable value`, found `i32`",
    "source_file": "tests/errors/057-generics.e",
    "primary_location": {
        "line": 2,
        "column": 18
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 18,
            "length": 1,
            "message": "This expression has type `i32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
