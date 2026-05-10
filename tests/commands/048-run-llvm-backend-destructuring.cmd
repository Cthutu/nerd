use std.io

main :: fn () -> i32 {
    (a, b) := (2, 3)
    prn($"{a} {b}")
    (a, b) = (b, a)
    prn($"{a} {b}")
    (a, _) = (7, 8)
    prn($"{a} {b}")
    (c, d) :: (4, "four")
    prn($"{c} {d}")
    (e, f): (i32, string) = (5, "five")
    prn($"{e} {f}")
    return 0
}
¬
0
¬
2 3
3 2
7 2
4 four
5 five

¬
delete
¬
--llvm-backend
