use std.io

a :: 'a'
ab :: 'ab'
abc :: 'abc'
abcde :: 'abcde'

main :: fn () -> i32 {
    ok := a == 97 && ab == 24930 && abc == 6382179 && abcde == 418262508645
    upper := on 'Q' {
        'A' ..= 'Z' => yes
        else => no
    }
    lower := on 'q' {
        'A' ..= 'Z' => yes
        else => no
    }

    return on ok && upper && !lower => 0 else 1
}
¬
0
¬

¬
hir 0
module module.0(075-packed-integer-literals.input)
import module.1(std.io)
import import.0 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind input = import.0
bind a = value.0
bind ab = value.1
bind abc = value.2
bind abcde = value.3
bind main = fn.0
const value.0: untyped integer = untyped integer 97
const value.1: u16 = u16 24930
const value.2: u32 = u32 6382179
const value.3: u64 = u64 418262508645
func fn.0() -> i32 {
  let ok: bool = bool logical_and(bool logical_and(bool logical_and(bool equal(untyped integer bind.1(a), untyped integer 97), bool equal(u16 bind.2(ab), u16 24930)), bool equal(u32 bind.3(abc), u32 6382179)), bool equal(u64 bind.4(abcde), u64 418262508645))
  let upper: bool = bool on i32 81 {
    range_inclusive(i32 65, i32 90) => {
      expr bool yes
    }
    else => {
      expr bool no
    }
  }
  let lower: bool = bool on i32 113 {
    range_inclusive(i32 65, i32 90) => {
      expr bool yes
    }
    else => {
      expr bool no
    }
  }
  return i32 on bool logical_and(bool logical_and(bool local.0(ok), bool local.1(upper)), bool logical_not(bool local.2(lower))) {
    value(bool yes) => {
      expr i32 0
    }
    else => {
      expr i32 1
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [45 x i8] c"tests/language/075-packed-integer-literals.t\00"

declare ptr @$input({ ptr, i64 })

define internal i32 @fn.0() {
  %t0 = icmp eq i32 97, 97
  %t1 = alloca i1
  br i1 %t0, label %logical.rhs.0, label %logical.short.1
logical.short.1:
  store i1 0, ptr %t1
  br label %logical.end.2
logical.rhs.0:
  %t3 = icmp eq i16 24930, 24930
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
  %t6 = icmp eq i32 6382179, 6382179
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
  %t9 = icmp eq i64 418262508645, 418262508645
  store i1 %t9, ptr %t7
  br label %logical.end.8
logical.end.8:
  %t8 = load i1, ptr %t7
  %t10 = icmp sge i32 81, 65
  %t11 = icmp sle i32 81, 90
  %t12 = and i1 %t10, %t11
  br i1 %t12, label %on.body.10, label %on.next.11
on.body.10:
  br label %on.value.12
on.value.12:
  br label %on.end.9
on.next.11:
  br label %on.body.13
on.body.13:
  br label %on.value.15
on.value.15:
  br label %on.end.9
on.end.9:
  %t13 = phi i1 [1, %on.value.12], [0, %on.value.15]
  %t14 = icmp sge i32 113, 65
  %t15 = icmp sle i32 113, 90
  %t16 = and i1 %t14, %t15
  br i1 %t16, label %on.body.17, label %on.next.18
on.body.17:
  br label %on.value.19
on.value.19:
  br label %on.end.16
on.next.18:
  br label %on.body.20
on.body.20:
  br label %on.value.22
on.value.22:
  br label %on.end.16
on.end.16:
  %t17 = phi i1 [1, %on.value.19], [0, %on.value.22]
  %t18 = alloca i1
  br i1 %t8, label %logical.rhs.23, label %logical.short.24
logical.short.24:
  store i1 0, ptr %t18
  br label %logical.end.25
logical.rhs.23:
  store i1 %t13, ptr %t18
  br label %logical.end.25
logical.end.25:
  %t19 = load i1, ptr %t18
  %t20 = alloca i1
  br i1 %t19, label %logical.rhs.26, label %logical.short.27
logical.short.27:
  store i1 0, ptr %t20
  br label %logical.end.28
logical.rhs.26:
  %t22 = xor i1 %t17, 1
  store i1 %t22, ptr %t20
  br label %logical.end.28
logical.end.28:
  %t21 = load i1, ptr %t20
  %t23 = icmp eq i1 %t21, 1
  br i1 %t23, label %on.body.30, label %on.next.31
on.body.30:
  br label %on.value.32
on.value.32:
  br label %on.end.29
on.next.31:
  br label %on.body.33
on.body.33:
  br label %on.value.35
on.value.35:
  br label %on.end.29
on.end.29:
  %t24 = phi i32 [0, %on.value.32], [1, %on.value.35]
  ret i32 %t24
}

@$main = alias i32 (), ptr @fn.0

