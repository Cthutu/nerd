convert :: fn {
    convert_i32
    convert_string
}

convert_i32 :: fn (value: i32) -> i32 { return value + 1 }
convert_string :: fn (value: string) -> i32 { return value.count.as(i32) }

main :: fn () -> i32 {
    selected: fn(i32) -> i32 = convert
    return convert(4) + convert("abc") + selected(5) - 14
}
¬
0
¬

¬
delete
