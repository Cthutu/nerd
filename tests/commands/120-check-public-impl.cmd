use test.public_impl

main :: fn () -> i32 {
    box := make_box(1)
    box.bump(41)
    return box.get().as(i32)
}
¬
0
¬

¬
delete
¬

¬
check
