main :: fn () {
    for {
        break
    } else {
    }
}
¬
{
    "message": "Loop `else` cannot run on an infinite loop",
    "source_file": "tests/errors/080-loop-else-infinite.e",
    "primary_location": {
        "line": 4,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 12,
            "length": 1,
            "message": "This `else` block is unreachable because the loop has no exhaustion condition"
        }
    ],
    "notes": [
        "Loop `else` blocks run only when a finite loop exhausts without `break`."
    ],
    "help": [
        "Remove the `else` block, add a loop condition, or use `break` to handle the exit path explicitly."
    ]
}
