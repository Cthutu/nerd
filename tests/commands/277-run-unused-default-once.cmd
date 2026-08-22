default_value :: fn (calls: ^i32) -> i32 {
    calls^ += 1
    return 9
}

select :: fn (calls: ^i32, value: i32 = default_value(calls)) -> i32 {
    return value
}

main :: fn () -> i32 {
    calls : i32 = 0
    explicit := select(^calls, 4)
    on explicit != 4 || calls != 0 => return 1
    implicit := select(^calls)
    return on implicit == 9 && calls == 1 { yes => 0 else => 2 }
}
¬
0
¬

¬
delete
¬
--llvm
