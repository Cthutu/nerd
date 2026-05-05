Mode :: enum {
    Read = 1
    Write = 1
}

main :: fn () {}
¬
{
    "code": "0342",
    "message": "Duplicate enum discriminant value `1`",
    "source_file": "tests/errors/068-enum-discriminants.e",
    "primary_location": {
        "line": 3,
        "column": 13
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 13,
            "length": 1,
            "message": "This variant also uses value `1`"
        },
        {
            "kind": "secondary",
            "line": 2,
            "column": 12,
            "length": 1,
            "message": "Previous variant with value `1` is here"
        }
    ],
    "notes": [],
    "help": [
        "Give one of the variants a distinct explicit value, or remove the explicit value so the implicit sequence does not collide."
    ]
}
¬
Mode :: enum {
    Read
    Write(i32)
    Read(string)
}

main :: fn () {}
¬
{
    "code": "0343",
    "message": "Duplicate enum variant `Read`",
    "source_file": "tests/errors/068-enum-discriminants.e",
    "primary_location": {
        "line": 4,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 5,
            "length": 4,
            "message": "This variant reuses `Read`"
        },
        {
            "kind": "secondary",
            "line": 2,
            "column": 5,
            "length": 4,
            "message": "Previous variant `Read` is here"
        }
    ],
    "notes": [],
    "help": [
        "Rename one of the variants so every variant in the enum is unique."
    ]
}
¬
Mode :: enum {
    Read = 1
    Write = 0
    Execute
}

main :: fn () {}
¬
{
    "code": "0342",
    "message": "Duplicate enum discriminant value `1`",
    "source_file": "tests/errors/068-enum-discriminants.e",
    "primary_location": {
        "line": 4,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 5,
            "length": 7,
            "message": "This variant also uses value `1`"
        },
        {
            "kind": "secondary",
            "line": 2,
            "column": 12,
            "length": 1,
            "message": "Previous variant with value `1` is here"
        }
    ],
    "notes": [],
    "help": [
        "Give one of the variants a distinct explicit value, or remove the explicit value so the implicit sequence does not collide."
    ]
}
