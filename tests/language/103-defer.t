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
module module.103-defer.input(103-defer.input)
import import.0 prn from module.core(core).decl.N: <unknown>
import import.1 prn_text from module.core(core).decl.N: fn (string) -> void
import import.2 prn_empty from module.core(core).decl.N: fn () -> void
bind prn = import.0
bind prn_text = import.1
bind prn_empty = import.2
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
  let total: i32 = i32 call bind.3(cleanup_return)()
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
  expr void call bind.1(prn_text)(string interpolate(<unknown> "order=", i32 local.2(order)))
  {
    defer {
      assign i32 local.1(total) = i32 add(i32 local.1(total), i32 10)
    }
    defer {
      {
        expr void call bind.1(prn_text)(string "deferred-module-call")
      }
    }
    assign i32 local.1(total) = i32 add(i32 local.1(total), i32 1)
  }
  expr void call bind.1(prn_text)(string interpolate(<unknown> "after-block=", i32 local.1(total)))
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
  expr void call bind.1(prn_text)(string interpolate(<unknown> "after-loop=", i32 local.1(total)))
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

declare i1 @nrt_string_eq(ptr, ptr)
declare void @nrt_string_builder_reset()
declare i64 @nrt_string_builder_mark()
declare void @nrt_string_builder_append_string(ptr)
declare void @nrt_string_builder_append_byte(i8)
declare void @nrt_string_builder_finish(ptr, i64)
declare void @nrt_string_builder_finish_in(ptr, i64, ptr)
declare void @nrt_to_string_string(ptr, ptr)
declare void @nrt_to_string_bool(ptr, i1)
declare void @nrt_to_string_i8(ptr, i8)
declare void @nrt_to_string_i16(ptr, i16)
declare void @nrt_to_string_i32(ptr, i32)
declare void @nrt_to_string_i64(ptr, i64)
declare void @nrt_to_string_u8(ptr, i8)
declare void @nrt_to_string_u16(ptr, i16)
declare void @nrt_to_string_u32(ptr, i32)
declare void @nrt_to_string_u64(ptr, i64)
declare void @nrt_to_string_isize(ptr, i64)
declare void @nrt_to_string_usize(ptr, i64)
declare void @nrt_to_string_f32(ptr, float)
declare void @nrt_to_string_f64(ptr, double)

declare void @$prn_text({ ptr, i64 })
declare void @$prn_empty()

define internal i32 @fn.0() {
  %local.0 = alloca i32
  store i32 1, ptr %local.0
  %t0 = load i32, ptr %local.0
  store i32 9, ptr %local.0
  ret i32 %t0
}

define internal i32 @fn.1() {
  %local.1 = alloca i32
  %local.2 = alloca i32
  %t12 = alloca { ptr, i64 }
  %t23 = alloca { ptr, i64 }
  %local.3 = alloca i32
  %t48 = alloca { ptr, i64 }
  %t0 = call i32 @fn.0()
  store i32 %t0, ptr %local.1
  store i32 0, ptr %local.2
  %t1 = load i32, ptr %local.2
  %t2 = mul i32 %t1, 10
  %t3 = add i32 %t2, 2
  store i32 %t3, ptr %local.2
  %t4 = load i32, ptr %local.2
  %t5 = mul i32 %t4, 10
  %t6 = add i32 %t5, 1
  store i32 %t6, ptr %local.2
  %t7 = load i32, ptr %local.1
  %t8 = load i32, ptr %local.2
  %t9 = add i32 %t7, %t8
  store i32 %t9, ptr %local.1
  %t10 = call i64 @nrt_string_builder_mark()
  %t11 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 6 }, ptr %t12
  call void @nrt_to_string_string(ptr %t11, ptr %t12)
  call void @nrt_string_builder_append_string(ptr %t11)
  %t13 = load i32, ptr %local.2
  %t14 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t14, i32 %t13)
  call void @nrt_string_builder_append_string(ptr %t14)
  %t15 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t15, i64 %t10)
  %t16 = load { ptr, i64 }, ptr %t15
  call void @$prn_text({ ptr, i64 } %t16)
  %t17 = load i32, ptr %local.1
  %t18 = add i32 %t17, 1
  store i32 %t18, ptr %local.1
  call void @$prn_text({ ptr, i64 } { ptr @.str.m0.1, i64 20 })
  %t19 = load i32, ptr %local.1
  %t20 = add i32 %t19, 10
  store i32 %t20, ptr %local.1
  %t21 = call i64 @nrt_string_builder_mark()
  %t22 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 12 }, ptr %t23
  call void @nrt_to_string_string(ptr %t22, ptr %t23)
  call void @nrt_string_builder_append_string(ptr %t22)
  %t24 = load i32, ptr %local.1
  %t25 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t25, i32 %t24)
  call void @nrt_string_builder_append_string(ptr %t25)
  %t26 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t26, i64 %t21)
  %t27 = load { ptr, i64 }, ptr %t26
  call void @$prn_text({ ptr, i64 } %t27)
  store i32 0, ptr %local.3
  br label %for.cond.0
for.cond.0:
  %t28 = load i32, ptr %local.3
  %t29 = icmp slt i32 %t28, 3
  br i1 %t29, label %for.body.1, label %for.end.3
for.body.1:
  %t30 = load i32, ptr %local.3
  %t31 = icmp eq i32 %t30, 0
  %t32 = icmp eq i1 %t31, 1
  br i1 %t32, label %on.body.5, label %on.end.4
on.body.5:
  %t33 = load i32, ptr %local.1
  %t34 = add i32 %t33, 100
  store i32 %t34, ptr %local.1
  br label %for.update.2
on.end.4:
  %t35 = load i32, ptr %local.3
  %t36 = icmp eq i32 %t35, 1
  %t37 = icmp eq i1 %t36, 1
  br i1 %t37, label %on.body.7, label %on.end.6
on.body.7:
  %t38 = load i32, ptr %local.1
  %t39 = add i32 %t38, 100
  store i32 %t39, ptr %local.1
  br label %for.end.3
on.end.6:
  %t40 = load i32, ptr %local.1
  %t41 = add i32 %t40, 1000
  store i32 %t41, ptr %local.1
  %t42 = load i32, ptr %local.1
  %t43 = add i32 %t42, 100
  store i32 %t43, ptr %local.1
  br label %for.update.2
for.update.2:
  %t44 = load i32, ptr %local.3
  %t45 = add i32 %t44, 1
  store i32 %t45, ptr %local.3
  br label %for.cond.0
for.end.3:
  %t46 = call i64 @nrt_string_builder_mark()
  %t47 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 11 }, ptr %t48
  call void @nrt_to_string_string(ptr %t47, ptr %t48)
  call void @nrt_string_builder_append_string(ptr %t47)
  %t49 = load i32, ptr %local.1
  %t50 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t50, i32 %t49)
  call void @nrt_string_builder_append_string(ptr %t50)
  %t51 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t51, i64 %t46)
  %t52 = load { ptr, i64 }, ptr %t51
  call void @$prn_text({ ptr, i64 } %t52)
  %t53 = load i32, ptr %local.1
  ret i32 %t53
}

@$cleanup_return = internal alias i32 (), ptr @fn.0
@$main = hidden alias i32 (), ptr @fn.1

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)
