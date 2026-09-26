-- test-platform: linux

use std.io

ffi "c" fcntl (fd: i32, command: i32, ...) -> i32

main :: fn() {
    result := fcntl(0, 1, 0)
    prn($"fcntl = {result >= 0}")
}
¬
0
¬
fcntl = yes

¬
hir 0
module module.069-ffi-varargs.input(069-ffi-varargs.input)
import module.std.io(std.io)
import import.0 prn from module.core(core).decl.N: <unknown>
import import.1 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.2 prn_text from module.core(core).decl.N: fn (string) -> void
import import.3 prn_empty from module.core(core).decl.N: fn () -> void
extern extern.0 fcntl from "c": fn (i32, i32, ...) -> i32
bind prn = import.0
bind input = import.1
bind prn_text = import.2
bind prn_empty = import.3
bind fcntl = fn.0
bind main = fn.1
extern func fn.0(i32, i32) -> i32
func fn.1() -> void {
  let result: i32 = i32 call bind.4(fcntl)(i32 0, i32 1, untyped integer 0)
  expr void call bind.2(prn_text)(string interpolate(<unknown> "fcntl = ", bool greater_equal(i32 local.0(result), i32 0)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [33 x i8] c"tests/language/069-ffi-varargs.t\00"
@.str.m0.0 = private unnamed_addr constant [2 x i8] c"c\00"
@.str.m0.1 = private unnamed_addr constant [9 x i8] c"fcntl = \00"

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

declare i32 @fcntl(i32, i32, ...)

define internal void @fn.1() {
  %t3 = alloca { ptr, i64 }
  %t0 = call i32 (i32, i32, ...) @fcntl(i32 0, i32 1, i32 0)
  %t1 = call i64 @nrt_string_builder_mark()
  %t2 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 8 }, ptr %t3
  call void @nrt_to_string_string(ptr %t2, ptr %t3)
  call void @nrt_string_builder_append_string(ptr %t2)
  %t4 = icmp sge i32 %t0, 0
  %t5 = alloca { ptr, i64 }
  call void @nrt_to_string_bool(ptr %t5, i1 zeroext %t4)
  call void @nrt_string_builder_append_string(ptr %t5)
  %t6 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t6, i64 %t1)
  %t7 = load { ptr, i64 }, ptr %t6
  call void @$prn_text({ ptr, i64 } %t7)
  ret void
}

@$main = hidden alias void (), ptr @fn.1

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)