use std.io

main :: fn () -> i32 {
    pair: (i32, i32) = (10, 20)
    pair_ptr := ^pair
    pair_ptr_ptr := ^pair_ptr

    values: [3]i32 = [1, 2, 3]
    values_ptr := ^values
    slice := values[..]
    slice_ptr := ^slice

    prn($"tuple {pair_ptr.0} {pair_ptr_ptr.1}")
    prn($"counts {values.count} {values_ptr.count} {slice.count} {slice_ptr.count}")
    prn($"data {values.data[0]} {values_ptr.data[1]} {[4, 5, 6].data[2]}")
    return pair_ptr.0 + pair_ptr_ptr.1 + values.data[0] +
        values_ptr.data[1] + [4, 5, 6].data[2]
}
¬
39
¬
tuple 10 20
counts 3 3 3 3
data 1 2 6

¬
delete
¬
