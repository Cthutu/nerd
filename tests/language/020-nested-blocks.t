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
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind main = fn.0
func fn.0() -> i32 {
  let value: i32 = untyped integer 10
  {
    let value: i32 = untyped integer 32
    assign i32 local.1(value) = i32 add(i32 local.1(value), i32 1)
    expr void call bind.0(prn)(string interpolate(<unknown> "inner=", i32 local.1(value)))
  }
  expr void call bind.0(prn)(string interpolate(<unknown> "outer=", i32 local.0(value)))
  return i32 local.0(value)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [57 x i8] c"tests/language/020-nested-blocks.t\00"
@.str.m0.0 = private unnamed_addr constant [7 x i8] c"inner=\00"
@.str.m0.1 = private unnamed_addr constant [7 x i8] c"outer=\00"

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
  %local.1 = alloca i32
  store i32 32, ptr %local.1
  %t0 = load i32, ptr %local.1
  %t1 = add i32 %t0, 1
  store i32 %t1, ptr %local.1
  %t2 = call i64 @string_builder_mark()
  %t3 = alloca { ptr, i64 }
  %t4 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 6 }, ptr %t4
  call void @to_string$string(ptr %t3, ptr %t4)
  call void @string_builder_append_string(ptr %t3)
  %t5 = load i32, ptr %local.1
  %t6 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t6, i32 %t5)
  call void @string_builder_append_string(ptr %t6)
  %t7 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t7, i64 %t2)
  %t8 = load { ptr, i64 }, ptr %t7
  call void @$prn({ ptr, i64 } %t8)
  %t9 = call i64 @string_builder_mark()
  %t10 = alloca { ptr, i64 }
  %t11 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 6 }, ptr %t11
  call void @to_string$string(ptr %t10, ptr %t11)
  call void @string_builder_append_string(ptr %t10)
  %t12 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t12, i32 10)
  call void @string_builder_append_string(ptr %t12)
  %t13 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t13, i64 %t9)
  %t14 = load { ptr, i64 }, ptr %t13
  call void @$prn({ ptr, i64 } %t14)
  ret i32 10
}

@$main = alias i32 (), ptr @fn.0
