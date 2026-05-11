-- Decodes underscores in integer literals as digit separators.
main :: fn () => 1_00 + 0b10_10 + 0o7_5 + 0x1_0
¬
187
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  return untyped integer add(untyped integer add(untyped integer add(untyped integer 100, untyped integer 10), untyped integer 61), untyped integer 16)
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %t0 = add i32 100, 10
  %t1 = add i32 %t0, 61
  %t2 = add i32 %t1, 16
  ret i32 %t2
}

@$main = alias i32 (), ptr @fn.0
