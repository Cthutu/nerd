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
  let value: i32 = i32 1
  expr void on bool yes {
    value(bool yes) => {
      assign i32 local.0(value) = i32 7
    }
  }
  expr void call bind.2(prn)(string interpolate(i32 local.0(value)))
  let copy: i32 = i32 assign(i32 local.0(value) = i32 9)
  expr void call bind.2(prn)(string interpolate(i32 local.0(value), <unknown> " ", i32 local.1(copy)))
  return i32 local.1(copy)
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

define i32 @fn.0() {
  %local.0 = alloca i32
  store i32 1, ptr %local.0
  %t0 = icmp eq i1 1, 1
  br i1 %t0, label %on.body.1, label %on.end.0
on.body.1:
  store i32 7, ptr %local.0
  br label %on.end.0
on.end.0:
  %t1 = call i64 @string_builder_mark()
  %t2 = load i32, ptr %local.0
  %t3 = call { ptr, i64 } @to_string$i32(i32 %t2)
  call void @string_builder_append_string({ ptr, i64 } %t3)
  %t4 = call { ptr, i64 } @string_builder_finish(i64 %t1)
  call void @$prn({ ptr, i64 } %t4)
  store i32 9, ptr %local.0
  %t5 = call i64 @string_builder_mark()
  %t6 = load i32, ptr %local.0
  %t7 = call { ptr, i64 } @to_string$i32(i32 %t6)
  call void @string_builder_append_string({ ptr, i64 } %t7)
  %t8 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t8)
  %t9 = call { ptr, i64 } @to_string$i32(i32 9)
  call void @string_builder_append_string({ ptr, i64 } %t9)
  %t10 = call { ptr, i64 } @string_builder_finish(i64 %t5)
  call void @$prn({ ptr, i64 } %t10)
  ret i32 9
}

@$main = alias i32 (), ptr @fn.0
