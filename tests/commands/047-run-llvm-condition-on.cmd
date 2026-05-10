use std.io

score :: fn (value: i32) -> i32 {
    return on value {
        < 0 => -1
        == 0 => 0
        1, 2 => 10
        >= 10 => 100
        else => 20
    }
}

describe :: fn (value: i32) -> string {
    return on {
        value < 0 => "negative"
        value == 0 => "zero"
        value < 10 => "small"
        else => "large"
    }
}

not_five :: fn (value: i32) -> i32 {
    return on value {
        != 5 => 1
        else => 0
    }
}

main :: fn () -> i32 {
    prn($"scores {score(-2)} {score(0)} {score(2)} {score(7)} {score(12)}")
    prn($"descriptions {describe(-2)} {describe(0)} {describe(7)} {describe(12)}")
    prn($"not-five {not_five(4)} {not_five(5)}")
    return score(12)
}
¬
100
¬
scores -1 0 10 20 100
descriptions negative zero small large
not-five 1 0

¬
delete
¬
