use std.io

main :: fn () -> i32 {
    value := 2
    hit := 0

    on value == 2 => {
        on {
            value == 1 => hit = 1
            else => hit = 2
        }
    }

    prn($"hit {hit}")
    return hit
}
¬
2
¬
hit 2

¬
delete
¬
