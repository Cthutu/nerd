Display :: trait {
    show :: fn (Self) -> string
}

id :: fn [T] (value: T) -> T where T: Display {
    return value
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
    return 0
}
¬
0
¬

¬
hir 0
bind Box = type.0
bind main = fn.0
generic type type.0 = <unknown>
func fn.0() -> i32 {
  return i32 0
}
¬
