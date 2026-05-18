-- Declares primitive variables with explicit types and casts.
greeting: string = "Hello"
enabled: bool = yes
disabled: bool = false
truthy: bool = true
ratio: f32 = 42.as(f32)
weight: f64

main :: fn () => enabled.as(i32) + disabled.as(i32) + truthy.as(i32)
¬
2
¬

¬
hir 0
bind greeting = value.0
bind enabled = value.1
bind disabled = value.2
bind truthy = value.3
bind ratio = value.4
bind weight = value.5
bind main = fn.0
global value.0: string = string "Hello"
global value.1: bool = bool yes
global value.2: bool = bool no
global value.3: bool = bool yes
global value.4: f32 = f32 cast(untyped integer 42 as f32)
global value.5: f64
func fn.0() -> i32 {
  return i32 add(i32 add(i32 cast(bool bind.1(enabled) as i32), i32 cast(bool bind.2(disabled) as i32)), i32 cast(bool bind.3(truthy) as i32))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [6 x i8] c"Hello\00"

@$greeting = internal global { ptr, i64 } zeroinitializer
@$enabled = internal global i1 0
@$disabled = internal global i1 0
@$truthy = internal global i1 0
@$ratio = internal global float 0.000000e+00
@$weight = internal global double 0.000000e+00

define void @m0.init() {
  store { ptr, i64 } { ptr @.str.m0.0, i64 5 }, ptr @$greeting
  store i1 1, ptr @$enabled
  store i1 0, ptr @$disabled
  store i1 1, ptr @$truthy
  %t0 = sitofp i32 42 to float
  store float %t0, ptr @$ratio
  ret void
}

define internal i32 @fn.0() {
  %t0 = load i1, ptr @$enabled
  %t1 = zext i1 %t0 to i32
  %t2 = load i1, ptr @$disabled
  %t3 = zext i1 %t2 to i32
  %t4 = add i32 %t1, %t3
  %t5 = load i1, ptr @$truthy
  %t6 = zext i1 %t5 to i32
  %t7 = add i32 %t4, %t6
  ret i32 %t7
}

@$main = alias i32 (), ptr @fn.0
