Values :: [10, 20]

main :: fn () {
    enabled := yes
    value := on enabled => {
        Values[0]
    } else {
        Values[1]
    }
    _ := value
}
¬
{
    "message": "`on` expression does not produce a value",
    "source_file": "tests/errors/103-on-branch-block-has-no-value.e",
    "primary_location": {
        "line": 5,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 14,
            "length": 2,
            "message": "This `on` expression has type `void`"
        },
        {
            "kind": "secondary",
            "line": 5,
            "column": 28,
            "length": 1,
            "message": "This braced branch is a statement block"
        }
    ],
    "notes": [],
    "help": [
        "Write each result expression directly, or use an expression block `${ ... }` with `break <value>` in each branch."
    ]
}
