Display :: trait {
    show :: fn (Self) -> string
}

Hasher :: trait {
    hash   :: fn (Self) -> u64
    finish :: fn () -> u64
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
