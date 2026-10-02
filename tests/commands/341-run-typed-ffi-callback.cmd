Compare :: fn (left: ^void, right: ^void) -> i32

ffi "c" {
    qsort (base: ^void, count: usize, size: usize, compare: Compare)
}

compare :: fn (left: ^void, right: ^void) -> i32 {
    a := left.as(^i32)[0]
    b := right.as(^i32)[0]
    return (a > b).as(i32) - (a < b).as(i32)
}

main :: fn () {
    values := [9, -4, 7, 0, 9, 2]
    callback: Compare = compare
    qsort(values.data.as(^void), 6, i32.size, callback)
    assert values[0] == -4 && values[1] == 0 && values[2] == 2
    assert values[3] == 7 && values[4] == 9 && values[5] == 9
    prn("Typed native callback passed")
}
¬
0
¬
Typed native callback passed

¬
delete
¬
