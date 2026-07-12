use std.atomics

main :: fn () {
    value := 1
    pointer : atomic[^i32] = ^value
    read := pointer^
}
¬
1
¬

¬
delete
¬

¬
check
