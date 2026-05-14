use std.io
x :: use test.ffi_alias

c_abs :: ffi "c" abs (value: i32) -> i32
absolute :: c_abs

main :: fn () -> i32 {
    local := absolute(-9)
    imported := x.absolute(-11)
    prn($"values {local} {imported}")
    return 0
}
¬
0
¬
values 9 11

¬
delete
¬
