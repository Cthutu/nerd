factorial :: fn (value: u64) -> u64 {
    on value <= 1 => return 1
    return value * factorial(value - 1)
}

main :: fn () -> i32 {
    return on factorial(5) == 120 => 0 else 1
}
¬
0
¬
¬
delete
