Resource :: plex {
    active bool
}

impl Resource {
    done :: fn (resource: ^Self) {
        resource.active = no
    }
}

main :: fn () {
    defer Resource.done()
}
¬
{
    "message": "Receiver method `done` cannot be called through a type",
    "source_file": "tests/errors/102-deferred-receiver-method-through-type.e",
    "primary_location": {
        "line": 12,
        "column": 24
    },
    "references": [
        {
            "kind": "primary",
            "line": 12,
            "column": 24,
            "length": 1,
            "message": "`done` requires a receiver value"
        }
    ],
    "notes": [],
    "help": [
        "Call the method through a value, for example `value.done()`."
    ]
}
