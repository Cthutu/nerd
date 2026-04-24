main :: fn () => on 1.cast(i32) => 42 else 7
¬
{
    "code": "0319",
    "message": "`on` condition must have type `bool`, found `i32`",
    "source_file": "tests/errors/015-on-short-form.e",
    "primary_location": {
        "line": 1,
        "column": 22
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 22,
            "length": 1,
            "message": "This condition is not boolean"
        }
    ],
    "notes": [],
    "help": [
        "Use a `bool` expression here. Comparisons and richer patterns can be added in later milestones."
    ]
}
¬
enabled: bool = yes
main :: fn () => on enabled => 42 else "no"
¬
{
    "code": "0320",
    "message": "`on` branches must return the same type",
    "source_file": "tests/errors/015-on-short-form.e",
    "primary_location": {
        "line": 2,
        "column": 40
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 40,
            "length": 4,
            "message": "This branch has type `string`"
        },
        {
            "kind": "secondary",
            "line": 2,
            "column": 32,
            "length": 2,
            "message": "The other branch has type `untyped integer`"
        }
    ],
    "notes": [],
    "help": [
        "Make both branches produce exactly the same type."
    ]
}
