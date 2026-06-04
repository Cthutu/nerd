sum :: fn (bytes: []u8) -> i32 {
    return bytes.count.as(i32) + bytes[0].as(i32) + bytes[2].as(i32)
}

main :: fn () -> i32 {
    bytes: [3]u8 = [10, 20, 7]
    return sum(bytes)
}
¬
20
¬

¬
delete
¬
