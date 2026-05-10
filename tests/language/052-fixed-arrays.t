use std.io

main :: fn () -> i32 {
    xs :: [1, 2, 3]
    ys: [2]i32 = [4, 5]

    prn($"xs = {xs[0]}, {xs[2]}")
    prn($"xs array = {xs}")
    prn($"ys = {ys[1]}")
    prn($"ys array = {ys}")

    return xs[1] + ys[0] + ys[1]
}
¬
11
¬
xs = 1, 3
xs array = [1, 2, 3]
ys = 5
ys array = [4, 5]

¬
hir 0
module module.0(052-fixed-arrays.input)
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
  let xs: [3]i32 = [3]i32 array(untyped integer 1, i32 2, i32 3)
  let ys: [2]i32 = [2]i32 array(i32 4, i32 5)
  expr void call bind.2(prn)(string interpolate(<unknown> "xs = ", i32 index([3]i32 local.0(xs), untyped integer 0), <unknown> ", ", i32 index([3]i32 local.0(xs), untyped integer 2)))
  expr void call bind.2(prn)(string interpolate(<unknown> "xs array = ", [3]i32 local.0(xs)))
  expr void call bind.2(prn)(string interpolate(<unknown> "ys = ", i32 index([2]i32 local.1(ys), untyped integer 1)))
  expr void call bind.2(prn)(string interpolate(<unknown> "ys array = ", [2]i32 local.1(ys)))
  return i32 add(i32 add(i32 index([3]i32 local.0(xs), untyped integer 1), i32 index([2]i32 local.1(ys), untyped integer 0)), i32 index([2]i32 local.1(ys), untyped integer 1))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [6 x i8] c"xs = \00"
@.str.m0.1 = private unnamed_addr constant [3 x i8] c", \00"
@.str.m0.2 = private unnamed_addr constant [12 x i8] c"xs array = \00"
@.str.m0.3 = private unnamed_addr constant [6 x i8] c"ys = \00"
@.str.m0.4 = private unnamed_addr constant [12 x i8] c"ys array = \00"

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
  %t0 = insertvalue [3 x i32] poison, i32 1, 0
  %t1 = insertvalue [3 x i32] %t0, i32 2, 1
  %t2 = insertvalue [3 x i32] %t1, i32 3, 2
  %t3 = insertvalue [2 x i32] poison, i32 4, 0
  %t4 = insertvalue [2 x i32] %t3, i32 5, 1
  %t5 = call i64 @string_builder_mark()
  %t6 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 5 })
  call void @string_builder_append_string({ ptr, i64 } %t6)
  %local.0 = alloca [3 x i32]
  store [3 x i32] %t2, ptr %local.0
  %t7 = getelementptr inbounds [3 x i32], ptr %local.0, i64 0, i32 0
  %t8 = load i32, ptr %t7
  %t9 = call { ptr, i64 } @to_string$i32(i32 %t8)
  call void @string_builder_append_string({ ptr, i64 } %t9)
  %t10 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 2 })
  call void @string_builder_append_string({ ptr, i64 } %t10)
  %t11 = load [3 x i32], ptr %local.0
  %t12 = getelementptr inbounds [3 x i32], ptr %local.0, i64 0, i32 2
  %t13 = load i32, ptr %t12
  %t14 = call { ptr, i64 } @to_string$i32(i32 %t13)
  call void @string_builder_append_string({ ptr, i64 } %t14)
  %t15 = call { ptr, i64 } @string_builder_finish(i64 %t5)
  call void @$prn({ ptr, i64 } %t15)
  %t16 = call i64 @string_builder_mark()
  %t17 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 11 })
  call void @string_builder_append_string({ ptr, i64 } %t17)
  %t18 = load [3 x i32], ptr %local.0
  call void @string_builder_append_byte(i8 91)
  %t19 = extractvalue [3 x i32] %t18, 0
  %t20 = call { ptr, i64 } @to_string$i32(i32 %t19)
  call void @string_builder_append_string({ ptr, i64 } %t20)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t21 = extractvalue [3 x i32] %t18, 1
  %t22 = call { ptr, i64 } @to_string$i32(i32 %t21)
  call void @string_builder_append_string({ ptr, i64 } %t22)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t23 = extractvalue [3 x i32] %t18, 2
  %t24 = call { ptr, i64 } @to_string$i32(i32 %t23)
  call void @string_builder_append_string({ ptr, i64 } %t24)
  call void @string_builder_append_byte(i8 93)
  %t25 = call { ptr, i64 } @string_builder_finish(i64 %t16)
  call void @$prn({ ptr, i64 } %t25)
  %t26 = call i64 @string_builder_mark()
  %t27 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.3, i64 5 })
  call void @string_builder_append_string({ ptr, i64 } %t27)
  %local.1 = alloca [2 x i32]
  store [2 x i32] %t4, ptr %local.1
  %t28 = getelementptr inbounds [2 x i32], ptr %local.1, i64 0, i32 1
  %t29 = load i32, ptr %t28
  %t30 = call { ptr, i64 } @to_string$i32(i32 %t29)
  call void @string_builder_append_string({ ptr, i64 } %t30)
  %t31 = call { ptr, i64 } @string_builder_finish(i64 %t26)
  call void @$prn({ ptr, i64 } %t31)
  %t32 = call i64 @string_builder_mark()
  %t33 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.4, i64 11 })
  call void @string_builder_append_string({ ptr, i64 } %t33)
  %t34 = load [2 x i32], ptr %local.1
  call void @string_builder_append_byte(i8 91)
  %t35 = extractvalue [2 x i32] %t34, 0
  %t36 = call { ptr, i64 } @to_string$i32(i32 %t35)
  call void @string_builder_append_string({ ptr, i64 } %t36)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t37 = extractvalue [2 x i32] %t34, 1
  %t38 = call { ptr, i64 } @to_string$i32(i32 %t37)
  call void @string_builder_append_string({ ptr, i64 } %t38)
  call void @string_builder_append_byte(i8 93)
  %t39 = call { ptr, i64 } @string_builder_finish(i64 %t32)
  call void @$prn({ ptr, i64 } %t39)
  %t40 = load [3 x i32], ptr %local.0
  %t41 = getelementptr inbounds [3 x i32], ptr %local.0, i64 0, i32 1
  %t42 = load i32, ptr %t41
  %t43 = load [2 x i32], ptr %local.1
  %t44 = getelementptr inbounds [2 x i32], ptr %local.1, i64 0, i32 0
  %t45 = load i32, ptr %t44
  %t46 = add i32 %t42, %t45
  %t47 = load [2 x i32], ptr %local.1
  %t48 = getelementptr inbounds [2 x i32], ptr %local.1, i64 0, i32 1
  %t49 = load i32, ptr %t48
  %t50 = add i32 %t46, %t49
  ret i32 %t50
}

@$main = alias i32 (), ptr @fn.0