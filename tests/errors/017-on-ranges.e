value: i32 = 2
main :: fn () => on value {
    2..2 => 10
    else => 20
}
¬
{
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
            "length": 2,
            "message": "This range cannot match any values"
        }
    ],
    "notes": [],
    "help": [
        "Use a lower bound that is strictly less than the upper bound for `..` ranges."
    ]
}
¬
