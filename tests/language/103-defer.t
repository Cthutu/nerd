use std.io

io :: use std.io

cleanup_return :: fn () -> i32 {
    value := 1
    defer value = 9
    return value
}

main :: fn () {
    total := cleanup_return()
    order := 0
    {
        defer order = order * 10 + 1
        defer order = order * 10 + 2
    }
    total += order
    prn($"order={order}")

    {
        defer total += 10
        defer {
            io.prn("deferred-module-call")
        }
        total += 1
    }
    prn($"after-block={total}")

    for i := 0; i < 3; i += 1 {
        defer total += 100
        on i == 0 => continue
        on i == 1 => break
        total += 1000
    }
    prn($"after-loop={total}")
    return total
}
¬
233
¬
order=21
deferred-module-call
after-block=33
after-loop=233

¬
hir 0
module module.0(103-defer.input)
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
bind io = module.1
bind cleanup_return = fn.0
bind main = fn.1
func fn.0() -> i32 {
  let value: i32 = untyped integer 1
  defer {
    assign i32 local.0(value) = i32 9
  }
  return i32 local.0(value)
}
func fn.1() -> i32 {
  let total: i32 = i32 call bind.6(cleanup_return)()
  let order: i32 = untyped integer 0
  {
    defer {
      assign i32 local.2(order) = i32 add(i32 multiply(i32 local.2(order), i32 10), i32 1)
    }
    defer {
      assign i32 local.2(order) = i32 add(i32 multiply(i32 local.2(order), i32 10), i32 2)
    }
  }
  assign i32 local.1(total) = i32 add(i32 local.1(total), i32 local.2(order))
  expr void call bind.2(prn)(string interpolate(<unknown> "order=", i32 local.2(order)))
  {
    defer {
      assign i32 local.1(total) = i32 add(i32 local.1(total), i32 10)
    }
    defer {
      {
        expr void call fn (string) -> void field(module bind.5(io), prn)(string "deferred-module-call")
      }
    }
    assign i32 local.1(total) = i32 add(i32 local.1(total), i32 1)
  }
  expr void call bind.2(prn)(string interpolate(<unknown> "after-block=", i32 local.1(total)))
  expr void for c_style {
    init {
      let i: i32 = untyped integer 0
    }
    condition bool less(i32 local.3(i), i32 3)
    body {
      defer {
        assign i32 local.1(total) = i32 add(i32 local.1(total), i32 100)
      }
      expr void on bool equal(i32 local.3(i), i32 0) {
    value(bool yes) => {
      continue
    }
  }
      expr void on bool equal(i32 local.3(i), i32 1) {
    value(bool yes) => {
      break
    }
  }
      assign i32 local.1(total) = i32 add(i32 local.1(total), i32 1000)
    }
    update {
      assign i32 local.3(i) = i32 add(i32 local.3(i), i32 1)
    }
  }
  expr void call bind.2(prn)(string interpolate(<unknown> "after-loop=", i32 local.1(total)))
  return i32 local.1(total)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [7 x i8] c"order=\00"
@.str.m0.1 = private unnamed_addr constant [21 x i8] c"deferred-module-call\00"
@.str.m0.2 = private unnamed_addr constant [13 x i8] c"after-block=\00"
@.str.m0.3 = private unnamed_addr constant [12 x i8] c"after-loop=\00"

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

define i32 @fn.0() {
  %local.0 = alloca i32
  store i32 9, ptr %local.0
  ret i32 1
}

define i32 @fn.1() {
  %t0 = call i32 @fn.0()
  %local.1 = alloca i32
  store i32 %t0, ptr %local.1
  %t1 = mul i32 0, 10
  %t2 = add i32 %t1, 2
  %local.2 = alloca i32
  store i32 %t2, ptr %local.2
  %t3 = load i32, ptr %local.2
  %t4 = mul i32 %t3, 10
  %t5 = add i32 %t4, 1
  store i32 %t5, ptr %local.2
  %t6 = load i32, ptr %local.1
  %t7 = load i32, ptr %local.2
  %t8 = add i32 %t6, %t7
  store i32 %t8, ptr %local.1
  %t9 = call i64 @string_builder_mark()
  %t10 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 6 })
  call void @string_builder_append_string({ ptr, i64 } %t10)
  %t11 = load i32, ptr %local.2
  %t12 = call { ptr, i64 } @to_string$i32(i32 %t11)
  call void @string_builder_append_string({ ptr, i64 } %t12)
  %t13 = call { ptr, i64 } @string_builder_finish(i64 %t9)
  call void @$prn({ ptr, i64 } %t13)
  %t14 = load i32, ptr %local.1
  %t15 = add i32 %t14, 1
  store i32 %t15, ptr %local.1
  call void @$prn({ ptr, i64 } { ptr @.str.m0.1, i64 20 })
  %t16 = load i32, ptr %local.1
  %t17 = add i32 %t16, 10
  store i32 %t17, ptr %local.1
  %t18 = call i64 @string_builder_mark()
  %t19 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 12 })
  call void @string_builder_append_string({ ptr, i64 } %t19)
  %t20 = load i32, ptr %local.1
  %t21 = call { ptr, i64 } @to_string$i32(i32 %t20)
  call void @string_builder_append_string({ ptr, i64 } %t21)
  %t22 = call { ptr, i64 } @string_builder_finish(i64 %t18)
  call void @$prn({ ptr, i64 } %t22)
  %local.3 = alloca i32
  store i32 0, ptr %local.3
  br label %for.cond.0
for.cond.0:
  %t23 = load i32, ptr %local.3
  %t24 = icmp slt i32 %t23, 3
  br i1 %t24, label %for.body.1, label %for.end.3
for.body.1:
  %t25 = load i32, ptr %local.3
  %t26 = icmp eq i32 %t25, 0
  %t27 = icmp eq i1 %t26, 1
  br i1 %t27, label %on.body.5, label %on.end.4
on.body.5:
  %t28 = load i32, ptr %local.1
  %t29 = add i32 %t28, 100
  store i32 %t29, ptr %local.1
  br label %for.update.2
on.end.4:
  %t30 = load i32, ptr %local.3
  %t31 = icmp eq i32 %t30, 1
  %t32 = icmp eq i1 %t31, 1
  br i1 %t32, label %on.body.7, label %on.end.6
on.body.7:
  %t33 = load i32, ptr %local.1
  %t34 = add i32 %t33, 100
  store i32 %t34, ptr %local.1
  br label %for.end.3
on.end.6:
  %t35 = load i32, ptr %local.1
  %t36 = add i32 %t35, 1000
  store i32 %t36, ptr %local.1
  %t37 = load i32, ptr %local.1
  %t38 = add i32 %t37, 100
  store i32 %t38, ptr %local.1
  br label %for.update.2
for.update.2:
  %t39 = load i32, ptr %local.3
  %t40 = add i32 %t39, 1
  store i32 %t40, ptr %local.3
  br label %for.cond.0
for.end.3:
  %t41 = call i64 @string_builder_mark()
  %t42 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.3, i64 11 })
  call void @string_builder_append_string({ ptr, i64 } %t42)
  %t43 = load i32, ptr %local.1
  %t44 = call { ptr, i64 } @to_string$i32(i32 %t43)
  call void @string_builder_append_string({ ptr, i64 } %t44)
  %t45 = call { ptr, i64 } @string_builder_finish(i64 %t41)
  call void @$prn({ ptr, i64 } %t45)
  %t46 = load i32, ptr %local.1
  ret i32 %t46
}

@$cleanup_return = internal alias i32 (), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
