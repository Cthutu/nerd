use std.io

main :: fn () {
    total := 0
    for i := 0; i < 5; i += 1 $outer {
        for j := 0; j < 5; j += 1 $inner {
            on j == 1 => continue $outer
            total += 100
        }
        total += i
    }

    value :: for {
        break 7
    }
    labelled_value :: for $value_loop {
        break $value_loop 11
    }

    prn($"total = {total}")
    prn($"value = {value}")
    prn($"labelled = {labelled_value}")
    return total + value + labelled_value
}
¬
6
¬
total = 500
value = 7
labelled = 11

¬
delete
¬
