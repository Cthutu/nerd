Point :: plex {
    x i32
    y i32
}

impl Display for Point {
    show :: fn (self: Self) -> string => $"Point({self.x}, {self.y})"
}

Display :: trait {
    show :: fn (Self) -> string
}

main :: fn () -> i32 {
    point := Point { x: 3, y: 4 }
    return point.x + point.y
}
¬
7
¬

¬
hir 0
bind Point = type.0
bind main = fn.0
type type.0 = Point
func fn.0() -> i32 {
  let point: Point = Point plex(x: i32 3, y: i32 4)
  return i32 add(i32 field(Point local.0(point), x), i32 field(Point local.0(point), y))
}
¬
