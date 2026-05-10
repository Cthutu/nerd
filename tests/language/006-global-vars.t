-- Uses top-level constant bindings from main.
answer :: 42
magic_number :: 7

main :: fn () => answer / magic_number
¬
6
¬

¬
hir 0
bind answer = value.0
bind magic_number = value.1
bind main = fn.0
const value.0: untyped integer = untyped integer 42
const value.1: untyped integer = untyped integer 7
func fn.0() -> i32 {
  return untyped integer divide(untyped integer bind.0(answer), untyped integer bind.1(magic_number))
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0() {
  %t0 = sdiv i32 42, 7
  ret i32 %t0
}

@$main = alias i32 (), ptr @fn.0
