use std.io

libm :: "m"
square_root :: ffi libm sqrt (value: f64) -> f64

main :: fn() {
    value := square_root(9.0)
    prn($"sqrt = {value}")
}
¬
0
¬
sqrt = 3

¬
hir 0
module module.0(070-ffi-library-linking.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
extern extern.0 sqrt from "m": fn (f64) -> f64
bind prn = import.0
bind input = import.1
bind libm = value.0
bind square_root = fn.0
bind main = fn.1
const value.0: string = string "m"
extern func fn.0(f64) -> f64
func fn.1() -> void {
  let value: f64 = f64 call bind.3(square_root)(f64 9)
  expr void call bind.0(prn)(string interpolate(<unknown> "sqrt = ", f64 local.0(value)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [41 x i8] c"tests/language/070-ffi-library-linking.t\00"
@.str.m0.0 = private unnamed_addr constant [2 x i8] c"m\00"
@.str.m0.1 = private unnamed_addr constant [8 x i8] c"sqrt = \00"

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

declare double @sqrt(double)

define internal void @fn.1() {
  %t3 = alloca { ptr, i64 }
  %t0 = call double @sqrt(double 0x4022000000000000)
  %t1 = call i64 @nrt_string_builder_mark()
  %t2 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 7 }, ptr %t3
  call void @nrt_to_string_string(ptr %t2, ptr %t3)
  call void @nrt_string_builder_append_string(ptr %t2)
  %t4 = alloca { ptr, i64 }
  call void @nrt_to_string_f64(ptr %t4, double %t0)
  call void @nrt_string_builder_append_string(ptr %t4)
  %t5 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t5, i64 %t1)
  %t6 = load { ptr, i64 }, ptr %t5
  call void @$prn({ ptr, i64 } %t6)
  ret void
}

@$main = hidden alias void (), ptr @fn.1
