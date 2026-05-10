-- Casts between i32 and f32, including float-to-integer truncation toward zero.
main :: fn () -> i32 {
    up: f32 = 42.as(f32)
    down: f32 = 3.9
    below: f32 = -3.9

    whole := down.as(i32)
    negative := below.as(i32)
    return up.as(i32) + whole + negative
}
¬
42
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  let up: f32 = f32 cast(untyped integer 42 as f32)
  let down: f32 = f32 3.8999999999999999
  let below: f32 = f32 negate(f32 3.8999999999999999)
  let whole: i32 = i32 cast(f32 local.1(down) as i32)
  let negative: i32 = i32 cast(f32 local.2(below) as i32)
  return i32 add(i32 add(i32 cast(f32 local.0(up) as i32), i32 local.3(whole)), i32 local.4(negative))
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0() {
  %t0 = sitofp i32 42 to float
  %t1 = fneg float 0x400F333340000000
  %t2 = fptosi float 0x400F333340000000 to i32
  %t3 = fptosi float %t1 to i32
  %t4 = fptosi float %t0 to i32
  %t5 = add i32 %t4, %t2
  %t6 = add i32 %t5, %t3
  ret i32 %t6
}

@$main = alias i32 (), ptr @fn.0
