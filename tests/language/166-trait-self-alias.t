use std.io

Display :: trait for Value {
    show :: fn (Value) -> string
}

Default :: trait for Value {
    default :: fn () -> Value
}

Point :: plex {
    x i32
    y i32
}

impl Display for Point {
    show :: fn (self: Self) => $"Point({self.x}, {self.y})"
}

impl Default for Point {
    default :: fn () -> Self {
        return Point { x: 5, y: 8 }
    }
}

main :: fn () -> i32 {
    point := Point.default()
    prn(point.show())
    return point.x + point.y
}
¬
13
¬
Point(5, 8)

¬
hir 0
¬
