add :: fn (a: i32, b: i32) => a + b
main :: fn () => add(20)
¬
{
    "message": "Argument count mismatch: expected 2, found 1",
    "source_file": "tests/errors/011-function-calls.e",
    "primary_location": {
        "line": 2,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 21,
            "length": 1,
            "message": "This call uses the wrong arity"
        }
    ],
    "notes": [],
    "help": [
        "Pass exactly 2 arguments to match the function signature."
    ]
}
¬
neg :: (0 - 1)
use_u32 :: fn (value: u32) => 0
main :: fn () => use_u32(neg)
¬
{
    "message": "Cannot infer negative integer as `u32`",
    "source_file": "tests/errors/011-function-calls.e",
    "primary_location": {
        "line": 3,
        "column": 26
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 26,
            "length": 3,
            "message": "This value is negative but `u32` is unsigned"
        }
    ],
    "notes": [],
    "help": [
        "Use a non-negative value here, or change the destination type to a signed integer."
    ]
}
