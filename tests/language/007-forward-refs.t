-- Resolves a top-level constant through a forward reference.
magic_number :: answer / 6
answer :: 42

main :: fn () => magic_number
¬
7
¬

¬
hir 0
bind answer = value.0
bind magic_number = value.1
bind main = fn.0
const value.0: untyped integer = untyped integer 42
const value.1: untyped integer = untyped integer divide(untyped integer bind.0(answer), untyped integer 6)
func fn.0() -> i32 {
  return untyped integer bind.1(magic_number)
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %t0 = sdiv i32 42, 6
  ret i32 %t0
}

@$main = alias i32 (), ptr @fn.0
