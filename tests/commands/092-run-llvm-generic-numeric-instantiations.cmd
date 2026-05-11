use std.io

add :: fn [T] (a: T, b: T) {
    return a + b
}

main :: fn () {
    prn($"Integer: {add(3, 4)}")
    prn($"Float: {add(3.1, 4.2)}")
}
¬
0
¬
Integer: 7
Float: 7.3

¬
clean-llvm
¬
--llvm
