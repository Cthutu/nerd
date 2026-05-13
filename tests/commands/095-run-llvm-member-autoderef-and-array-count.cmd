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
    return pair_ptr.0 + pair_ptr_ptr.1
}
¬
30
¬
tuple 10 20
counts 3 3 3 3

¬
delete
¬
