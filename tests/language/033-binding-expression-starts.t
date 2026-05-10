-- Accepts every expression-start token handled by binding classification.
floaty :: 3.14
negative :: -1
truth :: yes
inverted :: !no

main :: fn () => 0
¬
0
¬

¬
hir 0
bind floaty = value.0
bind negative = value.1
bind truth = value.2
bind inverted = value.3
bind main = fn.0
const value.0: untyped float = untyped float 3.1400000000000001
const value.1: untyped integer = untyped integer negate(untyped integer 1)
const value.2: bool = bool yes
const value.3: bool = bool logical_not(bool no)
func fn.0() -> i32 {
  return untyped integer 0
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0() {
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
