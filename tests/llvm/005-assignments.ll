main :: fn() -> i32 {
    value := 1
    value = value + 1
    return value
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %local.0 = alloca i32
  store i32 1, ptr %local.0
  %t0 = load i32, ptr %local.0
  %t1 = add i32 %t0, 1
  store i32 %t1, ptr %local.0
  %t2 = load i32, ptr %local.0
  ret i32 %t2
}

@$main = alias i32 (), ptr @fn.0
