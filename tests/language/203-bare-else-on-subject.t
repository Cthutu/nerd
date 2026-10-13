calls : i32
requested :: fn () -> bool\string {
    calls += 1
    return calls == 2
}
failed :: fn () -> bool\string { return "failed"! }
failure_exit :: fn () -> i32 {
    for {
        on failed() else [error] {
            assert error == "failed"
            return 17
        } => break
    }
    return 1
}
absent :: fn () -> ?bool { return nil }
propagate_error :: fn () -> i32\string {
    on failed() else [error] { assert error == "failed" } => return 1
    return 2
}
propagate_absence :: fn () -> ?i32 {
    on absent() else {} => return 1
    return 2
}
main :: fn () -> i32 {
    for {
        on requested() else [error] {
            assert error == "failed"
            return 1
        } => break
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
    assert (propagate_error() \ 42) == 42
    assert (propagate_absence() \ 43) == 43
    on (failed() \ no) => return 3
    matched : i32
    on (on no => 1 else 2) {
        2 => matched = 2
        else => return 4
    }
    assert matched == 2
    -- An unbraced branch reserves else for the enclosing on.
    untouched := on yes => requested() else requested()
    assert (untouched \ no) == no
    nested := on yes => on requested() else { return 5 } => 7 else 9 else 11
    assert nested == 9
    recovered := on yes => (requested() else { return 6 }) else no
    assert recovered == no
    return 0
}
¬
0
¬

¬

¬
