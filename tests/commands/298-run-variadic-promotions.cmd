use std.io
sum :: fn (count: i32, args: ...) -> f64 {
    assert count == 2
    a := args.next[i32]()
    b := args.next[f64]()
    return a.as(f64) + b
}
first :: fn (_fixed: i32, args: ...) => args.next[i32]()
borrow_alias :: fn (args: VaList) -> i32 {
    Args :: VaList
    duplicate: Args = args.copy()
    return duplicate.next[i32]()
}
second :: fn (_fixed: i32, args: ...) -> i32 {
    _ := args.next[i32]()
    return borrow_alias(args)
}
failure :: fn (_fixed: i32, args: ...) -> ?i32 {
    _copy := args.copy()
    absent: ?i32 = nil
    _ := absent?
    return args.next[i32]()
}
main :: fn () {
    _result := failure(0, 42)
    assert first(0, 42) == 42
    assert second(0, 20, 42) == 42
    cb: fn (arg1: i32, ...) -> f64 = sum
    a: i8 = 40
    b: f32 = 2.0
    assert cb(2, a, b) == 42.0
    assert sum(2, a, b) == 42.0
    prn("42")
}
¬
0
¬
42
