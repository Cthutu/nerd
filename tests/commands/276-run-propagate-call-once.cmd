Failure :: enum {
    Bad
}

make :: fn (calls: ^i32) -> i32\Failure {
    calls^ += 1
    return 7
}

forward :: fn (calls: ^i32) -> i32\Failure {
    value := make(calls)?
    return value
}

main :: fn () -> i32 {
    calls : i32 = 0
    result := forward(^calls)
    return on result {
        value => on value == 7 && calls == 1 { yes => 0 else => calls }
        _!    => 10
    }
}
¬
0
¬

¬
delete
¬
--llvm
