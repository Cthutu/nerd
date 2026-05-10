main :: fn () -> i32 {
    fixed: [3]i32 = [10, 20, 30]
    fixed_ptr: ^i32 = ^fixed[1]
    fixed_ptr^ = 21

    slice: []i32 = fixed[..]
    slice_ptr: ^i32 = ^slice[2]
    slice_ptr^ = 32

    text := "abc"
    char_ptr: ^u8 = ^text[1]

    return fixed[1] + slice[2] + char_ptr^.as(i32)
}
¬
151
¬

¬
delete
¬
--llvm-backend
