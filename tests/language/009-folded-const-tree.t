-- Folds a small constant-expression tree across bindings.
left :: 2 + 3
right :: left * (10 - 4)

main :: fn () => right % 7
¬
2
¬

¬
hir 0
bind left = value.0
bind right = value.1
bind main = fn.0
const value.0: untyped integer = untyped integer add(untyped integer 2, untyped integer 3)
const value.1: untyped integer = untyped integer multiply(untyped integer bind.0(left), untyped integer subtract(untyped integer 10, untyped integer 4))
func fn.0() -> i32 {
  return untyped integer modulo(untyped integer bind.1(right), untyped integer 7)
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0() {
  %t0 = add i32 2, 3
  %t1 = sub i32 10, 4
  %t2 = mul i32 %t0, %t1
  %t3 = srem i32 %t2, 7
  ret i32 %t3
}

@$main = alias i32 (), ptr @fn.0
