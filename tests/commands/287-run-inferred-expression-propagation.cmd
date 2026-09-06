Failure :: enum {
    Bad
}

result_value :: fn (ok: bool) -> i32\Failure {
    on ok => return 41
    return Bad!
}

optional_value :: fn (ok: bool) -> ?i32 {
    on ok => return 42
    return nil
}

forward_result :: fn (ok: bool) => result_value(ok)? + 1
forward_optional :: fn (ok: bool) => optional_value(ok)?

main :: fn () -> i32 {
    on forward_result(no) {
        _! => {
        }
        else => return 3
    }
    on !forward_optional(no) => {
    } else return 4
    on !forward_optional(yes) => return 5
    return on forward_result(yes) {
        value => value - 42
        _!    => 2
    }
}
¬
0
¬

¬
delete
