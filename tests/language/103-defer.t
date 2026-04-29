use std.io

io :: mod std.io

cleanup_return :: fn () -> i32 {
    value := 1
    defer value = 9
    return value
}

main :: fn () {
    total := cleanup_return()
    order := 0
    {
        defer order = order * 10 + 1
        defer order = order * 10 + 2
    }
    total += order
    prn($"order={order}")

    {
        defer total += 10
        defer {
            io.prn("deferred-module-call")
        }
        total += 1
    }
    prn($"after-block={total}")

    for i := 0; i < 3; i += 1 {
        defer total += 100
        on i == 0 => continue
        on i == 1 => break
        total += 1000
    }
    prn($"after-loop={total}")
    return total
}
¬
233
¬
order=21
deferred-module-call
after-block=33
after-loop=233

¬
¬
