Error :: enum { Stop }

next_optional :: fn (calls: ^i32) -> ?bool {
    calls^ += 1
    on calls^ > 3 => return nil
    return no
}
next_result :: fn (calls: ^i32) -> i32\Error {
    calls^ += 1
    on calls^ > 3 => return (Error.Stop)!
    return 0
}
next_optional_void :: fn (calls: ^i32) -> ?void {
    calls^ += 1
    on calls^ > 3 => return nil
}
next_result_void :: fn (calls: ^i32) -> void\Error {
    calls^ += 1
    on calls^ > 3 => return (Error.Stop)!
}

main :: fn () {
    calls := 0
    bodies := 0
    for next_optional(^calls) {
        bodies += 1
        again
    }
    assert calls == 4
    assert bodies == 3

    calls = 0
    bodies = 0
    for next_result(^calls) { bodies += 1 }
    assert calls == 4
    assert bodies == 3

    calls = 0
    bodies = 0
    for next_optional_void(^calls) { bodies += 1 }
    assert calls == 4
    assert bodies == 3

    calls = 0
    bodies = 0
    for next_result_void(^calls) { bodies += 1 }
    assert calls == 4
    assert bodies == 3

    pending: ?bool = no
    for pending {
        pending = nil
        bodies += 1
    }
    assert bodies == 4
    successful: i32\Error = 0
    for successful {
        successful = (Error.Stop)!
        bodies += 1
    }
    assert bodies == 5

    calls = 0
    bodies = 0
    for i := 0; next_result(^calls); i += 1 {
        assert i == bodies
        bodies += 1
        again
    }
    assert calls == 4
    assert bodies == 3

    calls = 0
    bodies = 0
    for i := 0; next_optional(^calls); i += 1 {
        assert i == bodies
        bodies += 1
    }
    assert calls == 4
    assert bodies == 3

    missing: ?i32
    exhausted := for missing { break 10 } else { break 20 }
    assert exhausted == 20
    present: ?i32 = 0
    broken := for present { break 30 } else { break 40 }
    assert broken == 30
    failed: bool\Error = (Error.Stop)!
    error_end := for failed { break 50 } else { break 60 }
    assert error_end == 60
    false_success: bool\Error = no
    success_break := for false_success { break 70 } else { break 80 }
    assert success_break == 70

    calls = 0
    c_style := for i := 0; next_optional(^calls); i += 1 {
        on i == 1 => break 90
    } else { break 100 }
    assert c_style == 90
    assert calls == 2

    full_box := box[i32](1)
    for full_box { break }
    empty_box: box[i32]
    for empty_box { assert no }
    absent_storage: ?arena
    for absent_storage { assert no }
    failed_storage: arena\Error = (Error.Stop)!
    for failed_storage { assert no }
    for no { assert no }
}
¬
0
¬
