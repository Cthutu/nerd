-- Checks left-associative division and modulo evaluation.
main :: fn () => 20/3%4
¬
2
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  return untyped integer modulo(untyped integer divide(untyped integer 20, untyped integer 3), untyped integer 4)
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %t0 = sdiv i32 20, 3
  %t1 = srem i32 %t0, 4
  ret i32 %t1
}

@$main = alias i32 (), ptr @fn.0
