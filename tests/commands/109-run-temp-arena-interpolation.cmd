use std.io
use core

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

    temp_arena_reset()
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
delete
¬
--llvm
