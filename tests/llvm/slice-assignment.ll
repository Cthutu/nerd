main :: fn () {
    xs := [1, 2, 3]
    xs[..2] = 7
    xs[1..] = xs[..2]
}
¬
declare i1 @nrt_slice_eq(ptr, i64, ptr, i64, i64)
declare void @nrt_slice_check(i64, i64, i64)
declare void @nrt_slice_copy(ptr, i64, ptr, i64, i64)
declare void @nrt_slice_fill(ptr, i64, ptr, i64)
call void @nrt_slice_check(i64 3, i64 0, i64 2)
call void @nrt_slice_fill(ptr %t6, i64 %t7, ptr %t8, i64 4)
call void @nrt_slice_check(i64 3, i64 1, i64 3)
call void @nrt_slice_check(i64 3, i64 0, i64 2)
call void @nrt_slice_copy(ptr %t15, i64 %t16, ptr %t17, i64 %t18, i64 4)
