Point :: plex {
    x i32
}

impl Display for Point {
    show :: fn (self: Self) -> string => "point"
}

Display :: trait {
    show :: fn (Self) -> string
}

main :: fn () -> i32 {
    return 0
}
¬
0
¬

¬
hir 0
bind Point = type.0
bind main = fn.0
type type.0 = Point
func fn.0() -> i32 {
  return i32 0
}
¬
