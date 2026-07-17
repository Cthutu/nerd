imported :: use test.compound

numeric :: fn {
    increment
}

convert :: fn {
    numeric
    string_count
}

with_default :: fn {
    add
    prefix_count
}

increment :: fn (value: i32) -> i32 {
    return value + 1
}

string_count :: fn (value: string) -> i32 {
    return value.count.as(i32)
}

add :: fn (lhs: i32, rhs: i32 = 2) -> i32 {
    return lhs + rhs
}

prefix_count :: fn (prefix: string, value: string = "x") -> i32 {
    return (prefix.count + value.count).as(i32)
}

main :: fn () -> i32 {
    concrete: fn(i32) -> i32 = convert
    _address: ^fn(i32) -> i32 = ^convert
    return convert(4) + convert("abc") + concrete(5) + with_default(8) +
           with_default("ab") + imported.convert(1) +
           imported.convert("ab") - 57
}
¬
0
¬

¬
hir 0
¬
; nerd llvm-ir 0
; generated from HIR
