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
import import.0 prn from module.2(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.5: fn (string) -> string
bind prn = import.0
bind input = import.1
bind main = fn.0
func fn.0() -> i32 {
  let xs: [3]i32 = [3]i32 array(untyped integer 1, i32 2, i32 3)
  let ys: [2]i32 = [2]i32 array(i32 4, i32 5)
  expr void call bind.0(prn)(string interpolate(<unknown> "xs = ", i32 index([3]i32 local.0(xs), untyped integer 0), <unknown> ", ", i32 index([3]i32 local.0(xs), untyped integer 2)))
  expr void call bind.0(prn)(string interpolate(<unknown> "xs array = ", [3]i32 local.0(xs)))
  expr void call bind.0(prn)(string interpolate(<unknown> "ys = ", i32 index([2]i32 local.1(ys), untyped integer 1)))
  expr void call bind.0(prn)(string interpolate(<unknown> "ys array = ", [2]i32 local.1(ys)))
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
declare { ptr, i64 } @$input({ ptr, i64 })

define internal i32 @fn.0() {
  %t0 = insertvalue [3 x i32] poison, i32 1, 0
  %t1 = insertvalue [3 x i32] %t0, i32 2, 1
  %t2 = insertvalue [3 x i32] %t1, i32 3, 2
  %t3 = insertvalue [2 x i32] poison, i32 4, 0
  %t4 = insertvalue [2 x i32] %t3, i32 5, 1
  %t5 = call i64 @string_builder_mark()
  %t6 = alloca { ptr, i64 }
  %t7 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 5 }, ptr %t7
  call void @to_string$string(ptr %t6, ptr %t7)
  call void @string_builder_append_string(ptr %t6)
  %t8 = extractvalue [3 x i32] %t2, 0
  %t9 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t9, i32 %t8)
  call void @string_builder_append_string(ptr %t9)
  %t10 = alloca { ptr, i64 }
  %t11 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 2 }, ptr %t11
  call void @to_string$string(ptr %t10, ptr %t11)
  call void @string_builder_append_string(ptr %t10)
  %t12 = extractvalue [3 x i32] %t2, 2
  %t13 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t13, i32 %t12)
  call void @string_builder_append_string(ptr %t13)
  %t14 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t14, i64 %t5)
  %t15 = load { ptr, i64 }, ptr %t14
  call void @$prn({ ptr, i64 } %t15)
  %t16 = call i64 @string_builder_mark()
  %t17 = alloca { ptr, i64 }
  %t18 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 11 }, ptr %t18
  call void @to_string$string(ptr %t17, ptr %t18)
  call void @string_builder_append_string(ptr %t17)
  call void @string_builder_append_byte(i8 91)
  %t19 = extractvalue [3 x i32] %t2, 0
  %t20 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t20, i32 %t19)
  call void @string_builder_append_string(ptr %t20)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t21 = extractvalue [3 x i32] %t2, 1
  %t22 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t22, i32 %t21)
  call void @string_builder_append_string(ptr %t22)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t23 = extractvalue [3 x i32] %t2, 2
  %t24 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t24, i32 %t23)
  call void @string_builder_append_string(ptr %t24)
  call void @string_builder_append_byte(i8 93)
  %t25 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t25, i64 %t16)
  %t26 = load { ptr, i64 }, ptr %t25
  call void @$prn({ ptr, i64 } %t26)
  %t27 = call i64 @string_builder_mark()
  %t28 = alloca { ptr, i64 }
  %t29 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 5 }, ptr %t29
  call void @to_string$string(ptr %t28, ptr %t29)
  call void @string_builder_append_string(ptr %t28)
  %t30 = extractvalue [2 x i32] %t4, 1
  %t31 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t31, i32 %t30)
  call void @string_builder_append_string(ptr %t31)
  %t32 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t32, i64 %t27)
  %t33 = load { ptr, i64 }, ptr %t32
  call void @$prn({ ptr, i64 } %t33)
  %t34 = call i64 @string_builder_mark()
  %t35 = alloca { ptr, i64 }
  %t36 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 11 }, ptr %t36
  call void @to_string$string(ptr %t35, ptr %t36)
  call void @string_builder_append_string(ptr %t35)
  call void @string_builder_append_byte(i8 91)
  %t37 = extractvalue [2 x i32] %t4, 0
  %t38 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t38, i32 %t37)
  call void @string_builder_append_string(ptr %t38)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t39 = extractvalue [2 x i32] %t4, 1
  %t40 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t40, i32 %t39)
  call void @string_builder_append_string(ptr %t40)
  call void @string_builder_append_byte(i8 93)
  %t41 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t41, i64 %t34)
  %t42 = load { ptr, i64 }, ptr %t41
  call void @$prn({ ptr, i64 } %t42)
  %t43 = extractvalue [3 x i32] %t2, 1
  %t44 = extractvalue [2 x i32] %t4, 0
  %t45 = add i32 %t43, %t44
  %t46 = extractvalue [2 x i32] %t4, 1
  %t47 = add i32 %t45, %t46
  ret i32 %t47
}

@$main = alias i32 (), ptr @fn.0