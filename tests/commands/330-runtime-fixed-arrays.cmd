fill :: fn [T] (n: i32, item: T) -> T {
    values: [n]T
    values[0] = item
    return values[0]
}
sum :: fn [T] (values: []T) -> T {
    total: T
    for value in values { total += value^ }
    return total
}
length :: fn (calls: ^i32) -> i32 {
    calls^ += 1
    return 3
}
fail :: fn () -> ?void { return nil }
cleanup :: fn (log: ^i32) -> ?void {
    n := 2
    values: [n]i32
    values[0] = 7
    defer log^ += values[0]
    undo log^ += values[0]
    fail()?
}
main :: fn () -> i32 {
    assert fill(2, 17) == 17
    calls: i32
    values: [length(^calls)]i32
    assert calls == 1
    assert values.count == 3
    assert values.size == 12
    assert values.bytes == 12
    values[0] = 5
    values[1] = 7
    assert values[2] == 0
    assert sum(values) == 12
    assert sum(values[..2]) == 12
    for value in values { value^ += 1 }
    assert sum(values) == 15
    n := 3
    other: [n + 1]u64
    n = 1
    assert other.count == 4
    assert other.size == 32
    empty: [n - 1]u8
    assert empty.count == 0
    assert empty.bytes == 0
    log: i32
    on cleanup(^log) => return 1
    assert log == 14
    for i := 0; i < 10; i += 1 {
        local: [i + 1]i32
        local[i] = i
        defer log += local[i]
        on i == 8 => break
        again
    }
    assert log == 50
    prn("Runtime fixed array lengths, views and cleanup passed")
    return 0
}

¬
0
¬
Runtime fixed array lengths, views and cleanup passed

¬
delete
¬

