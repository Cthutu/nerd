main :: fn () -> i32 {
    item: i32
    ptr: ^i32 = ^item
    nil_ptr: ^i32
    void_ptr: ^void = ptr

    on nil_ptr {
        nil => {}
        else => return 1
    }

    on ptr == nil => return 2
    on nil != nil_ptr => return 3
    on void_ptr != ptr => return 4
    return 0
}
¬
0
¬
¬
¬
