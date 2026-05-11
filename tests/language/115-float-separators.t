-- Decodes underscores in float literals as digit separators.
main :: fn () -> i32 {
    value := 1_0.5 + 3_1.5
    return value.as(i32)
}
¬
42
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  let value: f64 = untyped float add(untyped float 10.5, untyped float 31.5)
  return i32 cast(f64 local.0(value) as i32)
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %t0 = fadd double 0x4025000000000000, 0x403F800000000000
  %t1 = fptosi double %t0 to i32
  ret i32 %t1
}

@$main = alias i32 (), ptr @fn.0
