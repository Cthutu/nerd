main :: fn () -> i32 {
    value := 1
    return on value {
        1 => ${
            break on value < 2 => 42
            break 0
        }
        else => 0
    }
}
¬
42
¬

¬
delete
¬

¬
run
¬
