ffi "c" qsort (base: ^void, count: usize, width: usize,
               compare: ^fn (^void, ^void) -> i32)
Factory :: fn (i32) -> fn (f32) -> f32
apply :: fn (callback: fn (i32, i32) -> i32, value: i32) -> i32 {
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
