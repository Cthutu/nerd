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
import import.0 pr from module.1(std.io).decl.9: fn (string) -> void
import import.1 epr from module.1(std.io).decl.10: fn (string) -> void
import import.2 prn from module.1(std.io).decl.11: fn (string) -> void
import import.3 eprn from module.1(std.io).decl.12: fn (string) -> void
import import.4 input from module.1(std.io).decl.13: fn (string) -> string
bind pr = import.0
bind epr = import.1
bind prn = import.2
bind eprn = import.3
bind input = import.4
bind half = value.0
bind limit = value.1
bind main = fn.0
global value.0: f32 = f32 0.5
global value.1: f64 = f64 2
func fn.0() -> i32 {
  let mask: u32 = u32 14
  let ordered: bool = bool logical_and(bool logical_and(bool logical_and(bool less(untyped integer 3, untyped integer 4), bool less_equal(untyped integer 4, untyped integer 4)), bool greater(untyped integer 5, untyped integer 4)), bool greater_equal(untyped integer 5, untyped integer 5))
  expr void call bind.2(prn)(string on bool logical_and(bool less(untyped float 1.5, f64 bind.6(limit)), bool logical_not(bool no)) {
    value(bool yes) => {
      expr string "float"
    }
    else => {
      expr string "bad"
    }
  })
  expr void call bind.2(prn)(string on bool local.1(ordered) {
    value(bool yes) => {
      expr string "cmp"
    }
    else => {
      expr string "bad"
    }
  })
  expr void call bind.2(prn)(string on bool equal(untyped integer modulo(untyped integer 5, untyped integer 2), untyped integer 1) {
    value(bool yes) => {
      expr string "mod"
    }
    else => {
      expr string "bad"
    }
  })
  expr void call bind.2(prn)(string on bool logical_or(bool equal(u32 bitwise_xor(u32 bitwise_and(u32 local.0(mask), u32 11), u32 3), u32 9), bool no) {
    value(bool yes) => {
      expr string "bits"
    }
    else => {
      expr string "bad"
    }
  })
  return untyped integer on bool logical_and(bool less_equal(f32 bind.5(half), f32 0.5), bool not_equal(untyped integer 3, untyped integer 4)) {
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

@.str.m0.0 = private unnamed_addr constant [6 x i8] c"float\00"
@.str.m0.1 = private unnamed_addr constant [4 x i8] c"bad\00"
@.str.m0.2 = private unnamed_addr constant [4 x i8] c"cmp\00"
@.str.m0.3 = private unnamed_addr constant [4 x i8] c"bad\00"
@.str.m0.4 = private unnamed_addr constant [4 x i8] c"mod\00"
@.str.m0.5 = private unnamed_addr constant [4 x i8] c"bad\00"
@.str.m0.6 = private unnamed_addr constant [5 x i8] c"bits\00"
@.str.m0.7 = private unnamed_addr constant [4 x i8] c"bad\00"

declare void @$pr({ ptr, i64 })
declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

@$half = internal global float 0.000000e+00
@$limit = internal global double 0.000000e+00

define void @m0.init() {
  store float 0x3FE0000000000000, ptr @$half
  store double 0x4000000000000000, ptr @$limit
  ret void
}

define i32 @fn.0() {
  %t0 = icmp slt i32 3, 4
  %t1 = icmp sle i32 4, 4
  %t2 = and i1 %t0, %t1
  %t3 = icmp sgt i32 5, 4
  %t4 = and i1 %t2, %t3
  %t5 = icmp sge i32 5, 5
  %t6 = and i1 %t4, %t5
  %t7 = load double, ptr @$limit
  %t8 = fcmp olt double 0x3FF8000000000000, %t7
  %t9 = xor i1 0, 1
  %t10 = and i1 %t8, %t9
  %t11 = icmp eq i1 %t10, 1
  br i1 %t11, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  br label %on.body.4
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.end.0:
  %t12 = phi { ptr, i64 } [{ ptr @.str.m0.0, i64 5 }, %on.value.3], [{ ptr @.str.m0.1, i64 3 }, %on.value.6]
  call void @$prn({ ptr, i64 } %t12)
  %t13 = icmp eq i1 %t6, 1
  br i1 %t13, label %on.body.8, label %on.next.9
on.body.8:
  br label %on.value.10
on.value.10:
  br label %on.end.7
on.next.9:
  br label %on.body.11
on.body.11:
  br label %on.value.13
on.value.13:
  br label %on.end.7
on.end.7:
  %t14 = phi { ptr, i64 } [{ ptr @.str.m0.2, i64 3 }, %on.value.10], [{ ptr @.str.m0.3, i64 3 }, %on.value.13]
  call void @$prn({ ptr, i64 } %t14)
  %t15 = srem i32 5, 2
  %t16 = icmp eq i32 %t15, 1
  %t17 = icmp eq i1 %t16, 1
  br i1 %t17, label %on.body.15, label %on.next.16
on.body.15:
  br label %on.value.17
on.value.17:
  br label %on.end.14
on.next.16:
  br label %on.body.18
on.body.18:
  br label %on.value.20
on.value.20:
  br label %on.end.14
on.end.14:
  %t18 = phi { ptr, i64 } [{ ptr @.str.m0.4, i64 3 }, %on.value.17], [{ ptr @.str.m0.5, i64 3 }, %on.value.20]
  call void @$prn({ ptr, i64 } %t18)
  %t19 = and i32 14, 11
  %t20 = xor i32 %t19, 3
  %t21 = icmp eq i32 %t20, 9
  %t22 = or i1 %t21, 0
  %t23 = icmp eq i1 %t22, 1
  br i1 %t23, label %on.body.22, label %on.next.23
on.body.22:
  br label %on.value.24
on.value.24:
  br label %on.end.21
on.next.23:
  br label %on.body.25
on.body.25:
  br label %on.value.27
on.value.27:
  br label %on.end.21
on.end.21:
  %t24 = phi { ptr, i64 } [{ ptr @.str.m0.6, i64 4 }, %on.value.24], [{ ptr @.str.m0.7, i64 3 }, %on.value.27]
  call void @$prn({ ptr, i64 } %t24)
  %t25 = load float, ptr @$half
  %t26 = fcmp ole float %t25, 0x3FE0000000000000
  %t27 = icmp ne i32 3, 4
  %t28 = and i1 %t26, %t27
  %t29 = icmp eq i1 %t28, 1
  br i1 %t29, label %on.body.29, label %on.next.30
on.body.29:
  br label %on.value.31
on.value.31:
  br label %on.end.28
on.next.30:
  br label %on.body.32
on.body.32:
  br label %on.value.34
on.value.34:
  br label %on.end.28
on.end.28:
  %t30 = phi i32 [1, %on.value.31], [0, %on.value.34]
  ret i32 %t30
}

@$main = alias i32 (), ptr @fn.0
