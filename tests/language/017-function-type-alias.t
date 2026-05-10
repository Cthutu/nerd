-- Reuses a function type alias for the main binding.
MainFn :: fn () -> i32
main: MainFn: fn () => 7
¬
7
¬

¬
hir 0
bind MainFn = type.0
bind main = fn.0
type type.0 = fn () -> i32
func fn.0() -> i32 {
  return untyped integer 7
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0() {
  ret i32 7
}

@$main = alias i32 (), ptr @fn.0
