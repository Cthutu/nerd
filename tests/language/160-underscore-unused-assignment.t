main :: fn () -> i32 {
    _value := 1
    _value = 2
    return 0
}
¬
0
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  let _value: i32 = untyped integer 1
  assign i32 local.0(_value) = i32 2
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
¬

