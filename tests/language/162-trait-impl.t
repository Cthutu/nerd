use std.io

Point :: plex {
    x i32
    y i32
}

impl Display for Point {
    show :: fn (self: Self) => $"Point({self.x}, {self.y})"
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
module module.0(162-trait-impl.input)
import module.1(std.io)
import import.0 prn from module.2(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.5: fn (string) -> string
bind prn = import.0
bind input = import.1
bind Point = type.0
bind __impl_8_show = fn.0
bind main = fn.1
type type.0 = Point
func fn.0(self: Point) -> string {
  return string interpolate(<unknown> "Point(", i32 field(Point local.0(self), x), <unknown> ", ", i32 field(Point local.0(self), y), <unknown> ")")
}
func fn.1() -> i32 {
  let point: Point = Point plex(x: i32 3, y: i32 4)
  expr void call bind.0(prn)(string call bind.3(__impl_8_show)(Point local.1(point)))
  return i32 add(i32 field(Point local.1(point), x), i32 field(Point local.1(point), y))
}
¬
