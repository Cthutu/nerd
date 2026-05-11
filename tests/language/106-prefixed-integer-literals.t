-- Decodes hexadecimal, binary, and octal integer literal prefixes.
main :: fn () => 0x10 + 0b11 + 0o7
¬
26
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  return untyped integer add(untyped integer add(untyped integer 16, untyped integer 3), untyped integer 7)
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %t0 = add i32 16, 3
  %t1 = add i32 %t0, 7
  ret i32 %t1
}

@$main = alias i32 (), ptr @fn.0
