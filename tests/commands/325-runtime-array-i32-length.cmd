work :: fn (n: i32) -> i32 {
    values: [n]i32
    values[0] = 42
    return values[0]
}
main :: fn () -> i32 {
    return work(2) - 42
}
¬
0
¬

¬
delete
¬
--llvm
