use std.io

main :: fn () {
    values : [..]u8
    values.push(1)
    values.push(2)
    prn($"values={values}")
    values.free()
}
¬
0
¬
values=[1, 2]

¬
delete
¬
--llvm
