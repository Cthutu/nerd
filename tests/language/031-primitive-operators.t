use std.io

-- Checks float literals plus comparison, logical, bitwise, and modulo operators.
half: f32 = 0.5
limit: f64 = 2.0

main :: fn () {
    mask: u32 = 14
    ordered := 3 < 4 && 4 <= 4 && 5 > 4 && 5 >= 5
    prn(on (1.5 < limit && !no) => "float" else "bad")
    prn(on ordered => "cmp" else "bad")
    prn(on (5 % 2 == 1) => "mod" else "bad")
    prn(on (((mask & 11) ^ 3) == 9 || no) => "bits" else "bad")
    return on (half <= 0.5 && 3 != 4) => 1 else 0
}
¬
1
¬
float
cmp
mod
bits

¬
hir 0
module module.0(031-primitive-operators.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind half = value.0
bind limit = value.1
bind main = fn.0
global value.0: f32 = f32 0.5
global value.1: f64 = f64 2
func fn.0() -> i32 {
  let mask: u32 = u32 14
  let ordered: bool = bool logical_and(bool logical_and(bool logical_and(bool less(untyped integer 3, untyped integer 4), bool less_equal(untyped integer 4, untyped integer 4)), bool greater(untyped integer 5, untyped integer 4)), bool greater_equal(untyped integer 5, untyped integer 5))
  expr void call bind.0(prn)(string on bool logical_and(bool less(untyped float 1.5, f64 bind.3(limit)), bool logical_not(bool no)) {
    value(bool yes) => {
      expr string "float"
    }
    else => {
      expr string "bad"
    }
  })
  expr void call bind.0(prn)(string on bool local.1(ordered) {
    value(bool yes) => {
      expr string "cmp"
    }
    else => {
      expr string "bad"
    }
  })
  expr void call bind.0(prn)(string on bool equal(untyped integer modulo(untyped integer 5, untyped integer 2), untyped integer 1) {
    value(bool yes) => {
      expr string "mod"
    }
    else => {
      expr string "bad"
    }
  })
  expr void call bind.0(prn)(string on bool logical_or(bool equal(u32 bitwise_xor(u32 bitwise_and(u32 local.0(mask), u32 11), u32 3), u32 9), bool no) {
    value(bool yes) => {
      expr string "bits"
    }
    else => {
      expr string "bad"
    }
  })
  return untyped integer on bool logical_and(bool less_equal(f32 bind.2(half), f32 0.5), bool not_equal(untyped integer 3, untyped integer 4)) {
    value(bool yes) => {
      expr untyped integer 1
    }
    else => {
      expr untyped integer 0
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [41 x i8] c"tests/language/031-primitive-operators.t\00"
@.str.m0.0 = private unnamed_addr constant [6 x i8] c"float\00"
@.str.m0.1 = private unnamed_addr constant [4 x i8] c"bad\00"
@.str.m0.2 = private unnamed_addr constant [4 x i8] c"cmp\00"
@.str.m0.3 = private unnamed_addr constant [4 x i8] c"bad\00"
@.str.m0.4 = private unnamed_addr constant [4 x i8] c"mod\00"
@.str.m0.5 = private unnamed_addr constant [4 x i8] c"bad\00"
@.str.m0.6 = private unnamed_addr constant [5 x i8] c"bits\00"
@.str.m0.7 = private unnamed_addr constant [4 x i8] c"bad\00"

declare void @$prn({ ptr, i64 })
declare ptr @$input({ ptr, i64 })

@$half = internal global float 0.000000e+00
@$limit = internal global double 0.000000e+00

define void @m0.init() {
  store float 0x3FE0000000000000, ptr @$half
  store double 0x4000000000000000, ptr @$limit
  ret void
}

define internal i32 @fn.0() {
  %t0 = icmp slt i32 3, 4
  %t1 = alloca i1
  br i1 %t0, label %logical.rhs.0, label %logical.short.1
logical.short.1:
  store i1 0, ptr %t1
  br label %logical.end.2
logical.rhs.0:
  %t3 = icmp sle i32 4, 4
  store i1 %t3, ptr %t1
  br label %logical.end.2
logical.end.2:
  %t2 = load i1, ptr %t1
  %t4 = alloca i1
  br i1 %t2, label %logical.rhs.3, label %logical.short.4
logical.short.4:
  store i1 0, ptr %t4
  br label %logical.end.5
logical.rhs.3:
  %t6 = icmp sgt i32 5, 4
  store i1 %t6, ptr %t4
  br label %logical.end.5
logical.end.5:
  %t5 = load i1, ptr %t4
  %t7 = alloca i1
  br i1 %t5, label %logical.rhs.6, label %logical.short.7
logical.short.7:
  store i1 0, ptr %t7
  br label %logical.end.8
logical.rhs.6:
  %t9 = icmp sge i32 5, 5
  store i1 %t9, ptr %t7
  br label %logical.end.8
logical.end.8:
  %t8 = load i1, ptr %t7
  %t10 = load double, ptr @$limit
  %t11 = fcmp olt double 0x3FF8000000000000, %t10
  %t12 = alloca i1
  br i1 %t11, label %logical.rhs.9, label %logical.short.10
logical.short.10:
  store i1 0, ptr %t12
  br label %logical.end.11
logical.rhs.9:
  %t14 = xor i1 0, 1
  store i1 %t14, ptr %t12
  br label %logical.end.11
logical.end.11:
  %t13 = load i1, ptr %t12
  %t15 = icmp eq i1 %t13, 1
  br i1 %t15, label %on.body.13, label %on.next.14
on.body.13:
  br label %on.value.15
on.value.15:
  br label %on.end.12
on.next.14:
  br label %on.body.16
on.body.16:
  br label %on.value.18
on.value.18:
  br label %on.end.12
on.end.12:
  %t16 = phi { ptr, i64 } [{ ptr @.str.m0.0, i64 5 }, %on.value.15], [{ ptr @.str.m0.1, i64 3 }, %on.value.18]
  call void @$prn({ ptr, i64 } %t16)
  %t17 = icmp eq i1 %t8, 1
  br i1 %t17, label %on.body.20, label %on.next.21
on.body.20:
  br label %on.value.22
on.value.22:
  br label %on.end.19
on.next.21:
  br label %on.body.23
on.body.23:
  br label %on.value.25
on.value.25:
  br label %on.end.19
on.end.19:
  %t18 = phi { ptr, i64 } [{ ptr @.str.m0.2, i64 3 }, %on.value.22], [{ ptr @.str.m0.3, i64 3 }, %on.value.25]
  call void @$prn({ ptr, i64 } %t18)
  %t19 = srem i32 5, 2
  %t20 = icmp eq i32 %t19, 1
  %t21 = icmp eq i1 %t20, 1
  br i1 %t21, label %on.body.27, label %on.next.28
on.body.27:
  br label %on.value.29
on.value.29:
  br label %on.end.26
on.next.28:
  br label %on.body.30
on.body.30:
  br label %on.value.32
on.value.32:
  br label %on.end.26
on.end.26:
  %t22 = phi { ptr, i64 } [{ ptr @.str.m0.4, i64 3 }, %on.value.29], [{ ptr @.str.m0.5, i64 3 }, %on.value.32]
  call void @$prn({ ptr, i64 } %t22)
  %t23 = and i32 14, 11
  %t24 = xor i32 %t23, 3
  %t25 = icmp eq i32 %t24, 9
  %t26 = alloca i1
  br i1 %t25, label %logical.short.34, label %logical.rhs.33
logical.short.34:
  store i1 1, ptr %t26
  br label %logical.end.35
logical.rhs.33:
  store i1 0, ptr %t26
  br label %logical.end.35
logical.end.35:
  %t27 = load i1, ptr %t26
  %t28 = icmp eq i1 %t27, 1
  br i1 %t28, label %on.body.37, label %on.next.38
on.body.37:
  br label %on.value.39
on.value.39:
  br label %on.end.36
on.next.38:
  br label %on.body.40
on.body.40:
  br label %on.value.42
on.value.42:
  br label %on.end.36
on.end.36:
  %t29 = phi { ptr, i64 } [{ ptr @.str.m0.6, i64 4 }, %on.value.39], [{ ptr @.str.m0.7, i64 3 }, %on.value.42]
  call void @$prn({ ptr, i64 } %t29)
  %t30 = load float, ptr @$half
  %t31 = fcmp ole float %t30, 0x3FE0000000000000
  %t32 = alloca i1
  br i1 %t31, label %logical.rhs.43, label %logical.short.44
logical.short.44:
  store i1 0, ptr %t32
  br label %logical.end.45
logical.rhs.43:
  %t34 = icmp ne i32 3, 4
  store i1 %t34, ptr %t32
  br label %logical.end.45
logical.end.45:
  %t33 = load i1, ptr %t32
  %t35 = icmp eq i1 %t33, 1
  br i1 %t35, label %on.body.47, label %on.next.48
on.body.47:
  br label %on.value.49
on.value.49:
  br label %on.end.46
on.next.48:
  br label %on.body.50
on.body.50:
  br label %on.value.52
on.value.52:
  br label %on.end.46
on.end.46:
  %t36 = phi i32 [1, %on.value.49], [0, %on.value.52]
  ret i32 %t36
}

@$main = alias i32 (), ptr @fn.0

