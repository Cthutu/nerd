use std.io

make_pair :: fn(a: i32, b: string) -> (i32, string) {
    return (a, b)
}

main :: fn () {
    pair :: (7, "seven")
    single :: (pair.0 + 1,)
    from_fn :: make_pair(3, "three")
    nested :: (pair, single, yes)

    prn($"pair = {pair.0}, {pair.1}")
    prn($"pair tuple = {pair}")
    prn($"single = {single.0}")
    prn($"single tuple = {single}")
    prn($"from_fn = {from_fn.0}, {from_fn.1}")
    prn($"nested tuple = {nested}")

    return pair.0 + single.0 + from_fn.0
}
¬
18
¬
pair = 7, seven
pair tuple = (7, seven)
single = 8
single tuple = (8,)
from_fn = 3, three
nested tuple = ((7, seven), (8,), yes)

¬
hir 0
module module.0(051-tuples.input)
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
bind make_pair = fn.0
bind main = fn.1
func fn.0(a: i32, b: string) -> (i32, string) {
  return (i32, string) tuple(i32 local.0(a), string local.1(b))
}
func fn.1() -> i32 {
  let pair: (i32, string) = (i32, string) tuple(untyped integer 7, string "seven")
  let single: (i32,) = (i32,) tuple(i32 add(i32 tuple_field((i32, string) local.2(pair), 0), i32 1))
  let from_fn: (i32, string) = (i32, string) call bind.5(make_pair)(i32 3, string "three")
  let nested: ((i32, string)(i32, string),  = ((i32, string)(i32, string),  tuple((i32, string) local.2(pair), (i32,) local.3(single), bool yes)
  expr void call bind.2(prn)(string interpolate(<unknown> "pair = ", i32 tuple_field((i32, string) local.2(pair), 0), <unknown> ", ", string tuple_field((i32, string) local.2(pair), 1)))
  expr void call bind.2(prn)(string interpolate(<unknown> "pair tuple = ", (i32, string) local.2(pair)))
  expr void call bind.2(prn)(string interpolate(<unknown> "single = ", i32 tuple_field((i32,) local.3(single), 0)))
  expr void call bind.2(prn)(string interpolate(<unknown> "single tuple = ", (i32,) local.3(single)))
  expr void call bind.2(prn)(string interpolate(<unknown> "from_fn = ", i32 tuple_field((i32, string) local.4(from_fn), 0), <unknown> ", ", string tuple_field((i32, string) local.4(from_fn), 1)))
  expr void call bind.2(prn)(string interpolate(<unknown> "nested tuple = ", ((i32, string)(i32, string),  local.5(nested)))
  return i32 add(i32 add(i32 tuple_field((i32, string) local.2(pair), 0), i32 tuple_field((i32,) local.3(single), 0)), i32 tuple_field((i32, string) local.4(from_fn), 0))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [6 x i8] c"seven\00"
@.str.m0.1 = private unnamed_addr constant [6 x i8] c"three\00"
@.str.m0.2 = private unnamed_addr constant [8 x i8] c"pair = \00"
@.str.m0.3 = private unnamed_addr constant [3 x i8] c", \00"
@.str.m0.4 = private unnamed_addr constant [14 x i8] c"pair tuple = \00"
@.str.m0.5 = private unnamed_addr constant [10 x i8] c"single = \00"
@.str.m0.6 = private unnamed_addr constant [16 x i8] c"single tuple = \00"
@.str.m0.7 = private unnamed_addr constant [11 x i8] c"from_fn = \00"
@.str.m0.8 = private unnamed_addr constant [3 x i8] c", \00"
@.str.m0.9 = private unnamed_addr constant [16 x i8] c"nested tuple = \00"

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

define { i32, { ptr, i64 } } @fn.0(i32 %a, { ptr, i64 } %b) {
  %t0 = insertvalue { i32, { ptr, i64 } } poison, i32 %a, 0
  %t1 = insertvalue { i32, { ptr, i64 } } %t0, { ptr, i64 } %b, 1
  ret { i32, { ptr, i64 } } %t1
}

define i32 @fn.1() {
  %t0 = insertvalue { i32, { ptr, i64 } } poison, i32 7, 0
  %t1 = insertvalue { i32, { ptr, i64 } } %t0, { ptr, i64 } { ptr @.str.m0.0, i64 5 }, 1
  %t2 = extractvalue { i32, { ptr, i64 } } %t1, 0
  %t3 = add i32 %t2, 1
  %t4 = insertvalue { i32 } poison, i32 %t3, 0
  %t5 = call { i32, { ptr, i64 } } @fn.0(i32 3, { ptr, i64 } { ptr @.str.m0.1, i64 5 })
  %t6 = insertvalue { { i32, { ptr, i64 } }, { i32 }, i1 } poison, { i32, { ptr, i64 } } %t1, 0
  %t7 = insertvalue { { i32, { ptr, i64 } }, { i32 }, i1 } %t6, { i32 } %t4, 1
  %t8 = insertvalue { { i32, { ptr, i64 } }, { i32 }, i1 } %t7, i1 1, 2
  %t9 = call i64 @string_builder_mark()
  %t10 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 7 })
  call void @string_builder_append_string({ ptr, i64 } %t10)
  %t11 = extractvalue { i32, { ptr, i64 } } %t1, 0
  %t12 = call { ptr, i64 } @to_string$i32(i32 %t11)
  call void @string_builder_append_string({ ptr, i64 } %t12)
  %t13 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.3, i64 2 })
  call void @string_builder_append_string({ ptr, i64 } %t13)
  %t14 = extractvalue { i32, { ptr, i64 } } %t1, 1
  %t15 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t14)
  call void @string_builder_append_string({ ptr, i64 } %t15)
  %t16 = call { ptr, i64 } @string_builder_finish(i64 %t9)
  call void @$prn({ ptr, i64 } %t16)
  %t17 = call i64 @string_builder_mark()
  %t18 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.4, i64 13 })
  call void @string_builder_append_string({ ptr, i64 } %t18)
  call void @string_builder_append_byte(i8 40)
  %t19 = extractvalue { i32, { ptr, i64 } } %t1, 0
  %t20 = call { ptr, i64 } @to_string$i32(i32 %t19)
  call void @string_builder_append_string({ ptr, i64 } %t20)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t21 = extractvalue { i32, { ptr, i64 } } %t1, 1
  %t22 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t21)
  call void @string_builder_append_string({ ptr, i64 } %t22)
  call void @string_builder_append_byte(i8 41)
  %t23 = call { ptr, i64 } @string_builder_finish(i64 %t17)
  call void @$prn({ ptr, i64 } %t23)
  %t24 = call i64 @string_builder_mark()
  %t25 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.5, i64 9 })
  call void @string_builder_append_string({ ptr, i64 } %t25)
  %t26 = extractvalue { i32 } %t4, 0
  %t27 = call { ptr, i64 } @to_string$i32(i32 %t26)
  call void @string_builder_append_string({ ptr, i64 } %t27)
  %t28 = call { ptr, i64 } @string_builder_finish(i64 %t24)
  call void @$prn({ ptr, i64 } %t28)
  %t29 = call i64 @string_builder_mark()
  %t30 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.6, i64 15 })
  call void @string_builder_append_string({ ptr, i64 } %t30)
  call void @string_builder_append_byte(i8 40)
  %t31 = extractvalue { i32 } %t4, 0
  %t32 = call { ptr, i64 } @to_string$i32(i32 %t31)
  call void @string_builder_append_string({ ptr, i64 } %t32)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 41)
  %t33 = call { ptr, i64 } @string_builder_finish(i64 %t29)
  call void @$prn({ ptr, i64 } %t33)
  %t34 = call i64 @string_builder_mark()
  %t35 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.7, i64 10 })
  call void @string_builder_append_string({ ptr, i64 } %t35)
  %t36 = extractvalue { i32, { ptr, i64 } } %t5, 0
  %t37 = call { ptr, i64 } @to_string$i32(i32 %t36)
  call void @string_builder_append_string({ ptr, i64 } %t37)
  %t38 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.8, i64 2 })
  call void @string_builder_append_string({ ptr, i64 } %t38)
  %t39 = extractvalue { i32, { ptr, i64 } } %t5, 1
  %t40 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t39)
  call void @string_builder_append_string({ ptr, i64 } %t40)
  %t41 = call { ptr, i64 } @string_builder_finish(i64 %t34)
  call void @$prn({ ptr, i64 } %t41)
  %t42 = call i64 @string_builder_mark()
  %t43 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.9, i64 15 })
  call void @string_builder_append_string({ ptr, i64 } %t43)
  call void @string_builder_append_byte(i8 40)
  %t44 = extractvalue { { i32, { ptr, i64 } }, { i32 }, i1 } %t8, 0
  call void @string_builder_append_byte(i8 40)
  %t45 = extractvalue { i32, { ptr, i64 } } %t44, 0
  %t46 = call { ptr, i64 } @to_string$i32(i32 %t45)
  call void @string_builder_append_string({ ptr, i64 } %t46)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t47 = extractvalue { i32, { ptr, i64 } } %t44, 1
  %t48 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t47)
  call void @string_builder_append_string({ ptr, i64 } %t48)
  call void @string_builder_append_byte(i8 41)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t49 = extractvalue { { i32, { ptr, i64 } }, { i32 }, i1 } %t8, 1
  call void @string_builder_append_byte(i8 40)
  %t50 = extractvalue { i32 } %t49, 0
  %t51 = call { ptr, i64 } @to_string$i32(i32 %t50)
  call void @string_builder_append_string({ ptr, i64 } %t51)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 41)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t52 = extractvalue { { i32, { ptr, i64 } }, { i32 }, i1 } %t8, 2
  %t53 = call { ptr, i64 } @to_string$bool(i1 %t52)
  call void @string_builder_append_string({ ptr, i64 } %t53)
  call void @string_builder_append_byte(i8 41)
  %t54 = call { ptr, i64 } @string_builder_finish(i64 %t42)
  call void @$prn({ ptr, i64 } %t54)
  %t55 = extractvalue { i32, { ptr, i64 } } %t1, 0
  %t56 = extractvalue { i32 } %t4, 0
  %t57 = add i32 %t55, %t56
  %t58 = extractvalue { i32, { ptr, i64 } } %t5, 0
  %t59 = add i32 %t57, %t58
  ret i32 %t59
}

@$make_pair = internal alias { i32, { ptr, i64 } } (i32, { ptr, i64 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
