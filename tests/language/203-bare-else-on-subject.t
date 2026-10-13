calls : i32
requested :: fn () -> bool\string {
    calls += 1
    return calls == 2
}
failed :: fn () -> bool\string { return "failed"! }
failure_exit :: fn () -> i32 {
    for {
        on (failed() else [error] {
            assert error == "failed"
            return 17
        }) => break
    }
    return 1
}
main :: fn () -> i32 {
    for {
        on (requested() else [error] {
            assert error == "failed"
            return 1
        }) => break
    }
    assert calls == 2
    calls = 0
    for {
        on {
            (requested() else { return 2 }) => break
        }
    }
    assert calls == 2
    assert failure_exit() == 17
    on (failed() \ no) => return 3
    matched : i32
    on (on no => 1 else 2) {
        2 => matched = 2
        else => return 4
    }
    assert matched == 2
    return 0
}
¬
0
¬

¬

¬
