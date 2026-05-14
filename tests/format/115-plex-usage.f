use std.io
use std.random

main :: fn () {
    for _ in [0 .. 10] {
        prn($"{random_u64()}")
    }

    test()
}

test :: fn () {
    details: plex {
        age i32
        name string
    }
    = {
        age: 42, name: "Alice"
    }

    prn(on details {
        {
            age: 42, name: _
        }
        => $"Hello {details.name}!"
        else => "Whatever!"
    }
    )
}
¬
use std.io
use std.random

main :: fn () {
    for _ in [0 .. 10] {
        prn($"{random_u64()}")
    }

    test()
}

test :: fn () {
    details: plex {
        age  i32
        name string
    }
    = { age: 42 name: "Alice" }

    prn(on details {
        { age: 42, name: _ } => $"Hello {details.name}!"
        else => "Whatever!"
    })
}
