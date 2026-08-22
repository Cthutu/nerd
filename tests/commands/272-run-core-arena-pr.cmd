use std.io

main :: fn () -> i32 {
    storage := arena(32)
    defer storage.done()

    temporary_mark := temp_arena.mark()
    storage_mark := storage.mark()
    first := storage.pr("value")
    second := storage.prn($"-{41 + 1}")
    after_generated := storage.mark()
    empty := storage.pr()
    newline := storage.prn()

    pr(first)
    pr(second)

    assert first == "value"
    assert second == "-42\n"
    assert empty == ""
    assert newline == "\n"
    assert temp_arena.mark() == temporary_mark
    assert after_generated - storage_mark == (first.count + second.count).as(u32)
    return 0
}
¬
0
¬
value-42

¬
delete
¬
--llvm
