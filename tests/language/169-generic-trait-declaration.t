Iterator :: trait [Item] {
    next :: fn (Self) -> Item
}

Mapper :: trait [Input, Output] for This {
    map :: fn (This, Input) -> Output
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
