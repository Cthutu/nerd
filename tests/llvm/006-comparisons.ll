less :: fn(a: i32, b: i32) -> bool {
    return a < b
}

ready :: fn(a: bool, b: bool) -> bool {
    return a && b || yes
}

main :: fn() -> i32 {
    return 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [35 x i8] c"tests/llvm/006-comparisons.input.n\00"

define internal i1 @fn.0(i32 %a, i32 %b) {
  %t0 = icmp slt i32 %a, %b
  ret i1 %t0
}

define internal i1 @fn.1(i1 %a, i1 %b) {
  %t0 = alloca i1
  br i1 %a, label %logical.rhs.0, label %logical.short.1
logical.short.1:
  store i1 0, ptr %t0
  br label %logical.end.2
logical.rhs.0:
  store i1 %b, ptr %t0
  br label %logical.end.2
logical.end.2:
  %t1 = load i1, ptr %t0
  %t2 = alloca i1
  br i1 %t1, label %logical.short.4, label %logical.rhs.3
logical.short.4:
  store i1 1, ptr %t2
  br label %logical.end.5
logical.rhs.3:
  store i1 1, ptr %t2
  br label %logical.end.5
logical.end.5:
  %t3 = load i1, ptr %t2
  ret i1 %t3
}

define internal i32 @fn.2() {
  ret i32 0
}

@$less = internal alias i1 (i32, i32), ptr @fn.0
@$ready = internal alias i1 (i1, i1), ptr @fn.1
@$main = alias i32 (), ptr @fn.2

