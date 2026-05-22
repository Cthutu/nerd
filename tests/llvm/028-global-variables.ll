base := 40

main :: fn() {
    base = base + 1
    extra: i32
    extra = 1
    return base + extra
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [40 x i8] c"tests/llvm/028-global-variables.input.n\00"

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
