use std.io
use test.generics

main :: fn () {
    stack: Stack[i32]
    stack_push(^stack, 42)
    stack_push(^stack, 13)

    last := stack_pop(^stack)
    first := stack_pop(^stack)
    box := make_box("ok")

    prn($"{last} {first} {box.value}")
}
¬
0
¬
13 42 ok

¬

¬
