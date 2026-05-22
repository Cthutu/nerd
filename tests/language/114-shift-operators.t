-- Supports shift operators and compound shift assignment.
main :: fn () {
    x := 1
    y := 64
    x <<= 3
    y >>= 2
    return x + y + (2 << 4) + (128 >> 5)
}
¬
60
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  let x: i32 = untyped integer 1
  let y: i32 = untyped integer 64
  assign i32 local.0(x) = i32 shift_left(i32 local.0(x), i32 3)
  assign i32 local.1(y) = i32 shift_right(i32 local.1(y), i32 2)
  return i32 add(i32 add(i32 add(i32 local.0(x), i32 local.1(y)), i32 shift_left(i32 2, i32 4)), i32 shift_right(i32 128, i32 5))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [37 x i8] c"tests/language/114-shift-operators.t\00"

define internal i32 @fn.0() {
  %local.0 = alloca i32
  %local.1 = alloca i32
  store i32 1, ptr %local.0
  store i32 64, ptr %local.1
  %t0 = load i32, ptr %local.0
  %t1 = shl i32 %t0, 3
  store i32 %t1, ptr %local.0
  %t2 = load i32, ptr %local.1
  %t3 = ashr i32 %t2, 2
  store i32 %t3, ptr %local.1
  %t4 = load i32, ptr %local.0
  %t5 = load i32, ptr %local.1
  %t6 = add i32 %t4, %t5
  %t7 = shl i32 2, 4
  %t8 = add i32 %t6, %t7
  %t9 = ashr i32 128, 5
  %t10 = add i32 %t8, %t9
  ret i32 %t10
}

@$main = alias i32 (), ptr @fn.0
