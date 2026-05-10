use std.io

main :: fn () -> i32 {
    values: [5]i32 = [10, 20, 30, 40, 50]
    all: []i32 = values[..]
    end :: all.count - 1

    prefix: []i32 = values[.. end]
    middle: []i32 = values[1 .. end]
    nested: []i32 = values[.. all.count - 2]

    prn($"{prefix}")
    prn($"{middle}")
    prn($"{nested}")

    return prefix[3] + middle[2] + nested[2]
}
¬
110
¬
[10, 20, 30, 40]
[20, 30, 40]
[10, 20, 30]

¬
delete
¬
