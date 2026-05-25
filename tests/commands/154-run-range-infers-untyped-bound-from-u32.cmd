main :: fn () -> i32 {
    count: u32 = 4
    total := 0

    for value in [0 .. count] {
        total += value.as(i32)
    }

    on total != 6 => return 1
    return 0
}
¬
0
¬
