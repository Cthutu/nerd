use std.io

ffi "c" abs (value: i32) -> i32

main :: fn() {
    value := abs(-7)
    prn($"abs = {value}")
}
¬
0
¬
abs = 7

¬
hir 0
module module.0(067-ffi-functions.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
extern extern.0 abs from "c": fn (i32) -> i32
bind prn = import.0
bind input = import.1
bind abs = fn.0
bind main = fn.1
extern func fn.0(i32) -> i32
func fn.1() -> void {
  let value: i32 = i32 call bind.2(abs)(i32 negate(i32 7))
  expr void call bind.0(prn)(string interpolate(<unknown> "abs = ", i32 local.0(value)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [35 x i8] c"tests/language/067-ffi-functions.t\00"
@.str.m0.0 = private unnamed_addr constant [2 x i8] c"c\00"
@.str.m0.1 = private unnamed_addr constant [7 x i8] c"abs = \00"

declare i1 @nrt_string_eq(ptr, ptr)
declare void @nrt_string_builder_reset()
declare i64 @nrt_string_builder_mark()
declare void @nrt_string_builder_append_string(ptr)
declare void @nrt_string_builder_append_byte(i8)
declare void @nrt_string_builder_finish(ptr, i64)
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

declare void @$prn({ ptr, i64 })
declare ptr @$input({ ptr, i64 })

declare i32 @abs(i32)

define internal void @fn.1() {
  %t4 = alloca { ptr, i64 }
  %t0 = sub i32 0, 7
  %t1 = call i32 @abs(i32 %t0)
  %t2 = call i64 @nrt_string_builder_mark()
  %t3 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 6 }, ptr %t4
  call void @nrt_to_string_string(ptr %t3, ptr %t4)
  call void @nrt_string_builder_append_string(ptr %t3)
  %t5 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t5, i32 %t1)
  call void @nrt_string_builder_append_string(ptr %t5)
  %t6 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t6, i64 %t2)
  %t7 = load { ptr, i64 }, ptr %t6
  call void @$prn({ ptr, i64 } %t7)
  ret void
}

@$main = hidden alias void (), ptr @fn.1
