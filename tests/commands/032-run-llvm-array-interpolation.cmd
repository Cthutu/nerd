use std.io

main :: fn () -> i32 {
    xs :: [1, 2, 3]
    ys: [2]i32 = [4, 5]

    prn($"xs = {xs[0]}, {xs[2]}")
    prn($"xs array = {xs}")
    prn($"ys = {ys[1]}")
    prn($"ys array = {ys}")

    return xs[1] + ys[0] + ys[1]
}
¬
11
¬
xs = 1, 3
xs array = [1, 2, 3]
ys = 5
ys array = [4, 5]

¬
delete
¬
