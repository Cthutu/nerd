-- test-platform: linux

pub use variadic_source

pub string_eq :: fn (a: i32, b: i32) -> i32 { return a + b }
pub string_builder_reset :: fn (a: i32, b: i32) -> i32 { return a + b }
pub va_integers :: fn (count: i32, args: ...) -> i64 {
    total: i64 = 0
    for _ in [0..count] { total += args.next[i32]().as(i64) }
    return total
}
pub va_doubles :: fn (count: i32, args: ...) -> f64 {
    total: f64 = 0.0
    for _ in [0..count] { total += args.next[f64]() }
    return total
}
pub va_mixed :: fn (marker: i32, expected: ^i8, args: ...) -> i32 {
    assert marker == 7
    assert args.next[i32]() == -3
    assert args.next[i32]() == 65000
    assert args.next[f64]() == 1.25
    assert args.next[^i8]() == expected
    assert args.next[i64]() == -9000000000
    assert args.next[u64]() == 18000000000
    return 42
}
read_two :: fn (args: VaList) -> i32 {
    a := args.next[i32]()
    return a + args.next[i32]()
}
copies_impl :: fn (mode: i32, args: ...) -> i32 {
    duplicate := args.copy()
    defer assert duplicate.next[i32]() == 20
    assert read_two(args) == 42
    if_copy := duplicate.copy()
    on mode == 0 => { return read_two(if_copy) }
    return read_two(if_copy)
}
pub va_copies :: copies_impl
pub va_format :: fn (format: ^i8, args: ...) -> i32 {
    assert args.format(format) == "    1.25 ok 42"
    assert args.next[i32]() == 8
    return 42
}
¬
0
¬
¬
build-variadic-host
¬
--dll
¬
build
