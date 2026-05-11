main :: fn () {
    value: i32 = undefined
    value = 42
    return value
}
¬
42
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  expr <unknown> <unsupported>
  let value: i32 = <unknown> <unsupported>
  assign i32 local.0(value) = i32 42
  return i32 local.0(value)
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %local.0 = alloca i32
  store i32 0, ptr %local.0
  store i32 42, ptr %local.0
  %t0 = load i32, ptr %local.0
  ret i32 %t0
}

@$main = alias i32 (), ptr @fn.0
