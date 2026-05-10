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
  %t7 = extractvalue [3 x i32] %t2, 0
  %t8 = call { ptr, i64 } @to_string$i32(i32 %t7)
  call void @string_builder_append_string({ ptr, i64 } %t8)
  %t9 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 2 })
  call void @string_builder_append_string({ ptr, i64 } %t9)
  %t10 = extractvalue [3 x i32] %t2, 2
  %t11 = call { ptr, i64 } @to_string$i32(i32 %t10)
  call void @string_builder_append_string({ ptr, i64 } %t11)
  %t12 = call { ptr, i64 } @string_builder_finish(i64 %t5)
  call void @$prn({ ptr, i64 } %t12)
  %t13 = call i64 @string_builder_mark()
  %t14 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 11 })
  call void @string_builder_append_string({ ptr, i64 } %t14)
  call void @string_builder_append_byte(i8 91)
  %t15 = extractvalue [3 x i32] %t2, 0
  %t16 = call { ptr, i64 } @to_string$i32(i32 %t15)
  call void @string_builder_append_string({ ptr, i64 } %t16)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t17 = extractvalue [3 x i32] %t2, 1
  %t18 = call { ptr, i64 } @to_string$i32(i32 %t17)
  call void @string_builder_append_string({ ptr, i64 } %t18)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t19 = extractvalue [3 x i32] %t2, 2
  %t20 = call { ptr, i64 } @to_string$i32(i32 %t19)
  call void @string_builder_append_string({ ptr, i64 } %t20)
  call void @string_builder_append_byte(i8 93)
  %t21 = call { ptr, i64 } @string_builder_finish(i64 %t13)
  call void @$prn({ ptr, i64 } %t21)
  %t22 = call i64 @string_builder_mark()
  %t23 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.3, i64 5 })
  call void @string_builder_append_string({ ptr, i64 } %t23)
  %t24 = extractvalue [2 x i32] %t4, 1
  %t25 = call { ptr, i64 } @to_string$i32(i32 %t24)
  call void @string_builder_append_string({ ptr, i64 } %t25)
  %t26 = call { ptr, i64 } @string_builder_finish(i64 %t22)
  call void @$prn({ ptr, i64 } %t26)
  %t27 = call i64 @string_builder_mark()
  %t28 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.4, i64 11 })
  call void @string_builder_append_string({ ptr, i64 } %t28)
  call void @string_builder_append_byte(i8 91)
  %t29 = extractvalue [2 x i32] %t4, 0
  %t30 = call { ptr, i64 } @to_string$i32(i32 %t29)
  call void @string_builder_append_string({ ptr, i64 } %t30)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t31 = extractvalue [2 x i32] %t4, 1
  %t32 = call { ptr, i64 } @to_string$i32(i32 %t31)
  call void @string_builder_append_string({ ptr, i64 } %t32)
  call void @string_builder_append_byte(i8 93)
  %t33 = call { ptr, i64 } @string_builder_finish(i64 %t27)
  call void @$prn({ ptr, i64 } %t33)
  %t34 = extractvalue [3 x i32] %t2, 1
  %t35 = extractvalue [2 x i32] %t4, 0
  %t36 = add i32 %t34, %t35
  %t37 = extractvalue [2 x i32] %t4, 1
  %t38 = add i32 %t36, %t37
  ret i32 %t38
}

@$main = alias i32 (), ptr @fn.0
