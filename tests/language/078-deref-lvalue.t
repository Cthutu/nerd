use std.io

main :: fn () -> i32 {
    value := 1
    ptr: ^i32 = ^value

    ptr^ = 7

    prn($"{value} {ptr^}")
    return value - 7
}
¬
0
¬
7 7

¬
hir 0
module module.0(078-deref-lvalue.input)
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
bind main = fn.0
func fn.0() -> i32 {
  let value: i32 = untyped integer 1
  let ptr: ^i32 = ^i32 address_of(i32 local.0(value))
  assign i32 deref(^i32 local.1(ptr)) = i32 7
  expr void call bind.2(prn)(string interpolate(i32 local.0(value), <unknown> " ", i32 deref(^i32 local.1(ptr))))
  return i32 subtract(i32 local.0(value), i32 7)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [2 x i8] c" \00"

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

define internal i32 @fn.0() {
  %local.0 = alloca i32
  store i32 1, ptr %local.0
  store i32 7, ptr %local.0
  %t0 = call i64 @string_builder_mark()
  %t1 = load i32, ptr %local.0
  %t2 = call { ptr, i64 } @to_string$i32(i32 %t1)
  call void @string_builder_append_string({ ptr, i64 } %t2)
  %t3 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t3)
  %t4 = load i32, ptr %local.0
  %t5 = call { ptr, i64 } @to_string$i32(i32 %t4)
  call void @string_builder_append_string({ ptr, i64 } %t5)
  %t6 = call { ptr, i64 } @string_builder_finish(i64 %t0)
  call void @$prn({ ptr, i64 } %t6)
  %t7 = load i32, ptr %local.0
  %t8 = sub i32 %t7, 7
  ret i32 %t8
}

@$main = alias i32 (), ptr @fn.0
