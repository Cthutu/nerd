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
module module.067-ffi-functions.input(067-ffi-functions.input)
import module.std.io(std.io)
import import.0 prn from module.core(core).decl.N: <unknown>
import import.1 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.2 prn_text from module.core(core).decl.N: fn (string) -> void
import import.3 prn_empty from module.core(core).decl.N: fn () -> void
extern extern.0 abs from "c": fn (i32) -> i32
bind prn = import.0
bind input = import.1
bind prn_text = import.2
bind prn_empty = import.3
bind abs = fn.0
bind main = fn.1
extern func fn.0(i32) -> i32
func fn.1() -> void {
  let value: i32 = i32 call bind.4(abs)(i32 negate(i32 7))
  expr void call bind.2(prn_text)(string interpolate(<unknown> "abs = ", i32 local.0(value)))
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
  call void @$prn_text({ ptr, i64 } %t7)
  ret void
}

@$main = hidden alias void (), ptr @fn.1

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)