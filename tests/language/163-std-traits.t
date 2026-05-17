use std.io
use std.traits

Point :: plex {
    x i32
    y i32
}

impl Display for Point {
    show :: fn (self: Self) => $"Point({self.x}, {self.y})"
}

impl Eq for Point {
    eq :: fn (lhs: Self, rhs: Self) => lhs.x == rhs.x && lhs.y == rhs.y
}

impl Default for Point {
    default :: fn () -> Self {
        return Point { x: 0, y: 0 }
    }
}

main :: fn () -> i32 {
    point := Point { x: 3, y: 4 }
    origin := Point.default()
    prn(point.show())
    prn($"eq={point.eq(origin)}")
    return point.x + point.y + origin.x + origin.y
}
¬
7
¬
Point(3, 4)
eq=no

¬
hir 0
¬
