message :: "hello"
main :: fn () => on message {
    else => 1
}
¬
{
    "code": "0321",
    "message": "Block-form `on` does not support values of type `string`",
    "source_file": "tests/errors/016-on-value-branches.e",
    "primary_location": {
        "line": 2,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 21,
            "length": 7,
            "message": "This matched value type is unsupported"
        }
    ],
    "notes": [],
    "help": [
        "For this milestone, block-form `on` supports `bool` and concrete integer scrutinees."
    ]
}
¬
pattern: i32 = 1
value: i32 = 2
main :: fn () => on value {
    pattern => 10
    else => 20
}
¬
{
    "code": "0322",
    "message": "Block-form `on` patterns must be compile-time constants",
    "source_file": "tests/errors/016-on-value-branches.e",
    "primary_location": {
        "line": 4,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 5,
            "length": 7,
            "message": "This pattern is not constant"
        }
    ],
    "notes": [],
    "help": [
        "Use a literal or folded constant binding for this pattern until richer pattern forms land."
    ]
}
