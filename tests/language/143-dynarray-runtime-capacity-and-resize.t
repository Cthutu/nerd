make :: fn (initial_size: usize) -> i32 {
    values: [initial_size..]i32
    on values.capacity < initial_size => return 1
    on values.count != 0 => return 2

    values.resize_undefined(initial_size)
    values[0] = 7
    values[1] = 8
    values[2] = 9

    values.resize(5)
    on values.count != 5 => return 3
    on values[3] != 0 => return 4
    on values[4] != 0 => return 5

    result := values[0] + values[1] + values[2]
    values.free()
    return result
}

main :: fn () -> i32 {
    return make(3)
}
¬
24
¬
¬
¬
