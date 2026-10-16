Failure :: enum { Bad Other }
Translated :: enum { Converted }
State :: plex { calls i32 handled i32 defers i32 undos i32 handler_defers i32 handler_undos i32 }
operation :: fn (state: ^State, ok: bool) -> i32\Failure {
    state.calls += 1
    on !ok => return Failure.Bad!
    return 7
}
forward :: fn (state: ^State, ok: bool) -> i32\Failure {
    defer state.defers += 1
    undo state.undos += 1
    value := operation(state, ok) else [error] {
        state.handled += 1
        defer state.handler_defers += 1
        undo state.handler_undos += 1
        assert error == Failure.Bad
    }
    return value
}
translate :: fn (state: ^State) -> i32\Translated {
    value := operation(state, no) else [error] {
        assert error == Failure.Bad
        return Translated.Converted!
    }
    return value
}
recover :: fn (state: ^State) -> i32\Failure {
    value := operation(state, no) else { return 4 }
    return value
}
optional :: fn (ok: bool) -> ?i32 {
    on !ok => return nil
    return 9
}
optional_forward :: fn (state: ^State, ok: bool) -> ?i32 {
    value := optional(ok) else { state.handled += 1 }
    return value
}
optional_plain :: fn (ok: bool) -> i32 {
    value := optional(ok) else { return 6 }
    return value
}
void_operation :: fn (ok: bool) -> void\Failure {
    on !ok => return Failure.Bad!
}
void_forward :: fn (ok: bool) -> void\Failure {
    void_operation(ok) else {}
}
Record :: plex { number i32 }
record_operation :: fn () -> Record\Failure { return Record { number: 11 } }
record_forward :: fn () -> i32\Failure {
    record := record_operation() else {}
    return record.number
}
inferred :: fn () => record_operation() else {}
cached_forward :: fn (state: ^State) -> i32\Failure {
    cached := operation(state, no)
    value := cached else [error] {
        assert error == Failure.Bad
        cached = 8
    }
    return value
}
implicit_error :: fn (state: ^State) -> i32\Failure {
    cached := operation(state, no)
    value := cached else { assert cached == Failure.Bad }
    return value
}

panic_handler :: fn (state: ^State) -> i32 {
    value := operation(state, yes) else { abort("unexpected failure") }
    return value
}
exhaustive_handler :: fn (state: ^State, choice: bool) -> i32 {
    value := operation(state, no) else {
        on choice { yes => return 13 no => return 14 }
    }
    return value
}

run :: fn () -> void\Failure {
    state : State
    assert record_forward()? == 11
    assert inferred()?.number == 11
    assert forward(^state, yes)? == 7
    assert state.calls == 1 && state.handled == 0
    on forward(^state, no) {
        error! => { assert error == Failure.Bad }
        else => { assert no }
    }
    assert state.calls == 2 && state.handled == 1
    assert state.defers == 2 && state.undos == 1
    assert state.handler_defers == 1 && state.handler_undos == 1
    on translate(^state) { error! => { assert error == Translated.Converted } else => { assert no } }
    assert recover(^state)? == 4
    assert optional_plain(yes) == 9 && optional_plain(no) == 6
    present := optional_forward(^state, yes) else { return Failure.Other! }
    assert present == 9
    assert !optional_forward(^state, no)
    void_forward(yes)?
    on void_forward(no) { error! => { assert error == Failure.Bad } else => { assert no } }
    attempts := 0
    for attempts < 3 {
        attempts += 1
        value := optional(no) else { again }
        assert value == 0
    }
    assert attempts == 3
    for {
        value := optional(no) else { break }
        assert value == 0
    }
    attempts = 0
    for attempts < 3 {
        attempts += 1
        value := optional(no) else { again }
        assert value == 0
    }
    assert attempts == 3
    for {
        value := optional(no) else { break }
        assert value == 0
    }
    ignored := ${
        value := optional(no) else { break 15 }
        break value
    }
    assert ignored == 15
    outer := ${
        value := optional(no) else { break 12 }
        break value
    }
    assert outer == 12
    _ := operation(^state, yes) else { return Failure.Other! }
    assert panic_handler(^state) == 7
    assert exhaustive_handler(^state, yes) == 13
    assert exhaustive_handler(^state, no) == 14
    on cached_forward(^state) {
        error! => { assert error == Failure.Bad }
        else => { assert no }
    }
    on implicit_error(^state) {
        error! => { assert error == Failure.Bad }
        else => { assert no }
    }
    prn("bare else handler passed")
}
main :: fn () -> i32 {
    on run() { _! => return 1 else => return 0 }
}
¬
0
¬
bare else handler passed
¬
delete
¬
-r
