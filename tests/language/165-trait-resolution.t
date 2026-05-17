use std.io

Describe :: trait {
    describe :: fn (Self) -> string
}

Point :: plex {
    x i32
}

impl Describe for Point {
    describe :: fn (self: Self) => $"trait {self.x}"
}

impl Point {
    describe :: fn (self: Self) => $"inherent {self.x}"
}

Colour :: enum {
    Red
    Blue
}

impl Describe for Colour {
    describe :: fn (self: Self) => on self {
        Red => "red"
        Blue => "blue"
    }
}

Payload :: union {
    i i32
    f f32
}

impl Describe for Payload {
    describe :: fn (self: Self) => $"payload {self.i}"
}

main :: fn () -> i32 {
    point := Point { x: 7 }
    colour: Colour = Blue
    payload: Payload = Payload { i: 9 }

    prn(point.describe())
    prn(colour.describe())
    prn(payload.describe())

    return point.x + payload.i
}
¬
16
¬
inherent 7
blue
payload 9

¬
hir 0
¬
