use std.io

Named :: trait {
    name :: fn (Self) -> string
}

Labelled :: trait {
    name :: fn (Self) -> string
}

Point :: plex {
    x i32
}

impl Named for Point {
    name :: fn (self: Self) => $"named {self.x}"
}

impl Labelled for Point {
    name :: fn (self: Self) => $"label {self.x}"
}

main :: fn () -> i32 {
    point := Point { x: 7 }
    prn(Named.name(point))
    prn(Labelled.name(point))
    return point.x
}
¬
7
¬
named 7
label 7

¬
hir 0
¬
