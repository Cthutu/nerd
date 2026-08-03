Counter :: plex {
    value i32
}

impl Counter {
    increment :: fn (counter: ^Self) {
        counter.value += 1
    }
}

main :: fn () {
    Counter.increment()
}
¬
{
    "message": "Receiver method `increment` cannot be called through a type",
    "source_file": "tests/errors/101-receiver-method-through-type.e",
    "primary_location": {
        "line": 12,
        "column": 22
    },
    "references": [
        {
            "kind": "primary",
            "line": 12,
            "column": 22,
            "length": 1,
            "message": "`increment` requires a receiver value"
        }
    ],
    "notes": [],
    "help": [
        "Call the method through a value, for example `value.increment()`."
    ]
}
