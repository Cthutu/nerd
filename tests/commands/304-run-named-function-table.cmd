Functions :: plex #c {
    log fn (channel: ^u8, format: ^u8)
    apply fn (callback: fn (value: i32) -> i32, value: i32) -> i32
}
log :: fn (_channel: ^u8, _format: ^u8) {}
apply :: fn (operation: fn (input: i32) -> i32, number: i32) -> i32 {
    return operation(number)
}
double :: fn (n: i32) => n * 2
main :: fn () {
    funcs := Functions { log: log, apply: apply }
    callback: fn (different_name: i32) -> i32 = double
    funcs.log(nil, nil)
    assert funcs.apply(callback, 21) == 42
}
¬
0
¬
