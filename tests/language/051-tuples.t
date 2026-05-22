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
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind make_pair = fn.0
bind main = fn.1
func fn.0(a: i32, b: string) -> (i32, string) {
  return (i32, string) tuple(i32 local.0(a), string local.1(b))
}
func fn.1() -> i32 {
  let pair: (i32, string) = (i32, string) tuple(untyped integer 7, string "seven")
  let single: (i32,) = (i32,) tuple(i32 add(i32 tuple_field((i32, string) local.2(pair), 0), i32 1))
  let from_fn: (i32, string) = (i32, string) call bind.2(make_pair)(i32 3, string "three")
  let nested: ((i32, string)(i32, string),  = ((i32, string)(i32, string),  tuple((i32, string) local.2(pair), (i32,) local.3(single), bool yes)
  expr void call bind.0(prn)(string interpolate(<unknown> "pair = ", i32 tuple_field((i32, string) local.2(pair), 0), <unknown> ", ", string tuple_field((i32, string) local.2(pair), 1)))
  expr void call bind.0(prn)(string interpolate(<unknown> "pair tuple = ", (i32, string) local.2(pair)))
  expr void call bind.0(prn)(string interpolate(<unknown> "single = ", i32 tuple_field((i32,) local.3(single), 0)))
  expr void call bind.0(prn)(string interpolate(<unknown> "single tuple = ", (i32,) local.3(single)))
  expr void call bind.0(prn)(string interpolate(<unknown> "from_fn = ", i32 tuple_field((i32, string) local.4(from_fn), 0), <unknown> ", ", string tuple_field((i32, string) local.4(from_fn), 1)))
  expr void call bind.0(prn)(string interpolate(<unknown> "nested tuple = ", ((i32, string)(i32, string),  local.5(nested)))
  return i32 add(i32 add(i32 tuple_field((i32, string) local.2(pair), 0), i32 tuple_field((i32,) local.3(single), 0)), i32 tuple_field((i32, string) local.4(from_fn), 0))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [28 x i8] c"tests/language/051-tuples.t\00"
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

define internal { i32, { ptr, i64 } } @fn.0(i32 %a, { ptr, i64 } %b) {
  %t0 = insertvalue { i32, { ptr, i64 } } poison, i32 %a, 0
  %t1 = insertvalue { i32, { ptr, i64 } } %t0, { ptr, i64 } %b, 1
  ret { i32, { ptr, i64 } } %t1
}

define internal i32 @fn.1() {
  %t11 = alloca { ptr, i64 }
  %t15 = alloca { ptr, i64 }
  %t18 = alloca { ptr, i64 }
  %t23 = alloca { ptr, i64 }
  %t28 = alloca { ptr, i64 }
  %t33 = alloca { ptr, i64 }
  %t40 = alloca { ptr, i64 }
  %t47 = alloca { ptr, i64 }
  %t51 = alloca { ptr, i64 }
  %t54 = alloca { ptr, i64 }
  %t59 = alloca { ptr, i64 }
  %t65 = alloca { ptr, i64 }
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
  %t10 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 7 }, ptr %t11
  call void @to_string$string(ptr %t10, ptr %t11)
  call void @string_builder_append_string(ptr %t10)
  %t12 = extractvalue { i32, { ptr, i64 } } %t1, 0
  %t13 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t13, i32 %t12)
  call void @string_builder_append_string(ptr %t13)
  %t14 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 2 }, ptr %t15
  call void @to_string$string(ptr %t14, ptr %t15)
  call void @string_builder_append_string(ptr %t14)
  %t16 = extractvalue { i32, { ptr, i64 } } %t1, 1
  %t17 = alloca { ptr, i64 }
  store { ptr, i64 } %t16, ptr %t18
  call void @to_string$string(ptr %t17, ptr %t18)
  call void @string_builder_append_string(ptr %t17)
  %t19 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t19, i64 %t9)
  %t20 = load { ptr, i64 }, ptr %t19
  call void @$prn({ ptr, i64 } %t20)
  %t21 = call i64 @string_builder_mark()
  %t22 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 13 }, ptr %t23
  call void @to_string$string(ptr %t22, ptr %t23)
  call void @string_builder_append_string(ptr %t22)
  call void @string_builder_append_byte(i8 40)
  %t24 = extractvalue { i32, { ptr, i64 } } %t1, 0
  %t25 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t25, i32 %t24)
  call void @string_builder_append_string(ptr %t25)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t26 = extractvalue { i32, { ptr, i64 } } %t1, 1
  %t27 = alloca { ptr, i64 }
  store { ptr, i64 } %t26, ptr %t28
  call void @to_string$string(ptr %t27, ptr %t28)
  call void @string_builder_append_string(ptr %t27)
  call void @string_builder_append_byte(i8 41)
  %t29 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t29, i64 %t21)
  %t30 = load { ptr, i64 }, ptr %t29
  call void @$prn({ ptr, i64 } %t30)
  %t31 = call i64 @string_builder_mark()
  %t32 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.5, i64 9 }, ptr %t33
  call void @to_string$string(ptr %t32, ptr %t33)
  call void @string_builder_append_string(ptr %t32)
  %t34 = extractvalue { i32 } %t4, 0
  %t35 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t35, i32 %t34)
  call void @string_builder_append_string(ptr %t35)
  %t36 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t36, i64 %t31)
  %t37 = load { ptr, i64 }, ptr %t36
  call void @$prn({ ptr, i64 } %t37)
  %t38 = call i64 @string_builder_mark()
  %t39 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.6, i64 15 }, ptr %t40
  call void @to_string$string(ptr %t39, ptr %t40)
  call void @string_builder_append_string(ptr %t39)
  call void @string_builder_append_byte(i8 40)
  %t41 = extractvalue { i32 } %t4, 0
  %t42 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t42, i32 %t41)
  call void @string_builder_append_string(ptr %t42)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 41)
  %t43 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t43, i64 %t38)
  %t44 = load { ptr, i64 }, ptr %t43
  call void @$prn({ ptr, i64 } %t44)
  %t45 = call i64 @string_builder_mark()
  %t46 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.7, i64 10 }, ptr %t47
  call void @to_string$string(ptr %t46, ptr %t47)
  call void @string_builder_append_string(ptr %t46)
  %t48 = extractvalue { i32, { ptr, i64 } } %t5, 0
  %t49 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t49, i32 %t48)
  call void @string_builder_append_string(ptr %t49)
  %t50 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.8, i64 2 }, ptr %t51
  call void @to_string$string(ptr %t50, ptr %t51)
  call void @string_builder_append_string(ptr %t50)
  %t52 = extractvalue { i32, { ptr, i64 } } %t5, 1
  %t53 = alloca { ptr, i64 }
  store { ptr, i64 } %t52, ptr %t54
  call void @to_string$string(ptr %t53, ptr %t54)
  call void @string_builder_append_string(ptr %t53)
  %t55 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t55, i64 %t45)
  %t56 = load { ptr, i64 }, ptr %t55
  call void @$prn({ ptr, i64 } %t56)
  %t57 = call i64 @string_builder_mark()
  %t58 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.9, i64 15 }, ptr %t59
  call void @to_string$string(ptr %t58, ptr %t59)
  call void @string_builder_append_string(ptr %t58)
  call void @string_builder_append_byte(i8 40)
  %t60 = extractvalue { { i32, { ptr, i64 } }, { i32 }, i1 } %t8, 0
  call void @string_builder_append_byte(i8 40)
  %t61 = extractvalue { i32, { ptr, i64 } } %t60, 0
  %t62 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t62, i32 %t61)
  call void @string_builder_append_string(ptr %t62)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t63 = extractvalue { i32, { ptr, i64 } } %t60, 1
  %t64 = alloca { ptr, i64 }
  store { ptr, i64 } %t63, ptr %t65
  call void @to_string$string(ptr %t64, ptr %t65)
  call void @string_builder_append_string(ptr %t64)
  call void @string_builder_append_byte(i8 41)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t66 = extractvalue { { i32, { ptr, i64 } }, { i32 }, i1 } %t8, 1
  call void @string_builder_append_byte(i8 40)
  %t67 = extractvalue { i32 } %t66, 0
  %t68 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t68, i32 %t67)
  call void @string_builder_append_string(ptr %t68)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 41)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t69 = extractvalue { { i32, { ptr, i64 } }, { i32 }, i1 } %t8, 2
  %t70 = alloca { ptr, i64 }
  call void @to_string$bool(ptr %t70, i1 zeroext %t69)
  call void @string_builder_append_string(ptr %t70)
  call void @string_builder_append_byte(i8 41)
  %t71 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t71, i64 %t57)
  %t72 = load { ptr, i64 }, ptr %t71
  call void @$prn({ ptr, i64 } %t72)
  %t73 = extractvalue { i32, { ptr, i64 } } %t1, 0
  %t74 = extractvalue { i32 } %t4, 0
  %t75 = add i32 %t73, %t74
  %t76 = extractvalue { i32, { ptr, i64 } } %t5, 0
  %t77 = add i32 %t75, %t76
  ret i32 %t77
}

@$make_pair = internal alias { i32, { ptr, i64 } } (i32, { ptr, i64 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
