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
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind main = fn.0
func fn.0() -> i32 {
  let values: [5]i32 = [5]i32 array(i32 10, i32 20, i32 30, i32 40, i32 50)
  let all: []i32 = []i32 slice([5]i32 local.0(values), <none>, <none>)
  let middle: []i32 = []i32 slice([5]i32 local.0(values), untyped integer 1, untyped integer 4)
  let from_start: []i32 = []i32 slice([5]i32 local.0(values), <none>, untyped integer 3)
  let to_end: []i32 = []i32 slice([5]i32 local.0(values), untyped integer 2, <none>)
  let literal: []i32 = []i32 slice([3]i32 array(untyped integer 1, i32 2, i32 3), <none>, <none>)
  let reslice: []i32 = []i32 slice([]i32 local.1(all), untyped integer 1, untyped integer 3)
  expr void call bind.0(prn)(string interpolate(<unknown> "all = ", []i32 local.1(all)))
  expr void call bind.0(prn)(string interpolate(<unknown> "middle = ", []i32 local.2(middle)))
  expr void call bind.0(prn)(string interpolate(<unknown> "from_start = ", []i32 local.3(from_start)))
  expr void call bind.0(prn)(string interpolate(<unknown> "to_end = ", []i32 local.4(to_end)))
  expr void call bind.0(prn)(string interpolate(<unknown> "literal = ", []i32 local.5(literal)))
  expr void call bind.0(prn)(string interpolate(<unknown> "reslice count = ", usize field([]i32 local.6(reslice), count)))
  expr void call bind.0(prn)(string interpolate(<unknown> "data first = ", i32 index(^i32 field([]i32 local.2(middle), data), untyped integer 0)))
  return i32 add(i32 add(i32 add(i32 index([]i32 local.2(middle), untyped integer 1), i32 index([]i32 local.3(from_start), untyped integer 2)), i32 index([]i32 local.4(to_end), untyped integer 0)), i32 index([]i32 local.5(literal), untyped integer 2))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [50 x i8] c"tests/language/054-slices.t\00"
@.str.m0.0 = private unnamed_addr constant [7 x i8] c"all = \00"
@.str.m0.1 = private unnamed_addr constant [10 x i8] c"middle = \00"
@.str.m0.2 = private unnamed_addr constant [14 x i8] c"from_start = \00"
@.str.m0.3 = private unnamed_addr constant [10 x i8] c"to_end = \00"
@.str.m0.4 = private unnamed_addr constant [11 x i8] c"literal = \00"
@.str.m0.5 = private unnamed_addr constant [17 x i8] c"reslice count = \00"
@.str.m0.6 = private unnamed_addr constant [14 x i8] c"data first = \00"

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
  %t30 = alloca { ptr, i64 }
  %t31 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 6 }, ptr %t31
  call void @to_string$string(ptr %t30, ptr %t31)
  call void @string_builder_append_string(ptr %t30)
  call void @string_builder_append_byte(i8 91)
  %t32 = extractvalue { ptr, i64 } %t7, 0
  %t33 = extractvalue { ptr, i64 } %t7, 1
  %t34 = alloca i64
  store i64 0, ptr %t34
  br label %slice.string.cond.0
slice.string.cond.0:
  %t35 = load i64, ptr %t34
  %t36 = icmp ult i64 %t35, %t33
  br i1 %t36, label %slice.string.body.1, label %slice.string.end.4
slice.string.body.1:
  %t37 = icmp ne i64 %t35, 0
  br i1 %t37, label %slice.string.sep.2, label %slice.string.item.3
slice.string.sep.2:
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  br label %slice.string.item.3
slice.string.item.3:
  %t38 = getelementptr inbounds i32, ptr %t32, i64 %t35
  %t39 = load i32, ptr %t38
  %t40 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t40, i32 %t39)
  call void @string_builder_append_string(ptr %t40)
  %t41 = add i64 %t35, 1
  store i64 %t41, ptr %t34
  br label %slice.string.cond.0
slice.string.end.4:
  call void @string_builder_append_byte(i8 93)
  %t42 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t42, i64 %t29)
  %t43 = load { ptr, i64 }, ptr %t42
  call void @$prn({ ptr, i64 } %t43)
  %t44 = call i64 @string_builder_mark()
  %t45 = alloca { ptr, i64 }
  %t46 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 9 }, ptr %t46
  call void @to_string$string(ptr %t45, ptr %t46)
  call void @string_builder_append_string(ptr %t45)
  call void @string_builder_append_byte(i8 91)
  %t47 = extractvalue { ptr, i64 } %t10, 0
  %t48 = extractvalue { ptr, i64 } %t10, 1
  %t49 = alloca i64
  store i64 0, ptr %t49
  br label %slice.string.cond.5
slice.string.cond.5:
  %t50 = load i64, ptr %t49
  %t51 = icmp ult i64 %t50, %t48
  br i1 %t51, label %slice.string.body.6, label %slice.string.end.9
slice.string.body.6:
  %t52 = icmp ne i64 %t50, 0
  br i1 %t52, label %slice.string.sep.7, label %slice.string.item.8
slice.string.sep.7:
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  br label %slice.string.item.8
slice.string.item.8:
  %t53 = getelementptr inbounds i32, ptr %t47, i64 %t50
  %t54 = load i32, ptr %t53
  %t55 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t55, i32 %t54)
  call void @string_builder_append_string(ptr %t55)
  %t56 = add i64 %t50, 1
  store i64 %t56, ptr %t49
  br label %slice.string.cond.5
slice.string.end.9:
  call void @string_builder_append_byte(i8 93)
  %t57 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t57, i64 %t44)
  %t58 = load { ptr, i64 }, ptr %t57
  call void @$prn({ ptr, i64 } %t58)
  %t59 = call i64 @string_builder_mark()
  %t60 = alloca { ptr, i64 }
  %t61 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 13 }, ptr %t61
  call void @to_string$string(ptr %t60, ptr %t61)
  call void @string_builder_append_string(ptr %t60)
  call void @string_builder_append_byte(i8 91)
  %t62 = extractvalue { ptr, i64 } %t13, 0
  %t63 = extractvalue { ptr, i64 } %t13, 1
  %t64 = alloca i64
  store i64 0, ptr %t64
  br label %slice.string.cond.10
slice.string.cond.10:
  %t65 = load i64, ptr %t64
  %t66 = icmp ult i64 %t65, %t63
  br i1 %t66, label %slice.string.body.11, label %slice.string.end.14
slice.string.body.11:
  %t67 = icmp ne i64 %t65, 0
  br i1 %t67, label %slice.string.sep.12, label %slice.string.item.13
slice.string.sep.12:
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  br label %slice.string.item.13
slice.string.item.13:
  %t68 = getelementptr inbounds i32, ptr %t62, i64 %t65
  %t69 = load i32, ptr %t68
  %t70 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t70, i32 %t69)
  call void @string_builder_append_string(ptr %t70)
  %t71 = add i64 %t65, 1
  store i64 %t71, ptr %t64
  br label %slice.string.cond.10
slice.string.end.14:
  call void @string_builder_append_byte(i8 93)
  %t72 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t72, i64 %t59)
  %t73 = load { ptr, i64 }, ptr %t72
  call void @$prn({ ptr, i64 } %t73)
  %t74 = call i64 @string_builder_mark()
  %t75 = alloca { ptr, i64 }
  %t76 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 9 }, ptr %t76
  call void @to_string$string(ptr %t75, ptr %t76)
  call void @string_builder_append_string(ptr %t75)
  call void @string_builder_append_byte(i8 91)
  %t77 = extractvalue { ptr, i64 } %t16, 0
  %t78 = extractvalue { ptr, i64 } %t16, 1
  %t79 = alloca i64
  store i64 0, ptr %t79
  br label %slice.string.cond.15
slice.string.cond.15:
  %t80 = load i64, ptr %t79
  %t81 = icmp ult i64 %t80, %t78
  br i1 %t81, label %slice.string.body.16, label %slice.string.end.19
slice.string.body.16:
  %t82 = icmp ne i64 %t80, 0
  br i1 %t82, label %slice.string.sep.17, label %slice.string.item.18
slice.string.sep.17:
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  br label %slice.string.item.18
slice.string.item.18:
  %t83 = getelementptr inbounds i32, ptr %t77, i64 %t80
  %t84 = load i32, ptr %t83
  %t85 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t85, i32 %t84)
  call void @string_builder_append_string(ptr %t85)
  %t86 = add i64 %t80, 1
  store i64 %t86, ptr %t79
  br label %slice.string.cond.15
slice.string.end.19:
  call void @string_builder_append_byte(i8 93)
  %t87 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t87, i64 %t74)
  %t88 = load { ptr, i64 }, ptr %t87
  call void @$prn({ ptr, i64 } %t88)
  %t89 = call i64 @string_builder_mark()
  %t90 = alloca { ptr, i64 }
  %t91 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 10 }, ptr %t91
  call void @to_string$string(ptr %t90, ptr %t91)
  call void @string_builder_append_string(ptr %t90)
  call void @string_builder_append_byte(i8 91)
  %t92 = extractvalue { ptr, i64 } %t23, 0
  %t93 = extractvalue { ptr, i64 } %t23, 1
  %t94 = alloca i64
  store i64 0, ptr %t94
  br label %slice.string.cond.20
slice.string.cond.20:
  %t95 = load i64, ptr %t94
  %t96 = icmp ult i64 %t95, %t93
  br i1 %t96, label %slice.string.body.21, label %slice.string.end.24
slice.string.body.21:
  %t97 = icmp ne i64 %t95, 0
  br i1 %t97, label %slice.string.sep.22, label %slice.string.item.23
slice.string.sep.22:
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  br label %slice.string.item.23
slice.string.item.23:
  %t98 = getelementptr inbounds i32, ptr %t92, i64 %t95
  %t99 = load i32, ptr %t98
  %t100 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t100, i32 %t99)
  call void @string_builder_append_string(ptr %t100)
  %t101 = add i64 %t95, 1
  store i64 %t101, ptr %t94
  br label %slice.string.cond.20
slice.string.end.24:
  call void @string_builder_append_byte(i8 93)
  %t102 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t102, i64 %t89)
  %t103 = load { ptr, i64 }, ptr %t102
  call void @$prn({ ptr, i64 } %t103)
  %t104 = call i64 @string_builder_mark()
  %t105 = alloca { ptr, i64 }
  %t106 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.5, i64 16 }, ptr %t106
  call void @to_string$string(ptr %t105, ptr %t106)
  call void @string_builder_append_string(ptr %t105)
  %t107 = extractvalue { ptr, i64 } %t28, 1
  %t108 = alloca { ptr, i64 }
  call void @to_string$usize(ptr %t108, i64 %t107)
  call void @string_builder_append_string(ptr %t108)
  %t109 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t109, i64 %t104)
  %t110 = load { ptr, i64 }, ptr %t109
  call void @$prn({ ptr, i64 } %t110)
  %t111 = call i64 @string_builder_mark()
  %t112 = alloca { ptr, i64 }
  %t113 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.6, i64 13 }, ptr %t113
  call void @to_string$string(ptr %t112, ptr %t113)
  call void @string_builder_append_string(ptr %t112)
  %t114 = extractvalue { ptr, i64 } %t10, 0
  %t115 = getelementptr inbounds i32, ptr %t114, i32 0
  %t116 = load i32, ptr %t115
  %t117 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t117, i32 %t116)
  call void @string_builder_append_string(ptr %t117)
  %t118 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t118, i64 %t111)
  %t119 = load { ptr, i64 }, ptr %t118
  call void @$prn({ ptr, i64 } %t119)
  %t120 = extractvalue { ptr, i64 } %t10, 0
  %t121 = getelementptr inbounds i32, ptr %t120, i32 1
  %t122 = load i32, ptr %t121
  %t123 = extractvalue { ptr, i64 } %t13, 0
  %t124 = getelementptr inbounds i32, ptr %t123, i32 2
  %t125 = load i32, ptr %t124
  %t126 = add i32 %t122, %t125
  %t127 = extractvalue { ptr, i64 } %t16, 0
  %t128 = getelementptr inbounds i32, ptr %t127, i32 0
  %t129 = load i32, ptr %t128
  %t130 = add i32 %t126, %t129
  %t131 = extractvalue { ptr, i64 } %t23, 0
  %t132 = getelementptr inbounds i32, ptr %t131, i32 2
  %t133 = load i32, ptr %t132
  %t134 = add i32 %t130, %t133
  ret i32 %t134
}

@$main = alias i32 (), ptr @fn.0
