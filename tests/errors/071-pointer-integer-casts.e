main :: fn () {
    value: i32 = 4096
    ptr := value.as(^u8)
}
¬
{
    "message": "Cannot cast `i32` to `^u8`",
    "source_file": "tests/errors/071-pointer-integer-casts.e",
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
            "message": "This cast is not supported"
        }
    ],
    "notes": [],
    "help": [
        "Use explicit casts only between compatible primitive types."
    ]
}
