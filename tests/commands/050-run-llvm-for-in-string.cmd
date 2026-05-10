use std.io

main :: fn () -> i32 {
    values: [3]i32 = [1, 2, 3]

    total := 0
    for item in values[..] {
        total += item^
    }

    text := "ab"
    count := 0
    sum := 0
    for ch in text {
        count += 1
        sum += ch^.as(i32)
    }

    ptr: ^i32 = ^values[1]
    prn($"{ptr^} {total} {count} {sum}")
    return ptr^ + total + count
}
¬
10
¬
2 6 2 195

¬
delete
¬
