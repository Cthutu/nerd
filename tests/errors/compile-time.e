choose :: fn (enabled :: bool) -> i32 {
    return on enabled => 1 else 0
}

main :: fn () -> i32 {
    runtime := yes
    return choose(runtime)
}
¬
{
    "message": "Argument for compile-time parameter `enabled` is not constant",
    "source_file": "tests/errors/compile-time.e",
    "primary_location": {
        "line": 7,
        "column": 19
    },
    "references": [
        {
            "kind": "primary",
            "line": 7,
            "column": 19,
            "length": 7,
            "message": "This value is only known at runtime"
        }
    ],
    "notes": [],
    "help": [
        "Branch on the runtime value and call the function with an explicit constant in each branch."
    ]
}
