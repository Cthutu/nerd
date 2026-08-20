fill :: fn (values: []i32) {
    values[1] = 42
}

sum :: fn (values: []i32) -> i32 {
    return values[0] + values[1] + values[2]
}

main :: fn () -> i32 {
    values := [1, 2, 3]
    fill(values)
    return sum(values)
}
¬
46
¬
¬
¬
