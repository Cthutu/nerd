Iterator :: trait [Item] {
    next :: fn (Self) -> Item
}

first :: fn [T] (iter: T) -> T
where T: Iterator {
    return iter
}

main :: fn () => 0
¬
{
    "message": "Generic trait `Iterator` expects 1 type argument, found 0",
    "source_file": "tests/errors/086-trait-generic-constraints.e",
    "primary_location": {
        "line": 6,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 10,
            "length": 8,
            "message": "Trait generic parameter `Item` cannot be inferred from this constraint"
        }
    ],
    "notes": [],
    "help": [
        "Provide exactly 1 explicit type argument for `Iterator`."
    ]
}
¬
Iterator :: trait [Item] {
    next :: fn (Self) -> Item
}

first :: fn [T] (iter: T) -> T
where T: Iterator[i32, string] {
    return iter
}

main :: fn () => 0
¬
{
    "message": "Generic trait `Iterator` expects 1 type argument, found 2",
    "source_file": "tests/errors/086-trait-generic-constraints.e",
    "primary_location": {
        "line": 6,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 10,
            "length": 8,
            "message": "This trait constraint has the wrong number of type arguments"
        }
    ],
    "notes": [],
    "help": [
        "Provide exactly 1 explicit type argument for `Iterator`."
    ]
}
