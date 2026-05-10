use std.io

-- Shows that nested block state is dropped after leaving the block.
main :: fn () {
    value := 10
    {
        value := 32
        value = value + 1
        prn($"inner={value}")
    }
    prn($"outer={value}")
    return value
}
¬
10
¬
inner=33
outer=10

¬
hir 0
module module.0(020-nested-blocks.input)
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
  let value: i32 = untyped integer 10
  {
    let value: i32 = untyped integer 32
    assign i32 local.1(value) = i32 add(i32 local.1(value), i32 1)
    expr void call bind.2(prn)(string interpolate(<unknown> "inner=", i32 local.1(value)))
  }
  expr void call bind.2(prn)(string interpolate(<unknown> "outer=", i32 local.0(value)))
  return i32 local.0(value)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [7 x i8] c"inner=\00"
@.str.m0.1 = private unnamed_addr constant [7 x i8] c"outer=\00"

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
  %local.1 = alloca i32
  store i32 32, ptr %local.1
  %t0 = load i32, ptr %local.1
  %t1 = add i32 %t0, 1
  store i32 %t1, ptr %local.1
  %t2 = call i64 @string_builder_mark()
  %t3 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 6 })
  call void @string_builder_append_string({ ptr, i64 } %t3)
  %t4 = load i32, ptr %local.1
  %t5 = call { ptr, i64 } @to_string$i32(i32 %t4)
  call void @string_builder_append_string({ ptr, i64 } %t5)
  %t6 = call { ptr, i64 } @string_builder_finish(i64 %t2)
  call void @$prn({ ptr, i64 } %t6)
  %t7 = call i64 @string_builder_mark()
  %t8 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 6 })
  call void @string_builder_append_string({ ptr, i64 } %t8)
  %t9 = call { ptr, i64 } @to_string$i32(i32 10)
  call void @string_builder_append_string({ ptr, i64 } %t9)
  %t10 = call { ptr, i64 } @string_builder_finish(i64 %t7)
  call void @$prn({ ptr, i64 } %t10)
  ret i32 10
}

@$main = alias i32 (), ptr @fn.0
