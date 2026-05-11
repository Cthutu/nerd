-- Declares primitive variables with explicit types and casts.
greeting: string = "Hello"
enabled: bool = yes
ratio: f32 = 42.as(f32)
weight: f64

main :: fn () => enabled.as(i32)
¬
1
¬

¬
hir 0
bind greeting = value.0
bind enabled = value.1
bind ratio = value.2
bind weight = value.3
bind main = fn.0
global value.0: string = string "Hello"
global value.1: bool = bool yes
global value.2: f32 = f32 cast(untyped integer 42 as f32)
global value.3: f64
func fn.0() -> i32 {
  return i32 cast(bool bind.1(enabled) as i32)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [6 x i8] c"Hello\00"

@$greeting = internal global { ptr, i64 } zeroinitializer
@$enabled = internal global i1 0
@$ratio = internal global float 0.000000e+00
@$weight = internal global double 0.000000e+00

define void @m0.init() {
  store { ptr, i64 } { ptr @.str.m0.0, i64 5 }, ptr @$greeting
  store i1 1, ptr @$enabled
  %t0 = sitofp i32 42 to float
  store float %t0, ptr @$ratio
  ret void
}

define internal i32 @fn.0() {
  %t0 = load i1, ptr @$enabled
  %t1 = zext i1 %t0 to i32
  ret i32 %t1
}

@$main = alias i32 (), ptr @fn.0
