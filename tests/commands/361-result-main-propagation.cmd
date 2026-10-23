signal :: use std.signal
fail :: fn () -> i32\signal.Error {
    error: signal.Error = signal.Error.Unsupported
    return error!
}
main :: fn (args: []string) -> i32\signal.Error {
    assert args.count >= 1
    defer prn("defer")
    undo prn("undo")
    return fail()?
}

¬
1

¬
undo
defer

¬
delete

¬

¬
run

¬
Error: shutdown watching is unsupported on this platform
