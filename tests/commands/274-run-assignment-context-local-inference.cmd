main :: fn () -> i32 {
    count : usize = 4
    start := 0

    for i in [0 ..= count] {
        start = i + count
    }

    assert start == 8
    return 0
}
¬
0
¬

¬
delete
