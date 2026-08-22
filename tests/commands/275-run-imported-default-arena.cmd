use test.default_arena

main :: fn () -> i32 {
    text := copy_to("default arena")
    return on text == "default arena" { yes => 0 else => 1 }
}
¬
0
¬

¬
delete
¬
--llvm
