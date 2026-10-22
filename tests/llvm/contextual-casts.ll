main :: fn () {
    a: u32 = 42
    b: u64 = a.as() + 1
    values := [1, 2, 3]
    view: []i32 = values.as()
    pointer := values.data
    prefix: []i32 = pointer.as(2)
    prn($"{b} {view} {prefix}")
}
¬
%t0 = zext i32 42 to i64
%t6 = insertvalue { ptr, i64 } poison, ptr %t5, 0
%t7 = insertvalue { ptr, i64 } %t6, i64 3, 1
%t9 = insertvalue { ptr, i64 } poison, ptr %t8, 0
%t10 = insertvalue { ptr, i64 } %t9, i64 2, 1
