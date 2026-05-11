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
  destructure let (i32, i32) tuple(untyped integer 2, untyped integer 3) { .0 -> a: i32, .1 -> b: i32 }
  expr void call bind.2(prn)(string interpolate(i32 local.0(a), <unknown> " ", i32 local.1(b)))
  destructure assign (i32, i32) tuple(i32 local.1(b), i32 local.0(a)) { .0 -> a: i32, .1 -> b: i32 }
  expr void call bind.2(prn)(string interpolate(i32 local.0(a), <unknown> " ", i32 local.1(b)))
  destructure assign (i32, i32) tuple(untyped integer 7, untyped integer 8) { .0 -> a: i32 }
  expr void call bind.2(prn)(string interpolate(i32 local.0(a), <unknown> " ", i32 local.1(b)))
  destructure let (i32, string) tuple(untyped integer 4, string "four") { .0 -> c: i32, .1 -> d: string }
  expr void call bind.2(prn)(string interpolate(i32 local.2(c), <unknown> " ", string local.3(d)))
  destructure let (i32, string) tuple(i32 5, string "five") { .0 -> e: i32, .1 -> f: string }
  expr void call bind.2(prn)(string interpolate(i32 local.4(e), <unknown> " ", string local.5(f)))
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.2 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.3 = private unnamed_addr constant [5 x i8] c"four\00"
@.str.m0.4 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.5 = private unnamed_addr constant [5 x i8] c"five\00"
@.str.m0.6 = private unnamed_addr constant [2 x i8] c" \00"

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
  %t0 = insertvalue { i32, i32 } poison, i32 2, 0
  %t1 = insertvalue { i32, i32 } %t0, i32 3, 1
  %t2 = extractvalue { i32, i32 } %t1, 0
  %t3 = extractvalue { i32, i32 } %t1, 1
  %local.0 = alloca i32
  store i32 %t2, ptr %local.0
  %local.1 = alloca i32
  store i32 %t3, ptr %local.1
  %t4 = call i64 @string_builder_mark()
  %t5 = load i32, ptr %local.0
  %t6 = call { ptr, i64 } @to_string$i32(i32 %t5)
  call void @string_builder_append_string({ ptr, i64 } %t6)
  %t7 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t7)
  %t8 = load i32, ptr %local.1
  %t9 = call { ptr, i64 } @to_string$i32(i32 %t8)
  call void @string_builder_append_string({ ptr, i64 } %t9)
  %t10 = call { ptr, i64 } @string_builder_finish(i64 %t4)
  call void @$prn({ ptr, i64 } %t10)
  %t11 = load i32, ptr %local.1
  %t12 = load i32, ptr %local.0
  %t13 = insertvalue { i32, i32 } poison, i32 %t11, 0
  %t14 = insertvalue { i32, i32 } %t13, i32 %t12, 1
  %t15 = extractvalue { i32, i32 } %t14, 0
  %t16 = extractvalue { i32, i32 } %t14, 1
  store i32 %t15, ptr %local.0
  store i32 %t16, ptr %local.1
  %t17 = call i64 @string_builder_mark()
  %t18 = load i32, ptr %local.0
  %t19 = call { ptr, i64 } @to_string$i32(i32 %t18)
  call void @string_builder_append_string({ ptr, i64 } %t19)
  %t20 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t20)
  %t21 = load i32, ptr %local.1
  %t22 = call { ptr, i64 } @to_string$i32(i32 %t21)
  call void @string_builder_append_string({ ptr, i64 } %t22)
  %t23 = call { ptr, i64 } @string_builder_finish(i64 %t17)
  call void @$prn({ ptr, i64 } %t23)
  %t24 = insertvalue { i32, i32 } poison, i32 7, 0
  %t25 = insertvalue { i32, i32 } %t24, i32 8, 1
  %t26 = extractvalue { i32, i32 } %t25, 0
  store i32 %t26, ptr %local.0
  %t27 = call i64 @string_builder_mark()
  %t28 = load i32, ptr %local.0
  %t29 = call { ptr, i64 } @to_string$i32(i32 %t28)
  call void @string_builder_append_string({ ptr, i64 } %t29)
  %t30 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t30)
  %t31 = load i32, ptr %local.1
  %t32 = call { ptr, i64 } @to_string$i32(i32 %t31)
  call void @string_builder_append_string({ ptr, i64 } %t32)
  %t33 = call { ptr, i64 } @string_builder_finish(i64 %t27)
  call void @$prn({ ptr, i64 } %t33)
  %t34 = insertvalue { i32, { ptr, i64 } } poison, i32 4, 0
  %t35 = insertvalue { i32, { ptr, i64 } } %t34, { ptr, i64 } { ptr @.str.m0.3, i64 4 }, 1
  %t36 = extractvalue { i32, { ptr, i64 } } %t35, 0
  %t37 = extractvalue { i32, { ptr, i64 } } %t35, 1
  %t38 = call i64 @string_builder_mark()
  %t39 = call { ptr, i64 } @to_string$i32(i32 %t36)
  call void @string_builder_append_string({ ptr, i64 } %t39)
  %t40 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.4, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t40)
  %t41 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t37)
  call void @string_builder_append_string({ ptr, i64 } %t41)
  %t42 = call { ptr, i64 } @string_builder_finish(i64 %t38)
  call void @$prn({ ptr, i64 } %t42)
  %t43 = insertvalue { i32, { ptr, i64 } } poison, i32 5, 0
  %t44 = insertvalue { i32, { ptr, i64 } } %t43, { ptr, i64 } { ptr @.str.m0.5, i64 4 }, 1
  %t45 = extractvalue { i32, { ptr, i64 } } %t44, 0
  %t46 = extractvalue { i32, { ptr, i64 } } %t44, 1
  %t47 = call i64 @string_builder_mark()
  %t48 = call { ptr, i64 } @to_string$i32(i32 %t45)
  call void @string_builder_append_string({ ptr, i64 } %t48)
  %t49 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.6, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t49)
  %t50 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t46)
  call void @string_builder_append_string({ ptr, i64 } %t50)
  %t51 = call { ptr, i64 } @string_builder_finish(i64 %t47)
  call void @$prn({ ptr, i64 } %t51)
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
