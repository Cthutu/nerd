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
bind a = value.0
bind ab = value.1
bind abc = value.2
bind abcde = value.3
bind main = fn.0
const value.0: u8 = u8 97
const value.1: u16 = u16 24930
const value.2: u32 = u32 6382179
const value.3: u64 = u64 418262508645
func fn.0() -> i32 {
  let ok: bool = bool logical_and(bool logical_and(bool logical_and(bool equal(u8 bind.5(a), u8 97), bool equal(u16 bind.6(ab), u16 24930)), bool equal(u32 bind.7(abc), u32 6382179)), bool equal(u64 bind.8(abcde), u64 418262508645))
  let upper: bool = bool on u8 81 {
    range_inclusive(u8 65, u8 90) => {
      expr bool yes
    }
    else => {
      expr bool no
    }
  }
  let lower: bool = bool on u8 113 {
    range_inclusive(u8 65, u8 90) => {
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

declare void @$pr({ ptr, i64 })
declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

define i32 @fn.0() {
  %t0 = icmp eq i8 97, 97
  %t1 = icmp eq i16 24930, 24930
  %t2 = and i1 %t0, %t1
  %t3 = icmp eq i32 6382179, 6382179
  %t4 = and i1 %t2, %t3
  %t5 = icmp eq i64 418262508645, 418262508645
  %t6 = and i1 %t4, %t5
  %t7 = icmp sge i8 81, 65
  %t8 = icmp sle i8 81, 90
  %t9 = and i1 %t7, %t8
  br i1 %t9, label %on.body.1, label %on.next.2
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
  %t10 = phi i1 [1, %on.value.3], [0, %on.value.6]
  %t11 = icmp sge i8 113, 65
  %t12 = icmp sle i8 113, 90
  %t13 = and i1 %t11, %t12
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
  %t14 = phi i1 [1, %on.value.10], [0, %on.value.13]
  %t15 = and i1 %t6, %t10
  %t16 = xor i1 %t14, 1
  %t17 = and i1 %t15, %t16
  %t18 = icmp eq i1 %t17, 1
  br i1 %t18, label %on.body.15, label %on.next.16
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
  %t19 = phi i32 [0, %on.value.17], [1, %on.value.20]
  ret i32 %t19
}

@$main = alias i32 (), ptr @fn.0
