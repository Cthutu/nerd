use std.io

Default :: trait {
    default :: fn () -> Self
}

Point :: plex {
    x i32
    y i32
}

impl Default for Point {
    default :: fn () -> Self {
        return Point { x: 13, y: 21 }
    }
}

main :: fn () -> i32 {
    point := Default[Point].default()
    prn($"Point({point.x}, {point.y})")
    return point.x + point.y
}
¬
34
¬
Point(13, 21)

¬
hir 0
¬
