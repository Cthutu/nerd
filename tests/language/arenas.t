
=== case: 108-run-core-arena ===
use std.io


main :: fn () -> i32 {
    a: arena
    a = arena(16, 8)

    first := a.alloc[i32]()
    first^ = 41

    bytes := a.alloc_array[u8](9000)
    bytes[0] = 1
    bytes[8999] = 2

    first^ += 1
    prn($"first={first^} bytes={bytes[0]}:{bytes[8999]}")

    a.reset()
    reused := a.alloc[i32]()
    reused^ = 5

    result := on first == reused {
        yes  => reused^
        else => 99
    }

    a.done()
    return result
}
¬
5
¬
first=42 bytes=1:2

¬

¬


=== case: 109-run-temp-arena-interpolation ===
use std.io

make :: fn (value: i32) -> string {
    return $"value-{value}-abcdefghijklmnopqrstuvwxyz"
}

main :: fn () -> i32 {
    total := 0
    last: string = ""

    for i := 0; i < 2500; i += 1 {
        text := make(i)
        total += text.count.as(i32)
        last = text
    }

    prn($"total={total} last={last}")

    temp_arena.reset()
    after := make(7)
    prn(after)

    return on total > 65000 && after.count > 0 {
        yes  => 7
        else => 1
    }
}
¬
7
¬
total=91390 last=value-2499-abcdefghijklmnopqrstuvwxyz
value-7-abcdefghijklmnopqrstuvwxyz

¬

¬


=== case: 110-run-core-arena-mark-restore ===
use std.io


main :: fn () -> i32 {
    a := arena(16, 8)
    defer a.done()

    first := a.alloc[i32]()
    first^ = 11

    mark := a.mark()
    second := a.alloc[i32]()
    second^ = 22

    bytes := a.alloc_array[u8](9000)
    bytes[0] = 3
    bytes[8999] = 4

    on first^ != 11 => return 2

    a.restore(mark)
    replacement := a.alloc[i32]()
    replacement^ = 33

    on replacement != second => return 3
    prn($"first={first^} replacement={replacement^}")
    return first^ + replacement^
}
¬
44
¬
first=11 replacement=33

¬

¬


=== case: 272-run-core-arena-pr ===
use std.io

main :: fn () -> i32 {
    storage := arena(32)
    defer storage.done()

    temporary_mark := temp_arena.mark()
    storage_mark := storage.mark()
    first := storage.pr("value")
    second := storage.prn($"-{41 + 1}")
    after_generated := storage.mark()
    empty := storage.pr("")
    newline := storage.prn("")

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

¬


=== case: 275-run-imported-default-arena ===
use test.default_arena

main :: fn () -> i32 {
    text := copy_to("default arena")
    return on text == "default arena" { yes => 0 else => 1 }
}
¬
0
¬

¬

¬


=== case: 337-current-thread-arena ===
main :: fn () {
    assert current_temp_arena() == temp_arena
    mark := current_temp_arena().mark()
    prn($"current arena {42}")
    current_temp_arena().restore(mark)
}
¬
0
¬
current arena 42
¬

¬
