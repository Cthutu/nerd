Point :: plex {
    x i32
    y i32
}

main :: fn () -> i32 {
    point: Point
    prn($"point={point.x},{point.y}")
    return point.x + point.y
}
¬
0
¬
point=0,0

¬
hir 0
¬
