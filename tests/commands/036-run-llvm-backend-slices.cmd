use std.io

main :: fn () -> i32 {
    values: [5]i32 = [10, 20, 30, 40, 50]
    all: []i32 = values[..]
    middle: []i32 = values[1..4]
    from_start: []i32 = values[..3]
    to_end: []i32 = values[2..]
    literal: []i32 = [1, 2, 3][..]
    reslice: []i32 = all[1..3]

    prn($"all = {all}")
    prn($"middle = {middle}")
    prn($"from_start = {from_start}")
    prn($"to_end = {to_end}")
    prn($"literal = {literal}")
    prn($"reslice count = {reslice.count}")
    prn($"data first = {middle.data[0]}")

    return middle[1] + from_start[2] + to_end[0] + literal[2]
}
¬
93
¬
all = [10, 20, 30, 40, 50]
middle = [20, 30, 40]
from_start = [10, 20, 30]
to_end = [30, 40, 50]
literal = [1, 2, 3]
reslice count = 2
data first = 20

¬
delete
¬
--llvm-backend
