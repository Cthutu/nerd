use std.io

main :: fn () -> i32 {
    values: [5]i32 = [10, 20, 30, 40, 50]
    all: []i32 = values[..]
    middle: []i32 = values[1..4]
    from_start: []i32 = values[..3]
    to_end: []i32 = values[2..]
    literal: []i32 = [1, 2, 3][..]
    reslice: []i32 = all[1..3]

    prn($"all = {all}")
    prn($"middle = {middle}")
    prn($"from_start = {from_start}")
    prn($"to_end = {to_end}")
    prn($"literal = {literal}")
    prn($"reslice count = {reslice.count}")
    prn($"data first = {middle.data[0]}")

    return middle[1] + from_start[2] + to_end[0] + literal[2]
}
¬
93
¬
all = [10, 20, 30, 40, 50]
middle = [20, 30, 40]
from_start = [10, 20, 30]
to_end = [30, 40, 50]
literal = [1, 2, 3]
reslice count = 2
data first = 20

¬
hir 0
module module.0(054-slices.input)
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
  let values: [5]i32 = [5]i32 array(i32 10, i32 20, i32 30, i32 40, i32 50)
  let all: []i32 = []i32 slice([5]i32 local.0(values), <none>, <none>)
  let middle: []i32 = []i32 slice([5]i32 local.0(values), untyped integer 1, untyped integer 4)
  let from_start: []i32 = []i32 slice([5]i32 local.0(values), <none>, untyped integer 3)
  let to_end: []i32 = []i32 slice([5]i32 local.0(values), untyped integer 2, <none>)
  let literal: []i32 = []i32 slice([3]i32 array(untyped integer 1, i32 2, i32 3), <none>, <none>)
  let reslice: []i32 = []i32 slice([]i32 local.1(all), untyped integer 1, untyped integer 3)
  expr void call bind.2(prn)(string interpolate(<unknown> "all = ", []i32 local.1(all)))
  expr void call bind.2(prn)(string interpolate(<unknown> "middle = ", []i32 local.2(middle)))
  expr void call bind.2(prn)(string interpolate(<unknown> "from_start = ", []i32 local.3(from_start)))
  expr void call bind.2(prn)(string interpolate(<unknown> "to_end = ", []i32 local.4(to_end)))
  expr void call bind.2(prn)(string interpolate(<unknown> "literal = ", []i32 local.5(literal)))
  expr void call bind.2(prn)(string interpolate(<unknown> "reslice count = ", usize field([]i32 local.6(reslice), count)))
  expr void call bind.2(prn)(string interpolate(<unknown> "data first = ", i32 index(^i32 field([]i32 local.2(middle), data), untyped integer 0)))
  return i32 add(i32 add(i32 add(i32 index([]i32 local.2(middle), untyped integer 1), i32 index([]i32 local.3(from_start), untyped integer 2)), i32 index([]i32 local.4(to_end), untyped integer 0)), i32 index([]i32 local.5(literal), untyped integer 2))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [7 x i8] c"all = \00"
@.str.m0.1 = private unnamed_addr constant [10 x i8] c"middle = \00"
@.str.m0.2 = private unnamed_addr constant [14 x i8] c"from_start = \00"
@.str.m0.3 = private unnamed_addr constant [10 x i8] c"to_end = \00"
@.str.m0.4 = private unnamed_addr constant [11 x i8] c"literal = \00"
@.str.m0.5 = private unnamed_addr constant [17 x i8] c"reslice count = \00"
@.str.m0.6 = private unnamed_addr constant [14 x i8] c"data first = \00"

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
  %t0 = insertvalue [5 x i32] poison, i32 10, 0
  %t1 = insertvalue [5 x i32] %t0, i32 20, 1
  %t2 = insertvalue [5 x i32] %t1, i32 30, 2
  %t3 = insertvalue [5 x i32] %t2, i32 40, 3
  %t4 = insertvalue [5 x i32] %t3, i32 50, 4
  %local.0 = alloca [5 x i32]
  store [5 x i32] %t4, ptr %local.0
  %t5 = getelementptr inbounds [5 x i32], ptr %local.0, i64 0, i64 0
  %t6 = insertvalue { ptr, i64 } poison, ptr %t5, 0
  %t7 = insertvalue { ptr, i64 } %t6, i64 5, 1
  %t8 = getelementptr inbounds [5 x i32], ptr %local.0, i64 0, i64 1
  %t9 = insertvalue { ptr, i64 } poison, ptr %t8, 0
  %t10 = insertvalue { ptr, i64 } %t9, i64 3, 1
  %t11 = getelementptr inbounds [5 x i32], ptr %local.0, i64 0, i64 0
  %t12 = insertvalue { ptr, i64 } poison, ptr %t11, 0
  %t13 = insertvalue { ptr, i64 } %t12, i64 3, 1
  %t14 = getelementptr inbounds [5 x i32], ptr %local.0, i64 0, i64 2
  %t15 = insertvalue { ptr, i64 } poison, ptr %t14, 0
  %t16 = insertvalue { ptr, i64 } %t15, i64 3, 1
  %t17 = insertvalue [3 x i32] poison, i32 1, 0
  %t18 = insertvalue [3 x i32] %t17, i32 2, 1
  %t19 = insertvalue [3 x i32] %t18, i32 3, 2
  %t20 = alloca [3 x i32]
  store [3 x i32] %t19, ptr %t20
  %t21 = getelementptr inbounds [3 x i32], ptr %t20, i64 0, i64 0
  %t22 = insertvalue { ptr, i64 } poison, ptr %t21, 0
  %t23 = insertvalue { ptr, i64 } %t22, i64 3, 1
  %t24 = extractvalue { ptr, i64 } %t7, 0
  %t25 = extractvalue { ptr, i64 } %t7, 1
  %t26 = getelementptr inbounds i32, ptr %t24, i64 1
  %t27 = insertvalue { ptr, i64 } poison, ptr %t26, 0
  %t28 = insertvalue { ptr, i64 } %t27, i64 2, 1
  %t29 = call i64 @string_builder_mark()
  %t30 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 6 })
  call void @string_builder_append_string({ ptr, i64 } %t30)
  call void @string_builder_append_byte(i8 91)
  %t31 = extractvalue { ptr, i64 } %t7, 0
  %t32 = extractvalue { ptr, i64 } %t7, 1
  %t33 = alloca i64
  store i64 0, ptr %t33
  br label %slice.string.cond.0
slice.string.cond.0:
  %t34 = load i64, ptr %t33
  %t35 = icmp ult i64 %t34, %t32
  br i1 %t35, label %slice.string.body.1, label %slice.string.end.4
slice.string.body.1:
  %t36 = icmp ne i64 %t34, 0
  br i1 %t36, label %slice.string.sep.2, label %slice.string.item.3
slice.string.sep.2:
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  br label %slice.string.item.3
slice.string.item.3:
  %t37 = getelementptr inbounds i32, ptr %t31, i64 %t34
  %t38 = load i32, ptr %t37
  %t39 = call { ptr, i64 } @to_string$i32(i32 %t38)
  call void @string_builder_append_string({ ptr, i64 } %t39)
  %t40 = add i64 %t34, 1
  store i64 %t40, ptr %t33
  br label %slice.string.cond.0
slice.string.end.4:
  call void @string_builder_append_byte(i8 93)
  %t41 = call { ptr, i64 } @string_builder_finish(i64 %t29)
  call void @$prn({ ptr, i64 } %t41)
  %t42 = call i64 @string_builder_mark()
  %t43 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 9 })
  call void @string_builder_append_string({ ptr, i64 } %t43)
  call void @string_builder_append_byte(i8 91)
  %t44 = extractvalue { ptr, i64 } %t10, 0
  %t45 = extractvalue { ptr, i64 } %t10, 1
  %t46 = alloca i64
  store i64 0, ptr %t46
  br label %slice.string.cond.5
slice.string.cond.5:
  %t47 = load i64, ptr %t46
  %t48 = icmp ult i64 %t47, %t45
  br i1 %t48, label %slice.string.body.6, label %slice.string.end.9
slice.string.body.6:
  %t49 = icmp ne i64 %t47, 0
  br i1 %t49, label %slice.string.sep.7, label %slice.string.item.8
slice.string.sep.7:
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  br label %slice.string.item.8
slice.string.item.8:
  %t50 = getelementptr inbounds i32, ptr %t44, i64 %t47
  %t51 = load i32, ptr %t50
  %t52 = call { ptr, i64 } @to_string$i32(i32 %t51)
  call void @string_builder_append_string({ ptr, i64 } %t52)
  %t53 = add i64 %t47, 1
  store i64 %t53, ptr %t46
  br label %slice.string.cond.5
slice.string.end.9:
  call void @string_builder_append_byte(i8 93)
  %t54 = call { ptr, i64 } @string_builder_finish(i64 %t42)
  call void @$prn({ ptr, i64 } %t54)
  %t55 = call i64 @string_builder_mark()
  %t56 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 13 })
  call void @string_builder_append_string({ ptr, i64 } %t56)
  call void @string_builder_append_byte(i8 91)
  %t57 = extractvalue { ptr, i64 } %t13, 0
  %t58 = extractvalue { ptr, i64 } %t13, 1
  %t59 = alloca i64
  store i64 0, ptr %t59
  br label %slice.string.cond.10
slice.string.cond.10:
  %t60 = load i64, ptr %t59
  %t61 = icmp ult i64 %t60, %t58
  br i1 %t61, label %slice.string.body.11, label %slice.string.end.14
slice.string.body.11:
  %t62 = icmp ne i64 %t60, 0
  br i1 %t62, label %slice.string.sep.12, label %slice.string.item.13
slice.string.sep.12:
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  br label %slice.string.item.13
slice.string.item.13:
  %t63 = getelementptr inbounds i32, ptr %t57, i64 %t60
  %t64 = load i32, ptr %t63
  %t65 = call { ptr, i64 } @to_string$i32(i32 %t64)
  call void @string_builder_append_string({ ptr, i64 } %t65)
  %t66 = add i64 %t60, 1
  store i64 %t66, ptr %t59
  br label %slice.string.cond.10
slice.string.end.14:
  call void @string_builder_append_byte(i8 93)
  %t67 = call { ptr, i64 } @string_builder_finish(i64 %t55)
  call void @$prn({ ptr, i64 } %t67)
  %t68 = call i64 @string_builder_mark()
  %t69 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.3, i64 9 })
  call void @string_builder_append_string({ ptr, i64 } %t69)
  call void @string_builder_append_byte(i8 91)
  %t70 = extractvalue { ptr, i64 } %t16, 0
  %t71 = extractvalue { ptr, i64 } %t16, 1
  %t72 = alloca i64
  store i64 0, ptr %t72
  br label %slice.string.cond.15
slice.string.cond.15:
  %t73 = load i64, ptr %t72
  %t74 = icmp ult i64 %t73, %t71
  br i1 %t74, label %slice.string.body.16, label %slice.string.end.19
slice.string.body.16:
  %t75 = icmp ne i64 %t73, 0
  br i1 %t75, label %slice.string.sep.17, label %slice.string.item.18
slice.string.sep.17:
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  br label %slice.string.item.18
slice.string.item.18:
  %t76 = getelementptr inbounds i32, ptr %t70, i64 %t73
  %t77 = load i32, ptr %t76
  %t78 = call { ptr, i64 } @to_string$i32(i32 %t77)
  call void @string_builder_append_string({ ptr, i64 } %t78)
  %t79 = add i64 %t73, 1
  store i64 %t79, ptr %t72
  br label %slice.string.cond.15
slice.string.end.19:
  call void @string_builder_append_byte(i8 93)
  %t80 = call { ptr, i64 } @string_builder_finish(i64 %t68)
  call void @$prn({ ptr, i64 } %t80)
  %t81 = call i64 @string_builder_mark()
  %t82 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.4, i64 10 })
  call void @string_builder_append_string({ ptr, i64 } %t82)
  call void @string_builder_append_byte(i8 91)
  %t83 = extractvalue { ptr, i64 } %t23, 0
  %t84 = extractvalue { ptr, i64 } %t23, 1
  %t85 = alloca i64
  store i64 0, ptr %t85
  br label %slice.string.cond.20
slice.string.cond.20:
  %t86 = load i64, ptr %t85
  %t87 = icmp ult i64 %t86, %t84
  br i1 %t87, label %slice.string.body.21, label %slice.string.end.24
slice.string.body.21:
  %t88 = icmp ne i64 %t86, 0
  br i1 %t88, label %slice.string.sep.22, label %slice.string.item.23
slice.string.sep.22:
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  br label %slice.string.item.23
slice.string.item.23:
  %t89 = getelementptr inbounds i32, ptr %t83, i64 %t86
  %t90 = load i32, ptr %t89
  %t91 = call { ptr, i64 } @to_string$i32(i32 %t90)
  call void @string_builder_append_string({ ptr, i64 } %t91)
  %t92 = add i64 %t86, 1
  store i64 %t92, ptr %t85
  br label %slice.string.cond.20
slice.string.end.24:
  call void @string_builder_append_byte(i8 93)
  %t93 = call { ptr, i64 } @string_builder_finish(i64 %t81)
  call void @$prn({ ptr, i64 } %t93)
  %t94 = call i64 @string_builder_mark()
  %t95 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.5, i64 16 })
  call void @string_builder_append_string({ ptr, i64 } %t95)
  %t96 = extractvalue { ptr, i64 } %t28, 1
  %t97 = call { ptr, i64 } @to_string$usize(i64 %t96)
  call void @string_builder_append_string({ ptr, i64 } %t97)
  %t98 = call { ptr, i64 } @string_builder_finish(i64 %t94)
  call void @$prn({ ptr, i64 } %t98)
  %t99 = call i64 @string_builder_mark()
  %t100 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.6, i64 13 })
  call void @string_builder_append_string({ ptr, i64 } %t100)
  %t101 = extractvalue { ptr, i64 } %t10, 0
  %t102 = getelementptr inbounds i32, ptr %t101, i32 0
  %t103 = load i32, ptr %t102
  %t104 = call { ptr, i64 } @to_string$i32(i32 %t103)
  call void @string_builder_append_string({ ptr, i64 } %t104)
  %t105 = call { ptr, i64 } @string_builder_finish(i64 %t99)
  call void @$prn({ ptr, i64 } %t105)
  %t106 = extractvalue { ptr, i64 } %t10, 0
  %t107 = getelementptr inbounds i32, ptr %t106, i32 1
  %t108 = load i32, ptr %t107
  %t109 = extractvalue { ptr, i64 } %t13, 0
  %t110 = getelementptr inbounds i32, ptr %t109, i32 2
  %t111 = load i32, ptr %t110
  %t112 = add i32 %t108, %t111
  %t113 = extractvalue { ptr, i64 } %t16, 0
  %t114 = getelementptr inbounds i32, ptr %t113, i32 0
  %t115 = load i32, ptr %t114
  %t116 = add i32 %t112, %t115
  %t117 = extractvalue { ptr, i64 } %t23, 0
  %t118 = getelementptr inbounds i32, ptr %t117, i32 2
  %t119 = load i32, ptr %t118
  %t120 = add i32 %t116, %t119
  ret i32 %t120
}

@$main = alias i32 (), ptr @fn.0
