not_value :: fn(value: bool) -> bool {
    return !value
}

negative :: fn(value: i32) -> i32 {
    return -value
}

main :: fn() -> i32 {
    return 0
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i1 @fn.0(i1 %value) {
  %t0 = xor i1 %value, 1
  ret i1 %t0
}

define internal i32 @fn.1(i32 %value) {
  %t0 = sub i32 0, %value
  ret i32 %t0
}

define internal i32 @fn.2() {
  ret i32 0
}

@$not_value = internal alias i1 (i1), ptr @fn.0
@$negative = internal alias i32 (i32), ptr @fn.1
@$main = alias i32 (), ptr @fn.2
