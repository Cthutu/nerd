use std.io

main :: fn () -> i32 {
    (a, b) := (2, 3)
    prn($"{a} {b}")
    (a, b) = (b, a)
    prn($"{a} {b}")
    (a, _) = (7, 8)
    prn($"{a} {b}")
    (c, d) :: (4, "four")
    prn($"{c} {d}")
    (e, f): (i32, string) = (5, "five")
    prn($"{e} {f}")
    return 0
}
¬
0
¬
2 3
3 2
7 2
4 four
5 five

¬
hir 0
module module.0(059-destructuring-bindings.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind main = fn.0
func fn.0() -> i32 {
  destructure let (i32, i32) tuple(untyped integer 2, untyped integer 3) { .0 -> a: i32, .1 -> b: i32 }
  expr void call bind.0(prn)(string interpolate(i32 local.0(a), <unknown> " ", i32 local.1(b)))
  destructure assign (i32, i32) tuple(i32 local.1(b), i32 local.0(a)) { .0 -> a: i32, .1 -> b: i32 }
  expr void call bind.0(prn)(string interpolate(i32 local.0(a), <unknown> " ", i32 local.1(b)))
  destructure assign (i32, i32) tuple(untyped integer 7, untyped integer 8) { .0 -> a: i32 }
  expr void call bind.0(prn)(string interpolate(i32 local.0(a), <unknown> " ", i32 local.1(b)))
  destructure let (i32, string) tuple(untyped integer 4, string "four") { .0 -> c: i32, .1 -> d: string }
  expr void call bind.0(prn)(string interpolate(i32 local.2(c), <unknown> " ", string local.3(d)))
  destructure let (i32, string) tuple(i32 5, string "five") { .0 -> e: i32, .1 -> f: string }
  expr void call bind.0(prn)(string interpolate(i32 local.4(e), <unknown> " ", string local.5(f)))
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [44 x i8] c"tests/language/059-destructuring-bindings.t\00"
@.str.m0.0 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.2 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.3 = private unnamed_addr constant [5 x i8] c"four\00"
@.str.m0.4 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.5 = private unnamed_addr constant [5 x i8] c"five\00"
@.str.m0.6 = private unnamed_addr constant [2 x i8] c" \00"

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
  %local.1 = alloca i32
  %t8 = alloca { ptr, i64 }
  %t23 = alloca { ptr, i64 }
  %t35 = alloca { ptr, i64 }
  %t47 = alloca { ptr, i64 }
  %t49 = alloca { ptr, i64 }
  %t59 = alloca { ptr, i64 }
  %t61 = alloca { ptr, i64 }
  %t0 = insertvalue { i32, i32 } poison, i32 2, 0
  %t1 = insertvalue { i32, i32 } %t0, i32 3, 1
  %t2 = extractvalue { i32, i32 } %t1, 0
  %t3 = extractvalue { i32, i32 } %t1, 1
  store i32 %t2, ptr %local.0
  store i32 %t3, ptr %local.1
  %t4 = call i64 @string_builder_mark()
  %t5 = load i32, ptr %local.0
  %t6 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t6, i32 %t5)
  call void @string_builder_append_string(ptr %t6)
  %t7 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 1 }, ptr %t8
  call void @to_string$string(ptr %t7, ptr %t8)
  call void @string_builder_append_string(ptr %t7)
  %t9 = load i32, ptr %local.1
  %t10 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t10, i32 %t9)
  call void @string_builder_append_string(ptr %t10)
  %t11 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t11, i64 %t4)
  %t12 = load { ptr, i64 }, ptr %t11
  call void @$prn({ ptr, i64 } %t12)
  %t13 = load i32, ptr %local.1
  %t14 = load i32, ptr %local.0
  %t15 = insertvalue { i32, i32 } poison, i32 %t13, 0
  %t16 = insertvalue { i32, i32 } %t15, i32 %t14, 1
  %t17 = extractvalue { i32, i32 } %t16, 0
  %t18 = extractvalue { i32, i32 } %t16, 1
  store i32 %t17, ptr %local.0
  store i32 %t18, ptr %local.1
  %t19 = call i64 @string_builder_mark()
  %t20 = load i32, ptr %local.0
  %t21 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t21, i32 %t20)
  call void @string_builder_append_string(ptr %t21)
  %t22 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 1 }, ptr %t23
  call void @to_string$string(ptr %t22, ptr %t23)
  call void @string_builder_append_string(ptr %t22)
  %t24 = load i32, ptr %local.1
  %t25 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t25, i32 %t24)
  call void @string_builder_append_string(ptr %t25)
  %t26 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t26, i64 %t19)
  %t27 = load { ptr, i64 }, ptr %t26
  call void @$prn({ ptr, i64 } %t27)
  %t28 = insertvalue { i32, i32 } poison, i32 7, 0
  %t29 = insertvalue { i32, i32 } %t28, i32 8, 1
  %t30 = extractvalue { i32, i32 } %t29, 0
  store i32 %t30, ptr %local.0
  %t31 = call i64 @string_builder_mark()
  %t32 = load i32, ptr %local.0
  %t33 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t33, i32 %t32)
  call void @string_builder_append_string(ptr %t33)
  %t34 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 1 }, ptr %t35
  call void @to_string$string(ptr %t34, ptr %t35)
  call void @string_builder_append_string(ptr %t34)
  %t36 = load i32, ptr %local.1
  %t37 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t37, i32 %t36)
  call void @string_builder_append_string(ptr %t37)
  %t38 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t38, i64 %t31)
  %t39 = load { ptr, i64 }, ptr %t38
  call void @$prn({ ptr, i64 } %t39)
  %t40 = insertvalue { i32, { ptr, i64 } } poison, i32 4, 0
  %t41 = insertvalue { i32, { ptr, i64 } } %t40, { ptr, i64 } { ptr @.str.m0.3, i64 4 }, 1
  %t42 = extractvalue { i32, { ptr, i64 } } %t41, 0
  %t43 = extractvalue { i32, { ptr, i64 } } %t41, 1
  %t44 = call i64 @string_builder_mark()
  %t45 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t45, i32 %t42)
  call void @string_builder_append_string(ptr %t45)
  %t46 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 1 }, ptr %t47
  call void @to_string$string(ptr %t46, ptr %t47)
  call void @string_builder_append_string(ptr %t46)
  %t48 = alloca { ptr, i64 }
  store { ptr, i64 } %t43, ptr %t49
  call void @to_string$string(ptr %t48, ptr %t49)
  call void @string_builder_append_string(ptr %t48)
  %t50 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t50, i64 %t44)
  %t51 = load { ptr, i64 }, ptr %t50
  call void @$prn({ ptr, i64 } %t51)
  %t52 = insertvalue { i32, { ptr, i64 } } poison, i32 5, 0
  %t53 = insertvalue { i32, { ptr, i64 } } %t52, { ptr, i64 } { ptr @.str.m0.5, i64 4 }, 1
  %t54 = extractvalue { i32, { ptr, i64 } } %t53, 0
  %t55 = extractvalue { i32, { ptr, i64 } } %t53, 1
  %t56 = call i64 @string_builder_mark()
  %t57 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t57, i32 %t54)
  call void @string_builder_append_string(ptr %t57)
  %t58 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.6, i64 1 }, ptr %t59
  call void @to_string$string(ptr %t58, ptr %t59)
  call void @string_builder_append_string(ptr %t58)
  %t60 = alloca { ptr, i64 }
  store { ptr, i64 } %t55, ptr %t61
  call void @to_string$string(ptr %t60, ptr %t61)
  call void @string_builder_append_string(ptr %t60)
  %t62 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t62, i64 %t56)
  %t63 = load { ptr, i64 }, ptr %t62
  call void @$prn({ ptr, i64 } %t63)
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
