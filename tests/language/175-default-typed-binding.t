Point :: plex {
    x i32
    y i32
}

impl Default for Point {
    default :: fn () -> Self {
        return Point { x: 10, y: 20 }
    }
}

main :: fn () -> i32 {
    point: Point
    prn($"point={point.x},{point.y}")
    return point.x + point.y
}
¬
30
¬
point=10,20

¬
hir 0
¬
