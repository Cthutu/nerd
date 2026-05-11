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
  br label %for.cond.14
for.cond.14:
  br label %for.body.15
for.body.15:
  br label %for.end.17
for.end.17:
  %t16 = phi i32 [7, %for.body.15]
  br label %for.cond.18
for.cond.18:
  br label %for.body.19
for.body.19:
  br label %for.end.21
for.end.21:
  %t17 = phi i32 [11, %for.body.19]
  %t18 = call i64 @string_builder_mark()
  %t19 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 8 })
  call void @string_builder_append_string({ ptr, i64 } %t19)
  %t20 = load i32, ptr %local.2
  %t21 = call { ptr, i64 } @to_string$i32(i32 %t20)
  call void @string_builder_append_string({ ptr, i64 } %t21)
  %t22 = call { ptr, i64 } @string_builder_finish(i64 %t18)
  call void @$prn({ ptr, i64 } %t22)
  %t23 = call i64 @string_builder_mark()
  %t24 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 8 })
  call void @string_builder_append_string({ ptr, i64 } %t24)
  %t25 = call { ptr, i64 } @to_string$i32(i32 %t16)
  call void @string_builder_append_string({ ptr, i64 } %t25)
  %t26 = call { ptr, i64 } @string_builder_finish(i64 %t23)
  call void @$prn({ ptr, i64 } %t26)
  %t27 = call i64 @string_builder_mark()
  %t28 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 11 })
  call void @string_builder_append_string({ ptr, i64 } %t28)
  %t29 = call { ptr, i64 } @to_string$i32(i32 %t17)
  call void @string_builder_append_string({ ptr, i64 } %t29)
  %t30 = call { ptr, i64 } @string_builder_finish(i64 %t27)
  call void @$prn({ ptr, i64 } %t30)
  %t31 = load i32, ptr %local.2
  %t32 = add i32 %t31, %t16
  %t33 = add i32 %t32, %t17
  ret i32 %t33
}

@$main = alias i32 (), ptr @fn.0
