main :: fn() -> i32 {
    values := [1, 2, 3, 4]
    all: []i32 = values[..]
    middle: []i32 = values[1..3]
    return all[0] + middle[0]
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [30 x i8] c"tests/llvm/013-slices.input.n\00"

define internal i32 @fn.0() {
  %local.0 = alloca [4 x i32]
  %t0 = insertvalue [4 x i32] poison, i32 1, 0
  %t1 = insertvalue [4 x i32] %t0, i32 2, 1
  %t2 = insertvalue [4 x i32] %t1, i32 3, 2
  %t3 = insertvalue [4 x i32] %t2, i32 4, 3
  store [4 x i32] %t3, ptr %local.0
  %t4 = getelementptr inbounds [4 x i32], ptr %local.0, i64 0, i64 0
  %t5 = insertvalue { ptr, i64 } poison, ptr %t4, 0
  %t6 = insertvalue { ptr, i64 } %t5, i64 4, 1
  %t7 = getelementptr inbounds [4 x i32], ptr %local.0, i64 0, i64 1
  %t8 = insertvalue { ptr, i64 } poison, ptr %t7, 0
  %t9 = insertvalue { ptr, i64 } %t8, i64 2, 1
  %t10 = extractvalue { ptr, i64 } %t6, 0
  %t11 = getelementptr inbounds i32, ptr %t10, i32 0
  %t12 = load i32, ptr %t11
  %t13 = extractvalue { ptr, i64 } %t9, 0
  %t14 = getelementptr inbounds i32, ptr %t13, i32 0
  %t15 = load i32, ptr %t14
  %t16 = add i32 %t12, %t15
  ret i32 %t16
}

@$main = alias i32 (), ptr @fn.0
