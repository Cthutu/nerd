main :: fn() -> i32 {
    values := [1, 2, 3, 4]
    all: []i32 = values[..]
    middle: []i32 = values[1..3]
    return all[0] + middle[0]
}
¬
define i32 @fn.0() {
  %local.0 = alloca [4 x i32]
  store [4 x i32] [i32 1, i32 2, i32 3, i32 4], ptr %local.0
  %t0 = getelementptr inbounds [4 x i32], ptr %local.0, i64 0, i64 0
  %t1 = insertvalue { ptr, i64 } poison, ptr %t0, 0
  %t2 = insertvalue { ptr, i64 } %t1, i64 4, 1
  %t3 = getelementptr inbounds [4 x i32], ptr %local.0, i64 0, i64 1
  %t4 = insertvalue { ptr, i64 } poison, ptr %t3, 0
  %t5 = insertvalue { ptr, i64 } %t4, i64 2, 1
  %t6 = extractvalue { ptr, i64 } %t2, 0
  %t7 = getelementptr inbounds i32, ptr %t6, i32 0
  %t8 = load i32, ptr %t7
  %t9 = extractvalue { ptr, i64 } %t5, 0
  %t10 = getelementptr inbounds i32, ptr %t9, i32 0
  %t11 = load i32, ptr %t10
  %t12 = add i32 %t8, %t11
  ret i32 %t12
}
@$main = alias i32 (), ptr @fn.0
