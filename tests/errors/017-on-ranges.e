value: i32 = 2
main :: fn () => on value {
    2..<2 => 10
    else => 20
}
¬
{
    "code": "0324",
    "message": "Block-form `on` range pattern is empty",
    "source_file": "tests/errors/017-on-ranges.e",
    "primary_location": {
        "line": 3,
        "column": 6
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 6,
            "length": 3,
            "message": "This range cannot match any values"
        }
    ],
    "notes": [],
    "help": [
        "Use a lower bound that is strictly less than the upper bound for `..<` ranges."
    ]
}
¬
value: i32 = 2
limit: i32 = 2
main :: fn () => on value {
    0..<limit => 10
    else => 20
}
¬
{
    "code": "0322",
    "message": "Block-form `on` patterns must be compile-time constants",
    "source_file": "tests/errors/017-on-ranges.e",
    "primary_location": {
        "line": 4,
        "column": 6
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 6,
            "length": 3,
            "message": "This pattern is not constant"
        }
    ],
    "notes": [],
    "help": [
        "Use a literal or folded constant binding for this pattern until richer pattern forms land."
    ]
}
