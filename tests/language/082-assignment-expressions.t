use std.io

main :: fn () -> i32 {
    value: i32 = 1

    on yes => value = 7
    prn($"{value}")

    copy: i32 = value = 9
    prn($"{value} {copy}")

    return copy
}
¬
9
¬
7
9 9

¬
hir 0
module module.082-assignment-expressions.input(082-assignment-expressions.input)
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
  let value: i32 = i32 1
  expr void on bool yes {
    value(bool yes) => {
      assign i32 local.0(value) = i32 7
    }
  }
  expr void call bind.2(prn_text)(string interpolate(i32 local.0(value)))
  let copy: i32 = i32 assign(i32 local.0(value) = i32 9)
  expr void call bind.2(prn_text)(string interpolate(i32 local.0(value), <unknown> " ", i32 local.1(copy)))
  return i32 local.1(copy)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [44 x i8] c"tests/language/082-assignment-expressions.t\00"
@.str.m0.0 = private unnamed_addr constant [2 x i8] c" \00"

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
  %local.0 = alloca i32
  %t10 = alloca { ptr, i64 }
  store i32 1, ptr %local.0
  %t0 = icmp eq i1 1, 1
  br i1 %t0, label %on.body.1, label %on.end.0
on.body.1:
  store i32 7, ptr %local.0
  br label %on.end.0
on.end.0:
  %t1 = call i64 @nrt_string_builder_mark()
  %t2 = load i32, ptr %local.0
  %t3 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t3, i32 %t2)
  call void @nrt_string_builder_append_string(ptr %t3)
  %t4 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t4, i64 %t1)
  %t5 = load { ptr, i64 }, ptr %t4
  call void @$prn_text({ ptr, i64 } %t5)
  store i32 9, ptr %local.0
  %t6 = call i64 @nrt_string_builder_mark()
  %t7 = load i32, ptr %local.0
  %t8 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t8, i32 %t7)
  call void @nrt_string_builder_append_string(ptr %t8)
  %t9 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 1 }, ptr %t10
  call void @nrt_to_string_string(ptr %t9, ptr %t10)
  call void @nrt_string_builder_append_string(ptr %t9)
  %t11 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t11, i32 9)
  call void @nrt_string_builder_append_string(ptr %t11)
  %t12 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t12, i64 %t6)
  %t13 = load { ptr, i64 }, ptr %t12
  call void @$prn_text({ ptr, i64 } %t13)
  ret i32 9
}

@$main = hidden alias i32 (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)