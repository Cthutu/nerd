normalise :: fn (seed: u64) -> u64 {
    on seed == 0 => seed = 7
    return seed
}

main :: fn () => normalise(0).as(i32)
¬
{
    "message": "Cannot assign to `seed`",
    "source_file": "tests/errors/081-parameter-immutability.e",
    "primary_location": {
        "line": 2,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 21,
            "length": 4,
            "message": "`seed` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `seed` as a variable with `:` or assign to a different mutable symbol."
    ]
}
¬
State :: plex {
    player_loc i32
}

move :: fn (state: State) {
    state.player_loc = 1
}

main :: fn () {
    state := State { player_loc: 0 }
    move(state)
}
¬
{
    "message": "Cannot assign to `state`",
    "source_file": "tests/errors/081-parameter-immutability.e",
    "primary_location": {
        "line": 6,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 5,
            "length": 5,
            "message": "`state` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `state` as a variable with `:` or assign to a different mutable symbol."
    ]
}
¬
write :: fn (values: [1]i32) {
    values[0] = 1
}

main :: fn () {
    values: [1]i32 = [0]
    write(values)
}
¬
{
    "message": "Cannot assign to `values`",
    "source_file": "tests/errors/081-parameter-immutability.e",
    "primary_location": {
        "line": 2,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 5,
            "length": 6,
            "message": "`values` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `values` as a variable with `:` or assign to a different mutable symbol."
    ]
}
¬
normalise :: fn (seed: u64) -> u64 {
    (seed) = (7)
    return seed
}

main :: fn () => normalise(0).as(i32)
¬
{
    "message": "Cannot assign to `seed`",
    "source_file": "tests/errors/081-parameter-immutability.e",
    "primary_location": {
        "line": 2,
        "column": 6
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 6,
            "length": 4,
            "message": "`seed` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `seed` as a variable with `:` or assign to a different mutable symbol."
    ]
}
