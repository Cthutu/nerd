-- Adds two integer literals in an expression-bodied main.
main :: fn () => 123 + 44
¬
167
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  return untyped integer add(untyped integer 123, untyped integer 44)
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %t0 = add i32 123, 44
  ret i32 %t0
}

@$main = alias i32 (), ptr @fn.0
