on "debug" {
    use std.io
    answer :: 7

    on !"release" {
        mode_answer :: 11
    }

    on "x64" {
        arch_answer :: 13
    }

    on !"feature" {
        fallback :: 9
    }
}

main :: fn () {
    prn($"answer={answer}")
    prn($"mode={mode_answer}")
    prn($"arch={arch_answer}")
    prn($"fallback={fallback}")
}
¬
0
¬
answer=7
mode=11
arch=13
fallback=9

¬
hir 0
module module.0(072-top-level-on.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind answer = value.0
bind mode_answer = value.1
bind arch_answer = value.2
bind fallback = value.3
bind main = fn.0
const value.0: untyped integer = untyped integer 7
const value.1: untyped integer = untyped integer 11
const value.2: untyped integer = untyped integer 13
const value.3: untyped integer = untyped integer 9
func fn.0() -> void {
  expr void call bind.0(prn)(string interpolate(<unknown> "answer=", untyped integer bind.2(answer)))
  expr void call bind.0(prn)(string interpolate(<unknown> "mode=", untyped integer bind.3(mode_answer)))
  expr void call bind.0(prn)(string interpolate(<unknown> "arch=", untyped integer bind.4(arch_answer)))
  expr void call bind.0(prn)(string interpolate(<unknown> "fallback=", untyped integer bind.5(fallback)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [34 x i8] c"tests/language/072-top-level-on.t\00"
@.str.m0.0 = private unnamed_addr constant [6 x i8] c"debug\00"
@.str.m0.1 = private unnamed_addr constant [8 x i8] c"release\00"
@.str.m0.2 = private unnamed_addr constant [4 x i8] c"x64\00"
@.str.m0.3 = private unnamed_addr constant [8 x i8] c"feature\00"
@.str.m0.4 = private unnamed_addr constant [8 x i8] c"answer=\00"
@.str.m0.5 = private unnamed_addr constant [6 x i8] c"mode=\00"
@.str.m0.6 = private unnamed_addr constant [6 x i8] c"arch=\00"
@.str.m0.7 = private unnamed_addr constant [10 x i8] c"fallback=\00"

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
  %t8 = alloca { ptr, i64 }
  %t14 = alloca { ptr, i64 }
  %t20 = alloca { ptr, i64 }
  %t0 = call i64 @string_builder_mark()
  %t1 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 7 }, ptr %t2
  call void @to_string$string(ptr %t1, ptr %t2)
  call void @string_builder_append_string(ptr %t1)
  %t3 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t3, i32 7)
  call void @string_builder_append_string(ptr %t3)
  %t4 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t4, i64 %t0)
  %t5 = load { ptr, i64 }, ptr %t4
  call void @$prn({ ptr, i64 } %t5)
  %t6 = call i64 @string_builder_mark()
  %t7 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.5, i64 5 }, ptr %t8
  call void @to_string$string(ptr %t7, ptr %t8)
  call void @string_builder_append_string(ptr %t7)
  %t9 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t9, i32 11)
  call void @string_builder_append_string(ptr %t9)
  %t10 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t10, i64 %t6)
  %t11 = load { ptr, i64 }, ptr %t10
  call void @$prn({ ptr, i64 } %t11)
  %t12 = call i64 @string_builder_mark()
  %t13 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.6, i64 5 }, ptr %t14
  call void @to_string$string(ptr %t13, ptr %t14)
  call void @string_builder_append_string(ptr %t13)
  %t15 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t15, i32 13)
  call void @string_builder_append_string(ptr %t15)
  %t16 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t16, i64 %t12)
  %t17 = load { ptr, i64 }, ptr %t16
  call void @$prn({ ptr, i64 } %t17)
  %t18 = call i64 @string_builder_mark()
  %t19 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.7, i64 9 }, ptr %t20
  call void @to_string$string(ptr %t19, ptr %t20)
  call void @string_builder_append_string(ptr %t19)
  %t21 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t21, i32 9)
  call void @string_builder_append_string(ptr %t21)
  %t22 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t22, i64 %t18)
  %t23 = load { ptr, i64 }, ptr %t22
  call void @$prn({ ptr, i64 } %t23)
  ret void
}

@$main = alias void (), ptr @fn.0
