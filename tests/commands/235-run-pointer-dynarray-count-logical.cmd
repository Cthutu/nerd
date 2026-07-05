check :: fn (output: ^[..]u8, max: usize) -> bool {
    return max != 0 && output.count >= max
}

main :: fn () -> i32 {
    bytes : [..]u8
    defer bytes.free()
    bytes.push(1)

    on !check(^bytes, 2) => return 0
    return 1
}
¬
0
¬

¬
delete
