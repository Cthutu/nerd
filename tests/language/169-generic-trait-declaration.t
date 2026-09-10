Iterator :: trait [Item] {
    next :: fn (self: Self) -> Item
}

Mapper :: trait [Input, Output] for This {
    map :: fn (value: This, other: Input) -> Output
}

main :: fn () -> i32 {
    return 0
}
¬
0
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  return i32 0
}
¬
