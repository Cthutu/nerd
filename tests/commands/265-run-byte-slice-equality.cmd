main :: fn () -> i32 {
    data: []u8 = [3, 4, 5, 6, 7, 6, 7]

    assert data == [3, 4, 5, 6, 7, 6, 7]
    assert data != [3, 4, 5, 6, 7, 6, 8]
    assert data != [3, 4, 5]
    return 0
}
¬
0
¬

¬
delete
