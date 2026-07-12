impl atomic[i32] {
    load :: fn (self: ^atomic[i32]) -> i32 {
        return 73
    }
}

main :: fn () -> i32 {
    value : atomic[i32] = 0
    return value.load() - 73
}
¬
0
¬
