main :: fn () -> i32 {
    width: u32 = 3
    height: u32 = 2
    num_bytes := width * height
    total := 0

    for i := 0; i < num_bytes; i += 1 {
        total += i.as(i32)
    }

    on total != 15 => return 1
    return 0
}
¬
0
¬

