use std.text

check :: fn (format: string, args: ...) {
    mark := temp_arena.mark()
    defer temp_arena.restore(mark)
    message := format_c(format, args)
    assert message == "[test] 0042 3.50 %"
    assert format_c(format, args) == message
    assert format_c(c"[%s] %04d %.2f %%", args) == message
    assert args.next[^i8]().as([]i8, 4).as(string) == "test"
    assert args.next[i32]() == 42
    assert args.next[f64]() == 3.5
    assert message == "[test] 0042 3.50 %"
}
empty :: fn (format: string, args: ...) {
    assert format_c(format, args) == ""
}
main :: fn () {
    -- The selected view has no NUL terminator and must not read the suffix.
    backing := "[%s] %04d %.2f %% ignored %d"
    check(backing[0..17], c"test", 42, 3.5)
    empty("")
}
¬
0
¬
