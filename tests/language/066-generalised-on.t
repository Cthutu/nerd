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
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
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
  expr void call bind.0(prn)(string interpolate(<unknown> "scores ", i32 call bind.2(score)(i32 negate(i32 2)), <unknown> " ", i32 call bind.2(score)(i32 0), <unknown> " ", i32 call bind.2(score)(i32 2), <unknown> " ", i32 call bind.2(score)(i32 7), <unknown> " ", i32 call bind.2(score)(i32 12)))
  expr void call bind.0(prn)(string interpolate(<unknown> "descriptions ", string call bind.3(describe)(i32 negate(i32 2)), <unknown> " ", string call bind.3(describe)(i32 0), <unknown> " ", string call bind.3(describe)(i32 7), <unknown> " ", string call bind.3(describe)(i32 12)))
  expr void call bind.0(prn)(string interpolate(<unknown> "not-five ", i32 call bind.4(not_five)(i32 4), <unknown> " ", i32 call bind.4(not_five)(i32 5)))
  return i32 call bind.2(score)(i32 12)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [36 x i8] c"tests/language/066-generalised-on.t\00"
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

declare i1 @string_eq(ptr, ptr)
declare void @string_builder_reset()
declare i64 @string_builder_mark()
declare void @string_builder_append_string(ptr)
declare void @string_builder_append_byte(i8)
declare void @string_builder_finish(ptr, i64)
declare void @to_string$string(ptr, ptr)
declare void @to_string$bool(ptr, i1)
declare void @to_string$i8(ptr, i8)
declare void @to_string$i16(ptr, i16)
declare void @to_string$i32(ptr, i32)
declare void @to_string$i64(ptr, i64)
declare void @to_string$u8(ptr, i8)
declare void @to_string$u16(ptr, i16)
declare void @to_string$u32(ptr, i32)
declare void @to_string$u64(ptr, i64)
declare void @to_string$isize(ptr, i64)
declare void @to_string$usize(ptr, i64)
declare void @to_string$f32(ptr, float)
declare void @to_string$f64(ptr, double)

declare void @$prn({ ptr, i64 })
declare ptr @$input({ ptr, i64 })

define internal i32 @fn.0(i32 %value) {
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

define internal { ptr, i64 } @fn.1(i32 %value) {
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

define internal i32 @fn.2(i32 %value) {
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

define internal i32 @fn.3() {
  %t2 = alloca { ptr, i64 }
  %t7 = alloca { ptr, i64 }
  %t11 = alloca { ptr, i64 }
  %t15 = alloca { ptr, i64 }
  %t19 = alloca { ptr, i64 }
  %t26 = alloca { ptr, i64 }
  %t30 = alloca { ptr, i64 }
  %t32 = alloca { ptr, i64 }
  %t35 = alloca { ptr, i64 }
  %t37 = alloca { ptr, i64 }
  %t40 = alloca { ptr, i64 }
  %t42 = alloca { ptr, i64 }
  %t45 = alloca { ptr, i64 }
  %t50 = alloca { ptr, i64 }
  %t54 = alloca { ptr, i64 }
  %t0 = call i64 @string_builder_mark()
  %t1 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 7 }, ptr %t2
  call void @to_string$string(ptr %t1, ptr %t2)
  call void @string_builder_append_string(ptr %t1)
  %t3 = sub i32 0, 2
  %t4 = call i32 @fn.0(i32 %t3)
  %t5 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t5, i32 %t4)
  call void @string_builder_append_string(ptr %t5)
  %t6 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.5, i64 1 }, ptr %t7
  call void @to_string$string(ptr %t6, ptr %t7)
  call void @string_builder_append_string(ptr %t6)
  %t8 = call i32 @fn.0(i32 0)
  %t9 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t9, i32 %t8)
  call void @string_builder_append_string(ptr %t9)
  %t10 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.6, i64 1 }, ptr %t11
  call void @to_string$string(ptr %t10, ptr %t11)
  call void @string_builder_append_string(ptr %t10)
  %t12 = call i32 @fn.0(i32 2)
  %t13 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t13, i32 %t12)
  call void @string_builder_append_string(ptr %t13)
  %t14 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.7, i64 1 }, ptr %t15
  call void @to_string$string(ptr %t14, ptr %t15)
  call void @string_builder_append_string(ptr %t14)
  %t16 = call i32 @fn.0(i32 7)
  %t17 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t17, i32 %t16)
  call void @string_builder_append_string(ptr %t17)
  %t18 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.8, i64 1 }, ptr %t19
  call void @to_string$string(ptr %t18, ptr %t19)
  call void @string_builder_append_string(ptr %t18)
  %t20 = call i32 @fn.0(i32 12)
  %t21 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t21, i32 %t20)
  call void @string_builder_append_string(ptr %t21)
  %t22 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t22, i64 %t0)
  %t23 = load { ptr, i64 }, ptr %t22
  call void @$prn({ ptr, i64 } %t23)
  %t24 = call i64 @string_builder_mark()
  %t25 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.9, i64 13 }, ptr %t26
  call void @to_string$string(ptr %t25, ptr %t26)
  call void @string_builder_append_string(ptr %t25)
  %t27 = sub i32 0, 2
  %t28 = call { ptr, i64 } @fn.1(i32 %t27)
  %t29 = alloca { ptr, i64 }
  store { ptr, i64 } %t28, ptr %t30
  call void @to_string$string(ptr %t29, ptr %t30)
  call void @string_builder_append_string(ptr %t29)
  %t31 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.10, i64 1 }, ptr %t32
  call void @to_string$string(ptr %t31, ptr %t32)
  call void @string_builder_append_string(ptr %t31)
  %t33 = call { ptr, i64 } @fn.1(i32 0)
  %t34 = alloca { ptr, i64 }
  store { ptr, i64 } %t33, ptr %t35
  call void @to_string$string(ptr %t34, ptr %t35)
  call void @string_builder_append_string(ptr %t34)
  %t36 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.11, i64 1 }, ptr %t37
  call void @to_string$string(ptr %t36, ptr %t37)
  call void @string_builder_append_string(ptr %t36)
  %t38 = call { ptr, i64 } @fn.1(i32 7)
  %t39 = alloca { ptr, i64 }
  store { ptr, i64 } %t38, ptr %t40
  call void @to_string$string(ptr %t39, ptr %t40)
  call void @string_builder_append_string(ptr %t39)
  %t41 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.12, i64 1 }, ptr %t42
  call void @to_string$string(ptr %t41, ptr %t42)
  call void @string_builder_append_string(ptr %t41)
  %t43 = call { ptr, i64 } @fn.1(i32 12)
  %t44 = alloca { ptr, i64 }
  store { ptr, i64 } %t43, ptr %t45
  call void @to_string$string(ptr %t44, ptr %t45)
  call void @string_builder_append_string(ptr %t44)
  %t46 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t46, i64 %t24)
  %t47 = load { ptr, i64 }, ptr %t46
  call void @$prn({ ptr, i64 } %t47)
  %t48 = call i64 @string_builder_mark()
  %t49 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.13, i64 9 }, ptr %t50
  call void @to_string$string(ptr %t49, ptr %t50)
  call void @string_builder_append_string(ptr %t49)
  %t51 = call i32 @fn.2(i32 4)
  %t52 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t52, i32 %t51)
  call void @string_builder_append_string(ptr %t52)
  %t53 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.14, i64 1 }, ptr %t54
  call void @to_string$string(ptr %t53, ptr %t54)
  call void @string_builder_append_string(ptr %t53)
  %t55 = call i32 @fn.2(i32 5)
  %t56 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t56, i32 %t55)
  call void @string_builder_append_string(ptr %t56)
  %t57 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t57, i64 %t48)
  %t58 = load { ptr, i64 }, ptr %t57
  call void @$prn({ ptr, i64 } %t58)
  %t59 = call i32 @fn.0(i32 12)
  ret i32 %t59
}

@$score = internal alias i32 (i32), ptr @fn.0
@$describe = internal alias { ptr, i64 } (i32), ptr @fn.1
@$not_five = internal alias i32 (i32), ptr @fn.2
@$main = alias i32 (), ptr @fn.3
