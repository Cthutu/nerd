use std.io

main :: fn () {
    total := 0
    for i := 0; i < 5; i += 1 $outer {
        for j := 0; j < 5; j += 1 $inner {
            on j == 1 => continue $outer
            total += 100
        }
        total += i
    }

    for $done {
        break $done
    }

    value :: for {
        break 7
    }
    labelled_value :: for $value_loop {
        break $value_loop 11
    }

    prn($"total = {total}")
    prn($"value = {value}")
    prn($"labelled = {labelled_value}")
    return total + value + labelled_value
}
¬
6
¬
total = 500
value = 7
labelled = 11

¬
hir 0
module module.0(049-labelled-loop-expressions.input)
import module.1(std.io)
import import.0 pr from module.1(std.io).decl.6: fn (string) -> void
import import.1 epr from module.1(std.io).decl.7: fn (string) -> void
import import.2 prn from module.1(std.io).decl.8: fn (string) -> void
import import.3 eprn from module.1(std.io).decl.9: fn (string) -> void
import import.4 input from module.1(std.io).decl.10: fn (string) -> string
bind pr = import.0
bind epr = import.1
bind prn = import.2
bind eprn = import.3
bind input = import.4
bind main = fn.0
func fn.0() -> i32 {
  let total: i32 = untyped integer 0
  expr void for c_style $outer {
    init {
      let i: i32 = untyped integer 0
    }
    condition bool less(i32 local.3(i), i32 5)
    body {
      expr void for c_style $inner {
    init {
      let j: i32 = untyped integer 0
    }
    condition bool less(i32 local.4(j), i32 5)
    body {
      expr void on bool equal(i32 local.4(j), i32 1) {
    value(bool yes) => {
      continue $outer
    }
  }
      assign i32 local.2(total) = i32 add(i32 local.2(total), i32 100)
    }
    update {
      assign i32 local.4(j) = i32 add(i32 local.4(j), i32 1)
    }
  }
      assign i32 local.2(total) = i32 add(i32 local.2(total), i32 local.3(i))
    }
    update {
      assign i32 local.3(i) = i32 add(i32 local.3(i), i32 1)
    }
  }
  expr void for condition $done {
    body {
      break $done
    }
  }
  let value: untyped integer = untyped integer for condition {
    body {
      break untyped integer 7
    }
  }
  let labelled_value: untyped integer = untyped integer for condition $value_loop {
    body {
      break $value_loop untyped integer 11
    }
  }
  expr void call bind.2(prn)(string interpolate(<unknown> "total = ", i32 local.2(total)))
  expr void call bind.2(prn)(string interpolate(<unknown> "value = ", untyped integer local.0(value)))
  expr void call bind.2(prn)(string interpolate(<unknown> "labelled = ", untyped integer local.1(labelled_value)))
  return i32 add(i32 add(i32 local.2(total), i32 local.0(value)), i32 local.1(labelled_value))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [9 x i8] c"total = \00"
@.str.m0.1 = private unnamed_addr constant [9 x i8] c"value = \00"
@.str.m0.2 = private unnamed_addr constant [12 x i8] c"labelled = \00"

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

declare void @$pr({ ptr, i64 })
declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

define internal i32 @fn.0() {
  %local.2 = alloca i32
  store i32 0, ptr %local.2
  %local.3 = alloca i32
  store i32 0, ptr %local.3
  br label %for.cond.0
for.cond.0:
  %t0 = load i32, ptr %local.3
  %t1 = icmp slt i32 %t0, 5
  br i1 %t1, label %for.body.1, label %for.end.3
for.body.1:
  %local.4 = alloca i32
  store i32 0, ptr %local.4
  br label %for.cond.4
for.cond.4:
  %t2 = load i32, ptr %local.4
  %t3 = icmp slt i32 %t2, 5
  br i1 %t3, label %for.body.5, label %for.end.7
for.body.5:
  %t4 = load i32, ptr %local.4
  %t5 = icmp eq i32 %t4, 1
  %t6 = icmp eq i1 %t5, 1
  br i1 %t6, label %on.body.9, label %on.end.8
on.body.9:
  br label %for.update.2
on.end.8:
  %t7 = load i32, ptr %local.2
  %t8 = add i32 %t7, 100
  store i32 %t8, ptr %local.2
  br label %for.update.6
for.update.6:
  %t9 = load i32, ptr %local.4
  %t10 = add i32 %t9, 1
  store i32 %t10, ptr %local.4
  br label %for.cond.4
for.end.7:
  %t11 = load i32, ptr %local.2
  %t12 = load i32, ptr %local.3
  %t13 = add i32 %t11, %t12
  store i32 %t13, ptr %local.2
  br label %for.update.2
for.update.2:
  %t14 = load i32, ptr %local.3
  %t15 = add i32 %t14, 1
  store i32 %t15, ptr %local.3
  br label %for.cond.0
for.end.3:
  br label %for.cond.10
for.cond.10:
  br label %for.body.11
for.body.11:
  br label %for.end.13
for.end.13:
  %t16 = alloca i32, align 4
  store i32 0, ptr %t16, align 4
  br label %for.cond.14
for.cond.14:
  br label %for.body.15
for.body.15:
  store i32 7, ptr %t16, align 4
  br label %for.end.18
for.end.18:
  %t17 = load i32, ptr %t16, align 4
  %t18 = alloca i32, align 4
  store i32 0, ptr %t18, align 4
  br label %for.cond.19
for.cond.19:
  br label %for.body.20
for.body.20:
  store i32 11, ptr %t18, align 4
  br label %for.end.23
for.end.23:
  %t19 = load i32, ptr %t18, align 4
  %t20 = call i64 @string_builder_mark()
  %t21 = alloca { ptr, i64 }
  %t22 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 8 }, ptr %t22
  call void @to_string$string(ptr %t21, ptr %t22)
  call void @string_builder_append_string(ptr %t21)
  %t23 = load i32, ptr %local.2
  %t24 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t24, i32 %t23)
  call void @string_builder_append_string(ptr %t24)
  %t25 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t25, i64 %t20)
  %t26 = load { ptr, i64 }, ptr %t25
  call void @$prn({ ptr, i64 } %t26)
  %t27 = call i64 @string_builder_mark()
  %t28 = alloca { ptr, i64 }
  %t29 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 8 }, ptr %t29
  call void @to_string$string(ptr %t28, ptr %t29)
  call void @string_builder_append_string(ptr %t28)
  %t30 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t30, i32 %t17)
  call void @string_builder_append_string(ptr %t30)
  %t31 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t31, i64 %t27)
  %t32 = load { ptr, i64 }, ptr %t31
  call void @$prn({ ptr, i64 } %t32)
  %t33 = call i64 @string_builder_mark()
  %t34 = alloca { ptr, i64 }
  %t35 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 11 }, ptr %t35
  call void @to_string$string(ptr %t34, ptr %t35)
  call void @string_builder_append_string(ptr %t34)
  %t36 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t36, i32 %t19)
  call void @string_builder_append_string(ptr %t36)
  %t37 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t37, i64 %t33)
  %t38 = load { ptr, i64 }, ptr %t37
  call void @$prn({ ptr, i64 } %t38)
  %t39 = load i32, ptr %local.2
  %t40 = add i32 %t39, %t17
  %t41 = add i32 %t40, %t19
  ret i32 %t41
}

@$main = alias i32 (), ptr @fn.0
