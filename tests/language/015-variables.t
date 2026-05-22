-- Mutates variables and zero-initialises an explicit local variable.
base := 40

main :: fn () {
    base = base + 1
    extra: i32
    extra = 1
    return base + extra
}
¬
42
¬

¬
hir 0
bind base = value.0
bind main = fn.0
global value.0: i32 = untyped integer 40
func fn.0() -> i32 {
  assign i32 bind.0(base) = i32 add(i32 bind.0(base), i32 1)
  expr <unknown> default
  let extra: i32 = <unknown> default
  assign i32 local.0(extra) = i32 1
  return i32 add(i32 bind.0(base), i32 local.0(extra))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [31 x i8] c"tests/language/015-variables.t\00"

@$base = internal global i32 0

define void @m0.init() {
  store i32 40, ptr @$base
  ret void
}

define internal i32 @fn.0() {
  %local.0 = alloca i32
  %t0 = load i32, ptr @$base
  %t1 = add i32 %t0, 1
  store i32 %t1, ptr @$base
  store i32 0, ptr %local.0
  store i32 1, ptr %local.0
  %t2 = load i32, ptr @$base
  %t3 = load i32, ptr %local.0
  %t4 = add i32 %t2, %t3
  ret i32 %t4
}

@$main = alias i32 (), ptr @fn.0
