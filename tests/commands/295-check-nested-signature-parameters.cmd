ffi "c" qsort (base: ^void, count: usize, width: usize,
               compare: ^fn (arg1: ^void, arg2: ^void) -> i32)
Factory :: fn (arg1: i32) -> fn (arg1: f32) -> f32
apply :: fn (callback: fn (arg1: i32, arg2: i32) -> i32, value: i32) -> i32 {
    return callback(value, value)
}
main :: fn () {}
¬
0
¬

¬
delete
¬

¬
check
