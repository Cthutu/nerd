use std.io

libm :: "m"
square_root :: ffi libm sqrt (f64) -> f64

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
bind libm = value.0
bind square_root = fn.0
bind main = fn.1
const value.0: string = string "m"
extern func fn.0(f64) -> f64
func fn.1() -> void {
  let value: f64 = f64 call bind.6(square_root)(f64 9)
  expr void call bind.2(prn)(string interpolate(<unknown> "sqrt = ", f64 local.0(value)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [2 x i8] c"m\00"
@.str.m0.1 = private unnamed_addr constant [8 x i8] c"sqrt = \00"

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

declare double @sqrt(double)

define void @fn.1() {
  %t0 = call double @sqrt(double 0x4022000000000000)
  %t1 = call i64 @string_builder_mark()
  %t2 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 7 })
  call void @string_builder_append_string({ ptr, i64 } %t2)
  %t3 = call { ptr, i64 } @to_string$f64(double %t0)
  call void @string_builder_append_string({ ptr, i64 } %t3)
  %t4 = call { ptr, i64 } @string_builder_finish(i64 %t1)
  call void @$prn({ ptr, i64 } %t4)
  ret void
}

@$main = alias void (), ptr @fn.1
