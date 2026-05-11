-- Declares a top-level constant with an explicit type annotation.
value: i32: 120

main :: fn () => value
¬
120
¬

¬
hir 0
bind value = value.0
bind main = fn.0
const value.0: i32 = i32 120
func fn.0() -> i32 {
  return i32 bind.0(value)
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  ret i32 120
}

@$main = alias i32 (), ptr @fn.0
