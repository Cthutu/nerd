use std.io

main :: fn () -> i32 {
    values: [3]i32 = [10, 20, 30]
    array_ptr: ^[3]i32 = ^values
    elem_ptr: ^i32 = ^values[1]
    literal_ptr: ^[3]i32 = ^[1, 2, 3]

    prn($"array pointer = {array_ptr[0]}")
    prn($"elem pointer = {elem_ptr[0]}")
    prn($"literal pointer = {literal_ptr[0]}")

    return array_ptr[0][0] + elem_ptr[0] + literal_ptr[0][2]
}
¬
33
¬
array pointer = [10, 20, 30]
elem pointer = 20
literal pointer = [1, 2, 3]

¬
delete
¬
--llvm-backend
