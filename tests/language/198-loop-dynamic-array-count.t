main :: fn () -> i32 {
    values : [..]u32 = nil
    defer values.free()
    values.resize_to(120000)

    for i: usize = 0; i < values.count; i += 1 {
        values[i] = i.as(u32)
    }

    return on values[119999] == 119999 => 0 else 1
}
¬
0
¬

¬

¬

