take_slice :: fn (bytes: []u8) -> i32 {
    on bytes == nil => return 2
    return 0
}

ret_ptr :: fn () -> ^i32 {
    return nil
}

main :: fn () -> i32 {
    value: i32 = 7
    ptr: ^i32 = nil
    bytes: []u8 = nil

    ptr = ^value
    ptr^ = 9
    ptr = nil

    on take_slice(nil) != 3 - 1 => return 3
    on take_slice(bytes) != 3 - 1 => return 4
    ptr = ret_ptr()

    return value - 9
}
¬
0
¬

¬
delete
¬
--llvm-backend
