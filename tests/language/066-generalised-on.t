use std.io

score :: fn (value: i32) -> i32 {
    return on value {
        < 0 => -1
        == 0 => 0
        1, 2 => 10
        >= 10 => 100
        else => 20
    }
}

describe :: fn (value: i32) -> string {
    return on {
        value < 0 => "negative"
        value == 0 => "zero"
        value < 10 => "small"
        else => "large"
    }
}

not_five :: fn (value: i32) -> i32 {
    return on value {
        != 5 => 1
        else => 0
    }
}

main :: fn () -> i32 {
    prn($"scores {score(-2)} {score(0)} {score(2)} {score(7)} {score(12)}")
    prn($"descriptions {describe(-2)} {describe(0)} {describe(7)} {describe(12)}")
    prn($"not-five {not_five(4)} {not_five(5)}")
    return score(12)
}
¬
100
¬
scores -1 0 10 20 100
descriptions negative zero small large
not-five 1 0

¬
hir 0
module module.0(066-generalised-on.input)
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
bind score = fn.0
bind describe = fn.1
bind not_five = fn.2
bind main = fn.3
func fn.0(value: i32) -> i32 {
  return i32 on i32 local.0(value) {
    less(i32 0) => {
      expr i32 negate(i32 1)
    }
    equal(i32 0) => {
      expr i32 0
    }
    value(i32 1), value(i32 2) => {
      expr i32 10
    }
    greater_equal(i32 10) => {
      expr i32 100
    }
    else => {
      expr i32 20
    }
  }
}
func fn.1(value: i32) -> string {
  return string on condition {
    bool less(i32 local.1(value), i32 0) => {
      expr string "negative"
    }
    bool equal(i32 local.1(value), i32 0) => {
      expr string "zero"
    }
    bool less(i32 local.1(value), i32 10) => {
      expr string "small"
    }
    else => {
      expr string "large"
    }
  }
}
func fn.2(value: i32) -> i32 {
  return i32 on i32 local.2(value) {
    not_equal(i32 5) => {
      expr i32 1
    }
    else => {
      expr i32 0
    }
  }
}
func fn.3() -> i32 {
  expr void call bind.2(prn)(string interpolate(<unknown> "scores ", i32 call bind.5(score)(i32 negate(i32 2)), <unknown> " ", i32 call bind.5(score)(i32 0), <unknown> " ", i32 call bind.5(score)(i32 2), <unknown> " ", i32 call bind.5(score)(i32 7), <unknown> " ", i32 call bind.5(score)(i32 12)))
  expr void call bind.2(prn)(string interpolate(<unknown> "descriptions ", string call bind.6(describe)(i32 negate(i32 2)), <unknown> " ", string call bind.6(describe)(i32 0), <unknown> " ", string call bind.6(describe)(i32 7), <unknown> " ", string call bind.6(describe)(i32 12)))
  expr void call bind.2(prn)(string interpolate(<unknown> "not-five ", i32 call bind.7(not_five)(i32 4), <unknown> " ", i32 call bind.7(not_five)(i32 5)))
  return i32 call bind.5(score)(i32 12)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [9 x i8] c"negative\00"
@.str.m0.1 = private unnamed_addr constant [5 x i8] c"zero\00"
@.str.m0.2 = private unnamed_addr constant [6 x i8] c"small\00"
@.str.m0.3 = private unnamed_addr constant [6 x i8] c"large\00"
@.str.m0.4 = private unnamed_addr constant [8 x i8] c"scores \00"
@.str.m0.5 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.6 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.7 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.8 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.9 = private unnamed_addr constant [14 x i8] c"descriptions \00"
@.str.m0.10 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.11 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.12 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.13 = private unnamed_addr constant [10 x i8] c"not-five \00"
@.str.m0.14 = private unnamed_addr constant [2 x i8] c" \00"

declare i1 @string_eq({ ptr, i64 }, { ptr, i64 })
declare void @string_builder_reset()
declare i64 @string_builder_mark()
declare void @string_builder_append_string({ ptr, i64 })
declare void @string_builder_append_byte(i8)
declare { ptr, i64 } @string_builder_finish(i64)
declare { ptr, i64 } @to_string$string({ ptr, i64 })
declare { ptr, i64 } @to_string$bool(i1)
declare { ptr, i64 } @to_string$i8(i8)
declare { ptr, i64 } @to_string$i16(i16)
declare { ptr, i64 } @to_string$i32(i32)
declare { ptr, i64 } @to_string$i64(i64)
declare { ptr, i64 } @to_string$u8(i8)
declare { ptr, i64 } @to_string$u16(i16)
declare { ptr, i64 } @to_string$u32(i32)
declare { ptr, i64 } @to_string$u64(i64)
declare { ptr, i64 } @to_string$isize(i64)
declare { ptr, i64 } @to_string$usize(i64)
declare { ptr, i64 } @to_string$f32(float)
declare { ptr, i64 } @to_string$f64(double)

declare void @$pr({ ptr, i64 })
declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

define i32 @fn.0(i32 %value) {
  %t0 = icmp slt i32 %value, 0
  br i1 %t0, label %on.body.1, label %on.next.2
on.body.1:
  %t1 = sub i32 0, 1
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t2 = icmp eq i32 %value, 0
  br i1 %t2, label %on.body.4, label %on.next.5
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  %t3 = icmp eq i32 %value, 1
  %t4 = icmp eq i32 %value, 2
  %t5 = or i1 %t3, %t4
  br i1 %t5, label %on.body.7, label %on.next.8
on.body.7:
  br label %on.value.9
on.value.9:
  br label %on.end.0
on.next.8:
  %t6 = icmp sge i32 %value, 10
  br i1 %t6, label %on.body.10, label %on.next.11
on.body.10:
  br label %on.value.12
on.value.12:
  br label %on.end.0
on.next.11:
  br label %on.body.13
on.body.13:
  br label %on.value.15
on.value.15:
  br label %on.end.0
on.end.0:
  %t7 = phi i32 [%t1, %on.value.3], [0, %on.value.6], [10, %on.value.9], [100, %on.value.12], [20, %on.value.15]
  ret i32 %t7
}

define { ptr, i64 } @fn.1(i32 %value) {
  %t0 = icmp slt i32 %value, 0
  br i1 %t0, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t1 = icmp eq i32 %value, 0
  br i1 %t1, label %on.body.4, label %on.next.5
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  %t2 = icmp slt i32 %value, 10
  br i1 %t2, label %on.body.7, label %on.next.8
on.body.7:
  br label %on.value.9
on.value.9:
  br label %on.end.0
on.next.8:
  br label %on.body.10
on.body.10:
  br label %on.value.12
on.value.12:
  br label %on.end.0
on.end.0:
  %t3 = phi { ptr, i64 } [{ ptr @.str.m0.0, i64 8 }, %on.value.3], [{ ptr @.str.m0.1, i64 4 }, %on.value.6], [{ ptr @.str.m0.2, i64 5 }, %on.value.9], [{ ptr @.str.m0.3, i64 5 }, %on.value.12]
  ret { ptr, i64 } %t3
}

define i32 @fn.2(i32 %value) {
  %t0 = icmp ne i32 %value, 5
  br i1 %t0, label %on.body.1, label %on.next.2
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
  %t1 = phi i32 [1, %on.value.3], [0, %on.value.6]
  ret i32 %t1
}

define i32 @fn.3() {
  %t0 = call i64 @string_builder_mark()
  %t1 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.4, i64 7 })
  call void @string_builder_append_string({ ptr, i64 } %t1)
  %t2 = sub i32 0, 2
  %t3 = call i32 @fn.0(i32 %t2)
  %t4 = call { ptr, i64 } @to_string$i32(i32 %t3)
  call void @string_builder_append_string({ ptr, i64 } %t4)
  %t5 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.5, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t5)
  %t6 = call i32 @fn.0(i32 0)
  %t7 = call { ptr, i64 } @to_string$i32(i32 %t6)
  call void @string_builder_append_string({ ptr, i64 } %t7)
  %t8 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.6, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t8)
  %t9 = call i32 @fn.0(i32 2)
  %t10 = call { ptr, i64 } @to_string$i32(i32 %t9)
  call void @string_builder_append_string({ ptr, i64 } %t10)
  %t11 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.7, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t11)
  %t12 = call i32 @fn.0(i32 7)
  %t13 = call { ptr, i64 } @to_string$i32(i32 %t12)
  call void @string_builder_append_string({ ptr, i64 } %t13)
  %t14 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.8, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t14)
  %t15 = call i32 @fn.0(i32 12)
  %t16 = call { ptr, i64 } @to_string$i32(i32 %t15)
  call void @string_builder_append_string({ ptr, i64 } %t16)
  %t17 = call { ptr, i64 } @string_builder_finish(i64 %t0)
  call void @$prn({ ptr, i64 } %t17)
  %t18 = call i64 @string_builder_mark()
  %t19 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.9, i64 13 })
  call void @string_builder_append_string({ ptr, i64 } %t19)
  %t20 = sub i32 0, 2
  %t21 = call { ptr, i64 } @fn.1(i32 %t20)
  %t22 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t21)
  call void @string_builder_append_string({ ptr, i64 } %t22)
  %t23 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.10, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t23)
  %t24 = call { ptr, i64 } @fn.1(i32 0)
  %t25 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t24)
  call void @string_builder_append_string({ ptr, i64 } %t25)
  %t26 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.11, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t26)
  %t27 = call { ptr, i64 } @fn.1(i32 7)
  %t28 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t27)
  call void @string_builder_append_string({ ptr, i64 } %t28)
  %t29 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.12, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t29)
  %t30 = call { ptr, i64 } @fn.1(i32 12)
  %t31 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t30)
  call void @string_builder_append_string({ ptr, i64 } %t31)
  %t32 = call { ptr, i64 } @string_builder_finish(i64 %t18)
  call void @$prn({ ptr, i64 } %t32)
  %t33 = call i64 @string_builder_mark()
  %t34 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.13, i64 9 })
  call void @string_builder_append_string({ ptr, i64 } %t34)
  %t35 = call i32 @fn.2(i32 4)
  %t36 = call { ptr, i64 } @to_string$i32(i32 %t35)
  call void @string_builder_append_string({ ptr, i64 } %t36)
  %t37 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.14, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t37)
  %t38 = call i32 @fn.2(i32 5)
  %t39 = call { ptr, i64 } @to_string$i32(i32 %t38)
  call void @string_builder_append_string({ ptr, i64 } %t39)
  %t40 = call { ptr, i64 } @string_builder_finish(i64 %t33)
  call void @$prn({ ptr, i64 } %t40)
  %t41 = call i32 @fn.0(i32 12)
  ret i32 %t41
}

@$score = internal alias i32 (i32), ptr @fn.0
@$describe = internal alias { ptr, i64 } (i32), ptr @fn.1
@$not_five = internal alias i32 (i32), ptr @fn.2
@$main = alias i32 (), ptr @fn.3
