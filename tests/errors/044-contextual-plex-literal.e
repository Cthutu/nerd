Point :: plex { x i32 y i32 }

main :: fn () {
    p := { x: 1, y: 2 }
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `plex or union type`, found `<unknown>`",
    "source_file": "tests/errors/044-contextual-plex-literal.e",
    "primary_location": {
        "line": 4,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 10,
            "length": 1,
            "message": "This expression has type `<unknown>`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
