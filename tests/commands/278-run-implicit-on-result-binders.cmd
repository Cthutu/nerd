make_result :: fn (failed: bool) -> i32\string {
    on failed => return "fail"!
    return 21
}

read_result :: fn (value: i32\string) -> i32 {
    on value => {
        return value * 2
    } else {
        return value.count.as(i32)
    }
}

read_optional :: fn (value: ?i32) -> i32 {
    on value => {
        return value
    } else {
        assert value == nil
        return 0
    }
}

check_bool :: fn (flag: bool) {
    on flag => {
        assert flag
    } else {
        assert !flag
    }
}

main :: fn () -> i32 {
    check_bool(yes)
    check_bool(no)
    return read_result(make_result(no)) +
           read_result(make_result(yes)) +
           read_optional(7) +
           read_optional(nil) - 53
}
¬
0
¬

¬
delete
