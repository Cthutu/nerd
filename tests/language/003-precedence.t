-- Checks operator precedence between addition and multiplication.
main :: fn () => 1+2*3
¬
7
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  return untyped integer add(untyped integer 1, untyped integer multiply(untyped integer 2, untyped integer 3))
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0() {
  %t0 = mul i32 2, 3
  %t1 = add i32 1, %t0
  ret i32 %t1
}

@$main = alias i32 (), ptr @fn.0
