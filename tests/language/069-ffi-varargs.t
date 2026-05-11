-- test-platform: linux

use std.io

ffi "c" fcntl (i32, i32, ...) -> i32

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
module module.0(069-ffi-varargs.input)
import module.1(std.io)
import import.0 pr from module.1(std.io).decl.6: fn (string) -> void
import import.1 epr from module.1(std.io).decl.7: fn (string) -> void
import import.2 prn from module.1(std.io).decl.8: fn (string) -> void
import import.3 eprn from module.1(std.io).decl.9: fn (string) -> void
import import.4 input from module.1(std.io).decl.10: fn (string) -> string
extern extern.0 fcntl from "c": fn (i32, i32, ...) -> i32
bind pr = import.0
bind epr = import.1
bind prn = import.2
bind eprn = import.3
bind input = import.4
bind fcntl = fn.0
bind main = fn.1
extern func fn.0(i32, i32) -> i32
func fn.1() -> void {
  let result: i32 = i32 call bind.5(fcntl)(i32 0, i32 1, untyped integer 0)
  expr void call bind.2(prn)(string interpolate(<unknown> "fcntl = ", bool greater_equal(i32 local.0(result), i32 0)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [2 x i8] c"c\00"
@.str.m0.1 = private unnamed_addr constant [9 x i8] c"fcntl = \00"

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

declare void @$pr({ ptr, i64 })
declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

declare i32 @fcntl(i32, i32, ...)

define internal void @fn.1() {
  %t0 = call i32 @fcntl(i32 0, i32 1, i32 0)
  %t1 = call i64 @string_builder_mark()
  %t2 = alloca { ptr, i64 }
  %t3 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 8 }, ptr %t3
  call void @to_string$string(ptr %t2, ptr %t3)
  call void @string_builder_append_string(ptr %t2)
  %t4 = icmp sge i32 %t0, 0
  %t5 = alloca { ptr, i64 }
  call void @to_string$bool(ptr %t5, i1 %t4)
  call void @string_builder_append_string(ptr %t5)
  %t6 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t6, i64 %t1)
  %t7 = load { ptr, i64 }, ptr %t6
  call void @$prn({ ptr, i64 } %t7)
  ret void
}

@$main = alias void (), ptr @fn.1

