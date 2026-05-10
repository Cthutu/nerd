addr :: fn(value: i32) -> ^i32 {
    return ^value
}

read :: fn(ptr: ^i32) -> i32 {
    return ptr^
}

first :: fn(values: [2]i32) -> i32 {
    return values[0]
}

main :: fn() -> i32 {
    values: [2]i32 = [1, 2]
    return first(values)
}
¬
; nerd llvm-ir 0
; generated from HIR

define ptr @fn.0(i32 %value) {
  %local.0 = alloca i32
  store i32 %value, ptr %local.0
  ret ptr %local.0
}

define i32 @fn.1(ptr %ptr) {
  %t0 = load i32, ptr %ptr
  ret i32 %t0
}

define i32 @fn.2([2 x i32] %values) {
  %local.2 = alloca [2 x i32]
  store [2 x i32] %values, ptr %local.2
  %t0 = getelementptr inbounds [2 x i32], ptr %local.2, i64 0, i32 0
  %t1 = load i32, ptr %t0
  ret i32 %t1
}

define i32 @fn.3() {
  %t0 = insertvalue [2 x i32] poison, i32 1, 0
  %t1 = insertvalue [2 x i32] %t0, i32 2, 1
  %t2 = call i32 @fn.2([2 x i32] %t1)
  ret i32 %t2
}

@$addr = alias ptr (i32), ptr @fn.0
@$read = alias i32 (ptr), ptr @fn.1
@$first = alias i32 ([2 x i32]), ptr @fn.2
@$main = alias i32 (), ptr @fn.3