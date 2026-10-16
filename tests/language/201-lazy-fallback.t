calls : i32
fallbacks : i32

optional :: fn (present: bool) -> ?i32 {
    calls += 1
    on present => return 7
    return nil
}

result :: fn (success: bool) -> i32\string {
    calls += 1
    on success => return 11
    return "failed"!
}

fallback :: fn () -> i32 {
    fallbacks += 1
    return 42
}

recover :: fn [T] (value: ?T, default: T) -> T {
    return value \ default
}

recover_result :: fn [T] (value: T\string, default: T) -> T {
    return value \ default
}

early_return :: fn (present: bool) -> i32 {
    value := optional(present) \ ${ return 99 }
    return value
}

fallible_fallback :: fn () -> i32\string {
    value := optional(no) \ result(no)?
    return value
}

Record :: plex { value i32 }

void_result :: fn (success: bool) -> void\string {
    on success => return
    return "failed"!
}

void_fallback :: fn () {
    fallbacks += 1
}

main :: fn () -> i32 {
    assert optional(yes) \ fallback() == 7
    assert result(yes) \ fallback() == 11
    assert calls == 2 && fallbacks == 0

    assert optional(no) \ fallback() == 42
    assert result(no) \ fallback() == 42
    assert calls == 4 && fallbacks == 2

    -- Chains are right associative and stop at the first available value.
    assert optional(yes) \ result(yes) \ fallback() == 7
    assert calls == 5 && fallbacks == 2
    assert optional(no) \ result(yes) \ fallback() == 11
    assert calls == 7 && fallbacks == 2
    assert result(no) \ optional(no) \ fallback() == 42
    assert calls == 9 && fallbacks == 3

    -- Fallback binds above multiplication, below prefix and postfix operators.
    assert optional(yes) \ 2 * 3 + 1 == 22
    assert optional(no) \ (2 * 3 + 1) == 7
    assert optional(no) \ -2 * 3 == -6
    assert result(no) \ fallback().as(i32) == 42

    empty : ?i32
    assert empty \ ${ break 23 } == 23
    assert early_return(yes) == 7
    assert early_return(no) == 99
    failed := fallible_fallback()
    on failed => { return 1 } else [error] { assert error == "failed" }

    assert recover[i32](nil, 13) == 13
    assert recover_result[i32]("bad"!, 17) == 17
    present : ?string = "present"
    absent : ?string
    assert present \ "default" == "present"
    assert absent \ "default" == "default"
    record : ?Record
    recovered := record \ Record { value: 19 }
    assert recovered.value == 19
    small : ?u8
    inferred := small \ 255
    assert inferred == 255
    _ := optional(yes) \ fallback() -- Explicitly discarding stays lazy.

    -- Extract exactly one layer, including explicitly left-grouped chains.
    inner : ?i32 = 29
    nested : ??i32 = inner
    unwrapped := nested \ inner
    assert unwrapped \ 0 == 29
    assert (nested \ inner) \ 0 == 29
    widened : i32\string = empty \ 31
    assert widened \ 0 == 31

    before := fallbacks
    void_result(yes) \ void_fallback()
    assert fallbacks == before
    void_result(no) \ void_fallback()
    assert fallbacks == before + 1
    optional_void : ?void
    optional_void \ void_fallback()
    assert fallbacks == before + 2
    return 0
}
¬
0
¬

¬

¬
