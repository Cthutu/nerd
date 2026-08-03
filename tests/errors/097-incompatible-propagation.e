Failure :: enum {
    Bad
}

value :: fn () -> i32\Failure {
    return 1
}

main :: fn () {
    result := value()?
}
¬
{
    "message": "Cannot propagate `Failure` from a function returning `void`",
    "source_file": "tests/errors/097-incompatible-propagation.e",
    "primary_location": {
        "line": 10,
        "column": 22
    },
    "references": [
        {
            "kind": "primary",
            "line": 10,
            "column": 22,
            "length": 1,
            "message": "This `?` can return `Failure` to the caller"
        }
    ],
    "notes": [
        "The enclosing function has no compatible failure channel"
    ],
    "help": [
        "Handle the failure here, or change the function return type to `void\\Failure`"
    ]
}
