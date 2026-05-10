base := 40

main :: fn() {
    base = base + 1
    extra: i32
    extra = 1
    return base + extra
}
¬
; nerd llvm-ir 0
@$base = global i32 0
define void @init() {
  store i32 40, ptr @$base
  ret void
}
define i32 @fn.0() {
  %t0 = load i32, ptr @$base
  %t1 = add i32 %t0, 1
  store i32 %t1, ptr @$base
  %local.0 = alloca i32
  store i32 0, ptr %local.0
  store i32 1, ptr %local.0
  %t2 = load i32, ptr @$base
  %t3 = load i32, ptr %local.0
  %t4 = add i32 %t2, %t3
  ret i32 %t4
}
@$main = alias i32 (), ptr @fn.0
