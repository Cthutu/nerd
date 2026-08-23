probe :: fn (calls: ^i32, value: i32) -> i32 {
    calls^ += 1
    return value
}

is_digit :: fn (value: u8) -> bool {
    return on value {
        in ['0' ..= '9'] => yes
        else => no
    }
}

main :: fn () -> i32 {
    calls := 0
    on !(probe(^calls, 5) in [probe(^calls, 0)..probe(^calls, 10)]) => return 1
    on calls != 3 => return 2

    calls = 0
    on probe(^calls, -1) in [probe(^calls, 0)..probe(^calls, 10)] => return 3
    on calls != 2 => return 4

    on !(9 in [0..=9]) => return 5
    on 10 in [0..=9] => return 6
    on !(is_digit('0') && is_digit('9')) => return 7
    on is_digit('/') || is_digit(':') => return 8

    value := 0
    for (value in [0..3]) {
        value += 1
    }
    on value != 3 => return 9

    iterations := 0
    for item in [0..3] {
        iterations += item + 1
    }
    on iterations != 6 => return 10
    return 0
}
¬
0
¬

¬
default-main
