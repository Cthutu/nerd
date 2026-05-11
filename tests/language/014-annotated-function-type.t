-- Binds main with an explicit function type annotation.
main: fn () -> i32: fn () => 1
¬
1
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  return untyped integer 1
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  ret i32 1
}

@$main = alias i32 (), ptr @fn.0
