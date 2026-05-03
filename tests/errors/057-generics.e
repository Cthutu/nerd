Box :: plex [T] {
    value T
}
main :: fn () => 0
¬
{
    "code": "0339",
    "message": "Generics are not implemented for `generic plex` yet",
    "source_file": "tests/errors/057-generics.e",
    "primary_location": {
        "line": 1,
        "column": 8
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 8,
            "length": 4,
            "message": "This generic syntax is recognised but not lowered"
        }
    ],
    "notes": [
        "The parser and formatter understand the square-bracket generic syntax, but semantic instantiation is still in progress."
    ],
    "help": [
        "Use a concrete non-generic declaration for now, or keep this source until the generics milestone is completed."
    ]
}
¬
id :: fn [T] (value: T) => value
main :: fn () => id(1)
¬
{
    "code": "0339",
    "message": "Generics are not implemented for `generic function` yet",
    "source_file": "tests/errors/057-generics.e",
    "primary_location": {
        "line": 1,
        "column": 7
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 7,
            "length": 2,
            "message": "This generic syntax is recognised but not lowered"
        }
    ],
    "notes": [
        "The parser and formatter understand the square-bracket generic syntax, but semantic instantiation is still in progress."
    ],
    "help": [
        "Use a concrete non-generic declaration for now, or keep this source until the generics milestone is completed."
    ]
}
¬
Box :: plex {
    value i32
}
x: Box[i32]
main :: fn () => 0
¬
{
    "code": "0339",
    "message": "Generics are not implemented for `generic type application` yet",
    "source_file": "tests/errors/057-generics.e",
    "primary_location": {
        "line": 4,
        "column": 7
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 7,
            "length": 1,
            "message": "This generic syntax is recognised but not lowered"
        }
    ],
    "notes": [
        "The parser and formatter understand the square-bracket generic syntax, but semantic instantiation is still in progress."
    ],
    "help": [
        "Use a concrete non-generic declaration for now, or keep this source until the generics milestone is completed."
    ]
}
