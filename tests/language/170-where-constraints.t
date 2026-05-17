Display :: trait {
    show :: fn (Self) -> string
}

Point :: plex {
    x i32
    y i32
}

impl Display for Point {
    show :: fn (self: Self) => $"Point({self.x}, {self.y})"
}

id :: fn [T] (value: T) -> T where T: Display {
    return value
}

describe :: fn [T] (value: T) -> string where T: Display {
    return value.show()
}

describe_explicit :: fn [T] (value: T) -> string where T: Display {
    return Display.show(value)
}

Box :: plex [T] {
    value T
}

impl Box[T] where T: Display {
    get :: fn (self: Self) -> T {
        return self.value
    }
}

main :: fn () -> i32 {
    point := id(Point { x: 3, y: 4 })
    return point.x + point.y + describe(point).count.as(i32) +
        describe_explicit(point).count.as(i32)
}
¬
29
¬

¬
hir 0
bind Point = type.0
bind __impl_13_show = fn.0
bind Box = type.1
bind main = fn.1
type type.0 = Point
generic type type.1 = <unknown>
func fn.0(self: Point) -> string {
  return string interpolate(<unknown> "Point(", i32 field(Point local.0(self), x), <unknown> ", ", i32 field(Point local.0(self), y), <unknown> ")")
}
func fn.1() -> i32 {
  let point: Point = Point call decl.3(id)(Point plex(x: i32 3, y: i32 4))
  return i32 add(i32 add(i32 add(i32 field(Point local.1(point), x), i32 field(Point local.1(point), y)), i32 cast(usize field(string call decl.4(describe)(Point local.1(point)), count) as i32)), i32 cast(usize field(string call decl.5(describe_explicit)(Point local.1(point)), count) as i32))
}
inst func fn.2(value: Point) -> Point {
  return Point local.2(value)
}
inst func fn.3(value: Point) -> string {
  return string call bind.1(__impl_13_show)(Point local.3(value))
}
inst func fn.4(value: Point) -> string {
  return string call bind.1(__impl_13_show)(Point local.4(value))
}
¬
