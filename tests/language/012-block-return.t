-- Returns a value explicitly from a block-bodied main.
main :: fn () {
    return 42
}
¬
42
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  return untyped integer 42
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  ret i32 42
}

@$main = alias i32 (), ptr @fn.0
