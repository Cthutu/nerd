make_optional :: fn (present: bool) -> ?i32 {
    on present => return 7
    return nil
}

make_result :: fn (failed: bool) -> i32\string {
    on failed => return "failed"!
    return 41
}

propagate_optional :: fn (present: bool) -> ?i32 {
    value := make_optional(present)?
    return value + 1
}

propagate_result :: fn (failed: bool) -> i32\string {
    value := make_result(failed)?
    return value + 1
}

optional_value :: fn (value: ?i32) -> i32 {
    return on value => [present] {
        break present
    } else {
        break 0
    }
}

result_value :: fn (value: i32\string) -> i32 {
    return on value => [success] {
        break success
    } else [error] {
        assert error.count > 0
        break -1
    }
}

match_result :: fn (value: i32\string) -> i32 {
    return on value {
        41      => 41
        success => success
    } else {
        "failed" => -1
        else     => -2
    }
}

match_optional :: fn (value: ?i32) -> i32 {
    return on value {
        7       => 70
        present => present
    } else {
        break 0
    }
}

main :: fn () -> i32 {
    local_optional: ?i32 = 7
    local_result: i32\string = "local"!
    return optional_value(propagate_optional(yes)) +
           optional_value(propagate_optional(no)) +
           result_value(propagate_result(no)) +
           result_value(propagate_result(yes)) +
           match_optional(make_optional(yes)) +
           optional_value(local_optional) +
           result_value(local_result) +
           match_result(make_result(no)) +
           match_result(make_result(yes)) - 115
}
¬
50
¬

¬

¬
