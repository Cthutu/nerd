Error :: enum { Failed }
steps: i32
cleanup: i32
operation :: fn (fail: bool) -> void\Error {
    on fail => return Error.Failed!
}
run :: fn (fail: bool) -> i32\Error {
    defer cleanup += 1
    operation(fail)?
    steps += 1
    operation(no)?
    steps += 1
    return 42
}
main :: fn () {
    on run(no) => [value] { assert value == 42 } else { assert no }
    assert steps == 2 && cleanup == 1
    on run(yes) => { assert no } else [error] { assert error == Error.Failed }
    assert steps == 2 && cleanup == 2
}
¬
0
¬
