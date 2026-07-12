use std.atomics

main :: fn () {
    value := 1
    pointer : atomic[^i32] = ^value
    advanced := pointer + 1
}
¬
1
¬

¬
delete
¬

¬
check
