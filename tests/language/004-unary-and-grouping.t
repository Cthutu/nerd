-- Checks unary minus combined with grouped subtraction.
main :: fn () => -(2-5)
¬
3
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  return untyped integer negate(untyped integer subtract(untyped integer 2, untyped integer 5))
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %t0 = sub i32 2, 5
  %t1 = sub i32 0, %t0
  ret i32 %t1
}

@$main = alias i32 (), ptr @fn.0
