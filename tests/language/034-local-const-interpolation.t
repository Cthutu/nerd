use std.io

main :: fn () {
  i :: 2
  f :: 3.14
  s :: "Hello, world!"

  prn($"{s}  i = {i} and f = {f}!")
}
¬
0
¬
Hello, world!  i = 2 and f = 3.14!

¬
hir 0
module module.0(034-local-const-interpolation.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind main = fn.0
func fn.0() -> void {
  let i: untyped integer = untyped integer 2
  let f: untyped float = untyped float 3.1400000000000001
  let s: string = string "Hello, world!"
  expr void call bind.0(prn)(string interpolate(string local.2(s), <unknown> "  i = ", untyped integer local.0(i), <unknown> " and f = ", untyped float local.1(f), <unknown> "!"))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [47 x i8] c"tests/language/034-local-const-interpolation.t\00"
@.str.m0.0 = private unnamed_addr constant [14 x i8] c"Hello, world!\00"
@.str.m0.1 = private unnamed_addr constant [7 x i8] c"  i = \00"
@.str.m0.2 = private unnamed_addr constant [10 x i8] c" and f = \00"
@.str.m0.3 = private unnamed_addr constant [2 x i8] c"!\00"

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

define internal void @fn.0() {
  %t2 = alloca { ptr, i64 }
  %t4 = alloca { ptr, i64 }
  %t7 = alloca { ptr, i64 }
  %t10 = alloca { ptr, i64 }
  %t0 = call i64 @string_builder_mark()
  %t1 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 13 }, ptr %t2
  call void @to_string$string(ptr %t1, ptr %t2)
  call void @string_builder_append_string(ptr %t1)
  %t3 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 6 }, ptr %t4
  call void @to_string$string(ptr %t3, ptr %t4)
  call void @string_builder_append_string(ptr %t3)
  %t5 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t5, i32 2)
  call void @string_builder_append_string(ptr %t5)
  %t6 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 9 }, ptr %t7
  call void @to_string$string(ptr %t6, ptr %t7)
  call void @string_builder_append_string(ptr %t6)
  %t8 = alloca { ptr, i64 }
  call void @to_string$f64(ptr %t8, double 0x40091EB851EB851F)
  call void @string_builder_append_string(ptr %t8)
  %t9 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 1 }, ptr %t10
  call void @to_string$string(ptr %t9, ptr %t10)
  call void @string_builder_append_string(ptr %t9)
  %t11 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t11, i64 %t0)
  %t12 = load { ptr, i64 }, ptr %t11
  call void @$prn({ ptr, i64 } %t12)
  ret void
}

@$main = alias void (), ptr @fn.0
