use std.io

main :: fn () {
    total := 0
    found :: for i := 0; i < 1; i += 1 {
        total += i
        break total + 7
    } else {
        break -1
    }
    missing :: for j := 0; j < 0; j += 1 {
        break j
    } else {
        break 42
    }

    prn($"found = {found}")
    prn($"missing = {missing}")
    return found + missing
}
¬
49
¬
found = 7
missing = 42

¬
hir 0
module module.050-for-else.input(050-for-else.input)
import module.std.io(std.io)
import import.0 prn from module.core(core).decl.N: <unknown>
import import.1 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.2 prn_text from module.core(core).decl.N: fn (string) -> void
import import.3 prn_empty from module.core(core).decl.N: fn () -> void
bind prn = import.0
bind input = import.1
bind prn_text = import.2
bind prn_empty = import.3
bind main = fn.0
func fn.0() -> i32 {
  let total: i32 = untyped integer 0
  let found: i32 = i32 for c_style {
    init {
      let i: i32 = untyped integer 0
    }
    condition bool less(i32 local.3(i), i32 1)
    body {
      assign i32 local.2(total) = i32 add(i32 local.2(total), i32 local.3(i))
      break i32 add(i32 local.2(total), i32 7)
    }
    update {
      assign i32 local.3(i) = i32 add(i32 local.3(i), i32 1)
    }
    else {
      break i32 negate(i32 1)
    }
  }
  let missing: i32 = i32 for c_style {
    init {
      let j: i32 = untyped integer 0
    }
    condition bool less(i32 local.4(j), i32 0)
    body {
      break i32 local.4(j)
    }
    update {
      assign i32 local.4(j) = i32 add(i32 local.4(j), i32 1)
    }
    else {
      break i32 42
    }
  }
  expr void call bind.2(prn_text)(string interpolate(<unknown> "found = ", i32 local.0(found)))
  expr void call bind.2(prn_text)(string interpolate(<unknown> "missing = ", i32 local.1(missing)))
  return i32 add(i32 local.0(found), i32 local.1(missing))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [30 x i8] c"tests/language/050-for-else.t\00"
@.str.m0.0 = private unnamed_addr constant [9 x i8] c"found = \00"
@.str.m0.1 = private unnamed_addr constant [11 x i8] c"missing = \00"

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

declare ptr @$input({ ptr, i64 })
declare void @$prn_text({ ptr, i64 })
declare void @$prn_empty()

define internal i32 @fn.0() {
  %local.2 = alloca i32
  %local.3 = alloca i32
  %local.4 = alloca i32
  %t21 = alloca { ptr, i64 }
  %t27 = alloca { ptr, i64 }
  store i32 0, ptr %local.2
  store i32 0, ptr %local.3
  %t0 = alloca i32, align 4
  store i32 0, ptr %t0, align 4
  br label %for.cond.0
for.cond.0:
  %t1 = load i32, ptr %local.3
  %t2 = icmp slt i32 %t1, 1
  br i1 %t2, label %for.body.1, label %for.else.3
for.body.1:
  %t3 = load i32, ptr %local.2
  %t4 = load i32, ptr %local.3
  %t5 = add i32 %t3, %t4
  store i32 %t5, ptr %local.2
  %t6 = load i32, ptr %local.2
  %t7 = add i32 %t6, 7
  store i32 %t7, ptr %t0, align 4
  br label %for.end.4
for.update.2:
  %t8 = load i32, ptr %local.3
  %t9 = add i32 %t8, 1
  store i32 %t9, ptr %local.3
  br label %for.cond.0
for.else.3:
  %t10 = sub i32 0, 1
  store i32 %t10, ptr %t0, align 4
  br label %for.end.4
for.end.4:
  %t11 = load i32, ptr %t0, align 4
  store i32 0, ptr %local.4
  %t12 = alloca i32, align 4
  store i32 0, ptr %t12, align 4
  br label %for.cond.5
for.cond.5:
  %t13 = load i32, ptr %local.4
  %t14 = icmp slt i32 %t13, 0
  br i1 %t14, label %for.body.6, label %for.else.8
for.body.6:
  %t15 = load i32, ptr %local.4
  store i32 %t15, ptr %t12, align 4
  br label %for.end.9
for.update.7:
  %t16 = load i32, ptr %local.4
  %t17 = add i32 %t16, 1
  store i32 %t17, ptr %local.4
  br label %for.cond.5
for.else.8:
  store i32 42, ptr %t12, align 4
  br label %for.end.9
for.end.9:
  %t18 = load i32, ptr %t12, align 4
  %t19 = call i64 @nrt_string_builder_mark()
  %t20 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 8 }, ptr %t21
  call void @nrt_to_string_string(ptr %t20, ptr %t21)
  call void @nrt_string_builder_append_string(ptr %t20)
  %t22 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t22, i32 %t11)
  call void @nrt_string_builder_append_string(ptr %t22)
  %t23 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t23, i64 %t19)
  %t24 = load { ptr, i64 }, ptr %t23
  call void @$prn_text({ ptr, i64 } %t24)
  %t25 = call i64 @nrt_string_builder_mark()
  %t26 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 10 }, ptr %t27
  call void @nrt_to_string_string(ptr %t26, ptr %t27)
  call void @nrt_string_builder_append_string(ptr %t26)
  %t28 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t28, i32 %t18)
  call void @nrt_string_builder_append_string(ptr %t28)
  %t29 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t29, i64 %t25)
  %t30 = load { ptr, i64 }, ptr %t29
  call void @$prn_text({ ptr, i64 } %t30)
  %t31 = add i32 %t11, %t18
  ret i32 %t31
}

@$main = hidden alias i32 (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)
