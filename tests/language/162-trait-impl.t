use std.io

Point :: plex {
    x i32
    y i32
}

impl Display for Point {
    show :: fn (self: Self) -> string {
        on self.x == 3 && self.y == 4 => return "Point(3, 4)"
        return "Point"
    }
}

Display :: trait {
    show :: fn (Self) -> string
}

main :: fn () -> i32 {
    point := Point { x: 3, y: 4 }
    prn(point.show())
    return point.x + point.y
}
¬
7
¬
Point(3, 4)

¬
hir 0
bind Point = type.0
bind __impl_8_show = fn.0
bind main = fn.1
type type.0 = Point
func fn.0(self: Point) -> string {
  expr void on bool logical_and(bool equal(i32 field(Point local.0(self), x), i32 3), bool equal(i32 field(Point local.0(self), y), i32 4)) {
    value(bool yes) => {
      return string "Point(3, 4)"
    }
  }
  return string "Point"
}
func fn.1() -> i32 {
  let point: Point = Point plex(x: i32 3, y: i32 4)
  expr void call bind.2(prn)(string call bind.6(__impl_8_show)(Point local.1(point)))
  return i32 add(i32 field(Point local.1(point), x), i32 field(Point local.1(point), y))
}
¬
