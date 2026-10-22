use std.io
Error :: enum { Failed }
missing :: fn () -> ?void { return nil }
error :: fn () -> void\Error { return (Error.Failed)! }

failure :: fn (log: ^i32) -> ?void {
    view: []u8
    undo { assert view[0] == 9 log^ = log^ * 10 + 1 }
    defer { assert view[0] == 9 log^ = log^ * 10 + 4 }
    n := 32
    values: [n]u8
    values[0] = 9
    view = values
    defer { assert values[0] == 9 log^ = log^ * 10 + 2 }
    undo { assert values[0] == 9 log^ = log^ * 10 + 3 }
    missing()?
}
result_failure :: fn (log: ^i32) -> void\Error {
    n := 64
    values: [n]u8
    values[0] = 7
    defer log^ += values[0].as(i32)
    undo log^ += values[0].as(i32)
    error()?
}
early :: fn (n: i32, log: ^i32) -> i32 {
    -- Returning before a declaration must not restore an uninitialised save.
    on n == 0 => return 0
    values: [n]i32
    values[0] = 42
    defer log^ += values[0]
    return values[0]
}
main :: fn () {
    log: i32
    {
        view: []u8
        defer { assert view[0] == 23 log = log * 10 + 1 }
        n := 32
        values: [n]u8
        assert values[0] == 0 && values[n - 1] == 0
        values[0] = 23
        view = values
        defer { assert values[0] == 23 log = log * 10 + 2 }
        other: [n + 1]u64
        other[n] = 99
        defer { assert other[n] == 99 }
    }
    assert log == 21
    log = 0
    on failure(^log) => { assert no }
    assert log == 3241
    log = 0
    on result_failure(^log) => { assert no }
    assert log == 14
    log = 0
    assert early(0, ^log) == 0 && log == 0
    assert early(3, ^log) == 42 && log == 42

    count := 32768
    total: i32
    -- 64 MiB across iterations: storage must be reclaimed on every exit.
    for i := 0; i < 2048; i += 1 {
        values: [count]u8
        values[count - 1] = 1
        defer total += values[count - 1].as(i32)
        on i % 2 == 0 => again
    }
    assert total == 2048
    for i := 0; i < 2048; i += 1 {
        step := ${
            values: [count]u8
            values[0] = 1
            defer total += values[0].as(i32)
            break 1
        }
        assert step == 1
    }
    assert total == 4096
    for i := 0; i < 1024; i += 1 {
        for scratch: [count]u8; yes; {
            scratch[0] = 1
            defer total += scratch[0].as(i32)
            break
        }
    }
    assert total == 5120
    selected := for scratch: [count]u8; yes; {
        scratch[0] = 3
        defer total += scratch[0].as(i32)
        break scratch[0].as(i32)
    } else {
        break 0
    }
    assert selected == 3 && total == 5123
    prn("Runtime arrays use scoped stack storage and ordered cleanup")
}
¬
0
¬
Runtime arrays use scoped stack storage and ordered cleanup

¬

¬
