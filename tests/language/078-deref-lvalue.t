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
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind main = fn.0
func fn.0() -> i32 {
  let value: i32 = untyped integer 1
  let ptr: ^i32 = ^i32 address_of(i32 local.0(value))
  assign i32 deref(^i32 local.1(ptr)) = i32 7
  expr void call bind.0(prn)(string interpolate(i32 local.0(value), <unknown> " ", i32 deref(^i32 local.1(ptr))))
  return i32 subtract(i32 local.0(value), i32 7)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [56 x i8] c"tests/language/078-deref-lvalue.t\00"
@.str.m0.0 = private unnamed_addr constant [2 x i8] c" \00"

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

declare void @$prn({ ptr, i64 })
declare ptr @$input({ ptr, i64 })

define internal i32 @fn.0() {
  %local.0 = alloca i32
  store i32 1, ptr %local.0
  store i32 7, ptr %local.0
  %t0 = call i64 @string_builder_mark()
  %t1 = load i32, ptr %local.0
  %t2 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t2, i32 %t1)
  call void @string_builder_append_string(ptr %t2)
  %t3 = alloca { ptr, i64 }
  %t4 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 1 }, ptr %t4
  call void @to_string$string(ptr %t3, ptr %t4)
  call void @string_builder_append_string(ptr %t3)
  %t5 = load i32, ptr %local.0
  %t6 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t6, i32 %t5)
  call void @string_builder_append_string(ptr %t6)
  %t7 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t7, i64 %t0)
  %t8 = load { ptr, i64 }, ptr %t7
  call void @$prn({ ptr, i64 } %t8)
  %t9 = load i32, ptr %local.0
  %t10 = sub i32 %t9, 7
  ret i32 %t10
}

@$main = alias i32 (), ptr @fn.0
