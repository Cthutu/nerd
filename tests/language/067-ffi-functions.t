use std.io

ffi "c" abs (i32) -> i32

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
bind abs = fn.0
bind main = fn.1
extern func fn.0(i32) -> i32
func fn.1() -> void {
  let value: i32 = i32 call bind.5(abs)(i32 negate(i32 7))
  expr void call bind.2(prn)(string interpolate(<unknown> "abs = ", i32 local.0(value)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [2 x i8] c"c\00"
@.str.m0.1 = private unnamed_addr constant [7 x i8] c"abs = \00"

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

declare i32 @abs(i32)

define void @fn.1() {
  %t0 = sub i32 0, 7
  %t1 = call i32 @abs(i32 %t0)
  %t2 = call i64 @string_builder_mark()
  %t3 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 6 })
  call void @string_builder_append_string({ ptr, i64 } %t3)
  %t4 = call { ptr, i64 } @to_string$i32(i32 %t1)
  call void @string_builder_append_string({ ptr, i64 } %t4)
  %t5 = call { ptr, i64 } @string_builder_finish(i64 %t2)
  call void @$prn({ ptr, i64 } %t5)
  ret void
}

@$main = alias void (), ptr @fn.1
