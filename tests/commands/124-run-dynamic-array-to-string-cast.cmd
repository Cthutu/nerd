use std.io

main :: fn () {
    name : [..]u8
    name.push('M')
    name.push('a')
    name.push('t')
    name.push('t')
    prn($"Hello, {name.as(string)}!")
    name.free()
}
¬
0
¬
Hello, Matt!

¬
delete
¬
--llvm
