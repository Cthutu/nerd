Box :: plex [T] {
    value T
}

IntBox :: Box[i32]

impl Box[T] {
    init :: fn (value: T) -> Box {
        return { value }
    }
}

main :: fn () -> i32 {
    box := IntBox.init(42)
    return box.value
}
¬
{
    "message": "Impl function `init` cannot be called as an associated function",
    "source_file": "tests/errors/069-associated-functions.e",
    "primary_location": {
        "line": 14,
        "column": 23
    },
    "references": [
        {
            "kind": "primary",
            "line": 14,
            "column": 23,
            "length": 1,
            "message": "`init` is called through a type, so its return type must contain `Self`"
        }
    ],
    "notes": [],
    "help": [
        "Change the impl function return type to `Self`, `^Self`, or a type such as `?Self`, or call it through a receiver value."
    ]
}
