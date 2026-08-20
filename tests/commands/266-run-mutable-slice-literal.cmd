main :: fn () -> i32 {
    data: []u8 = [3, 4, 5]
    data[1] = 9

    assert data == [3, 9, 5]
    return 0
}
¬
0
¬

¬
delete
