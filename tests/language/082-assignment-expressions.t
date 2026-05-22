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
module module.0(082-assignment-expressions.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind main = fn.0
func fn.0() -> i32 {
  let value: i32 = i32 1
  expr void on bool yes {
    value(bool yes) => {
      assign i32 local.0(value) = i32 7
    }
  }
  expr void call bind.0(prn)(string interpolate(i32 local.0(value)))
  let copy: i32 = i32 assign(i32 local.0(value) = i32 9)
  expr void call bind.0(prn)(string interpolate(i32 local.0(value), <unknown> " ", i32 local.1(copy)))
  return i32 local.1(copy)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [44 x i8] c"tests/language/082-assignment-expressions.t\00"
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
  %t10 = alloca { ptr, i64 }
  store i32 1, ptr %local.0
  %t0 = icmp eq i1 1, 1
  br i1 %t0, label %on.body.1, label %on.end.0
on.body.1:
  store i32 7, ptr %local.0
  br label %on.end.0
on.end.0:
  %t1 = call i64 @string_builder_mark()
  %t2 = load i32, ptr %local.0
  %t3 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t3, i32 %t2)
  call void @string_builder_append_string(ptr %t3)
  %t4 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t4, i64 %t1)
  %t5 = load { ptr, i64 }, ptr %t4
  call void @$prn({ ptr, i64 } %t5)
  store i32 9, ptr %local.0
  %t6 = call i64 @string_builder_mark()
  %t7 = load i32, ptr %local.0
  %t8 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t8, i32 %t7)
  call void @string_builder_append_string(ptr %t8)
  %t9 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 1 }, ptr %t10
  call void @to_string$string(ptr %t9, ptr %t10)
  call void @string_builder_append_string(ptr %t9)
  %t11 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t11, i32 9)
  call void @string_builder_append_string(ptr %t11)
  %t12 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t12, i64 %t6)
  %t13 = load { ptr, i64 }, ptr %t12
  call void @$prn({ ptr, i64 } %t13)
  ret i32 9
}

@$main = alias i32 (), ptr @fn.0
