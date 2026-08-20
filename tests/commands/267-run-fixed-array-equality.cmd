main :: fn () -> i32 {
    data: [5]u8 = [42, 42, 42, 42, 42]

    assert data == [42, 42, 42, 42, 42]
    assert data != [42, 42, 42, 42, 41]
    return 0
}
¬
0
¬

¬
delete
