Point :: plex {
    x i32
    y i32
}

impl Default for Point {
    default :: fn () -> Self {
        prn("default")
        return Point { x: 10, y: 20 }
    }
}

main :: fn () -> i32 {
    point: Point = undefined
    point = Point { x: 1, y: 2 }
    prn($"point={point.x},{point.y}")
    return point.x + point.y
}
¬
3
¬
point=1,2

¬
hir 0
¬
