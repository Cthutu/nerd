main :: fn () -> i32 {
    data: [5]u8 = [0, 0, 0, 0, 0]
    return on data == [42, 42, 42, 42, 42] {
        yes => 0
        else => 1
    }
}
¬
@.slice.const.m0.15 = private unnamed_addr constant [5 x i8] [i8 42, i8 42, i8 42, i8 42, i8 42]

define internal i32 @fn.0() {
  %t11 = alloca [5 x i8]
  store [5 x i8] %t4, ptr %t11
  %t12 = call i1 @slice_eq(ptr %t11, i64 5, ptr @.slice.const.m0.15, i64 5, i64 1)
