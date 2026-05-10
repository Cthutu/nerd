-- Compiles an unannotated top-level float literal as f64 storage.
f :: 3.14

main :: fn () => 0
¬
0
¬

¬
hir 0
bind f = value.0
bind main = fn.0
const value.0: untyped float = untyped float 3.1400000000000001
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
