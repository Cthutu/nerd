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
            prn("deferred-module-call")
        }
        total += 1
    }
    prn($"after-block={total}")

    for i := 0; i < 3; i += 1 {
        defer total += 100
        on i == 0 => again
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
import import.0 prn from module.1(core).decl.13: fn (string) -> void
bind prn = import.0
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
  let total: i32 = i32 call bind.1(cleanup_return)()
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
  expr void call bind.0(prn)(string interpolate(<unknown> "order=", i32 local.2(order)))
  {
    defer {
      assign i32 local.1(total) = i32 add(i32 local.1(total), i32 10)
    }
    defer {
      {
        expr void call bind.0(prn)(string "deferred-module-call")
      }
    }
    assign i32 local.1(total) = i32 add(i32 local.1(total), i32 1)
  }
  expr void call bind.0(prn)(string interpolate(<unknown> "after-block=", i32 local.1(total)))
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
      again
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
  expr void call bind.0(prn)(string interpolate(<unknown> "after-loop=", i32 local.1(total)))
  return i32 local.1(total)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [27 x i8] c"tests/language/103-defer.t\00"
@.str.m0.0 = private unnamed_addr constant [7 x i8] c"order=\00"
@.str.m0.1 = private unnamed_addr constant [21 x i8] c"deferred-module-call\00"
@.str.m0.2 = private unnamed_addr constant [13 x i8] c"after-block=\00"
@.str.m0.3 = private unnamed_addr constant [12 x i8] c"after-loop=\00"

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

define internal i32 @fn.0() {
  %local.0 = alloca i32
  store i32 9, ptr %local.0
  ret i32 1
}

define internal i32 @fn.1() {
  %local.1 = alloca i32
  %local.2 = alloca i32
  %t11 = alloca { ptr, i64 }
  %t22 = alloca { ptr, i64 }
  %local.3 = alloca i32
  %t47 = alloca { ptr, i64 }
  %t0 = call i32 @fn.0()
  store i32 %t0, ptr %local.1
  %t1 = mul i32 0, 10
  %t2 = add i32 %t1, 2
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
  %t10 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 6 }, ptr %t11
  call void @to_string$string(ptr %t10, ptr %t11)
  call void @string_builder_append_string(ptr %t10)
  %t12 = load i32, ptr %local.2
  %t13 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t13, i32 %t12)
  call void @string_builder_append_string(ptr %t13)
  %t14 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t14, i64 %t9)
  %t15 = load { ptr, i64 }, ptr %t14
  call void @$prn({ ptr, i64 } %t15)
  %t16 = load i32, ptr %local.1
  %t17 = add i32 %t16, 1
  store i32 %t17, ptr %local.1
  call void @$prn({ ptr, i64 } { ptr @.str.m0.1, i64 20 })
  %t18 = load i32, ptr %local.1
  %t19 = add i32 %t18, 10
  store i32 %t19, ptr %local.1
  %t20 = call i64 @string_builder_mark()
  %t21 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 12 }, ptr %t22
  call void @to_string$string(ptr %t21, ptr %t22)
  call void @string_builder_append_string(ptr %t21)
  %t23 = load i32, ptr %local.1
  %t24 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t24, i32 %t23)
  call void @string_builder_append_string(ptr %t24)
  %t25 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t25, i64 %t20)
  %t26 = load { ptr, i64 }, ptr %t25
  call void @$prn({ ptr, i64 } %t26)
  store i32 0, ptr %local.3
  br label %for.cond.0
for.cond.0:
  %t27 = load i32, ptr %local.3
  %t28 = icmp slt i32 %t27, 3
  br i1 %t28, label %for.body.1, label %for.end.3
for.body.1:
  %t29 = load i32, ptr %local.3
  %t30 = icmp eq i32 %t29, 0
  %t31 = icmp eq i1 %t30, 1
  br i1 %t31, label %on.body.5, label %on.end.4
on.body.5:
  %t32 = load i32, ptr %local.1
  %t33 = add i32 %t32, 100
  store i32 %t33, ptr %local.1
  br label %for.update.2
on.end.4:
  %t34 = load i32, ptr %local.3
  %t35 = icmp eq i32 %t34, 1
  %t36 = icmp eq i1 %t35, 1
  br i1 %t36, label %on.body.7, label %on.end.6
on.body.7:
  %t37 = load i32, ptr %local.1
  %t38 = add i32 %t37, 100
  store i32 %t38, ptr %local.1
  br label %for.end.3
on.end.6:
  %t39 = load i32, ptr %local.1
  %t40 = add i32 %t39, 1000
  store i32 %t40, ptr %local.1
  %t41 = load i32, ptr %local.1
  %t42 = add i32 %t41, 100
  store i32 %t42, ptr %local.1
  br label %for.update.2
for.update.2:
  %t43 = load i32, ptr %local.3
  %t44 = add i32 %t43, 1
  store i32 %t44, ptr %local.3
  br label %for.cond.0
for.end.3:
  %t45 = call i64 @string_builder_mark()
  %t46 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 11 }, ptr %t47
  call void @to_string$string(ptr %t46, ptr %t47)
  call void @string_builder_append_string(ptr %t46)
  %t48 = load i32, ptr %local.1
  %t49 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t49, i32 %t48)
  call void @string_builder_append_string(ptr %t49)
  %t50 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t50, i64 %t45)
  %t51 = load { ptr, i64 }, ptr %t50
  call void @$prn({ ptr, i64 } %t51)
  %t52 = load i32, ptr %local.1
  ret i32 %t52
}

@$cleanup_return = internal alias i32 (), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
