-- Folds a chain of dependent top-level constant bindings.
base :: 4
factor :: 5
answer :: base * factor + 2

main :: fn () => answer
¬
22
¬

¬
hir 0
bind base = value.0
bind factor = value.1
bind answer = value.2
bind main = fn.0
const value.0: untyped integer = untyped integer 4
const value.1: untyped integer = untyped integer 5
const value.2: untyped integer = untyped integer add(untyped integer multiply(untyped integer bind.0(base), untyped integer bind.1(factor)), untyped integer 2)
func fn.0() -> i32 {
  return untyped integer bind.2(answer)
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0() {
  %t0 = mul i32 4, 5
  %t1 = add i32 %t0, 2
  ret i32 %t1
}

@$main = alias i32 (), ptr @fn.0
