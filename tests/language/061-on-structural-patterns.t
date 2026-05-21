use std.io

Point :: plex {
    x i32
    y i32
    name string
}

score_pair :: fn (pair: (i32, i32)) -> i32 {
    wanted :: 9
    return on pair {
        (0, _) => 100
        (1, y) => y
        (x, for wanted) => x * 10
        else => 0
    }
}

score_point :: fn (point: Point) -> i32 {
    fallback :: "fallback"
    return on point {
        { name: "origin", x: x } => x
        { x: 3, y: y } => y * 10
        { name: for fallback } => 50
        else => 0
    }
}

name_of :: fn (point: Point) -> string {
    return on point {
        { name: name } => name
        else => ""
    }
}

main :: fn () -> i32 {
    prn($"pair zero {score_pair((0, 7))}")
    prn($"pair one {score_pair((1, 8))}")
    prn($"pair nine {score_pair((4, 9))}")
    prn($"pair else {score_pair((2, 3))}")
    prn($"point origin {score_point(Point { x: 2, y: 3, name: "origin" })}")
    prn($"point x {score_point(Point { x: 3, y: 4, name: "other" })}")
    prn($"point fallback {score_point(Point { x: 5, y: 6, name: "fallback" })}")
    prn($"point name {name_of(Point { x: 9, y: 10, name: "named" })}")
    return 0
}
¬
0
¬
pair zero 100
pair one 8
pair nine 40
pair else 0
point origin 2
point x 40
point fallback 50
point name named

¬
hir 0
module module.0(061-on-structural-patterns.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind Point = type.0
bind score_pair = fn.0
bind score_point = fn.1
bind name_of = fn.2
bind main = fn.3
type type.0 = Point
func fn.0(pair: (i32, i32)) -> i32 {
  let wanted: untyped integer = untyped integer 9
  return i32 on (i32, i32) local.0(pair) {
    tuple(value(i32 0), _) => {
      expr i32 100
    }
    tuple(value(i32 1), as y) => {
      expr i32 local.2(y)
    }
    tuple(as x, value(i32 local.1(wanted))) => {
      expr i32 multiply(i32 local.3(x), i32 10)
    }
    else => {
      expr i32 0
    }
  }
}
func fn.1(point: Point) -> i32 {
  let fallback: string = string "fallback"
  return i32 on Point local.4(point) {
    plex(name: value(string "origin"), x: as x) => {
      expr i32 local.6(x)
    }
    plex(x: value(i32 3), y: as y) => {
      expr i32 multiply(i32 local.7(y), i32 10)
    }
    plex(name: value(string local.5(fallback))) => {
      expr i32 50
    }
    else => {
      expr i32 0
    }
  }
}
func fn.2(point: Point) -> string {
  return string on Point local.8(point) {
    plex(name: as name) => {
      expr string local.9(name)
    }
    else => {
      expr string ""
    }
  }
}
func fn.3() -> i32 {
  expr void call bind.0(prn)(string interpolate(<unknown> "pair zero ", i32 call bind.3(score_pair)((i32, i32) tuple(i32 0, i32 7))))
  expr void call bind.0(prn)(string interpolate(<unknown> "pair one ", i32 call bind.3(score_pair)((i32, i32) tuple(i32 1, i32 8))))
  expr void call bind.0(prn)(string interpolate(<unknown> "pair nine ", i32 call bind.3(score_pair)((i32, i32) tuple(i32 4, i32 9))))
  expr void call bind.0(prn)(string interpolate(<unknown> "pair else ", i32 call bind.3(score_pair)((i32, i32) tuple(i32 2, i32 3))))
  expr void call bind.0(prn)(string interpolate(<unknown> "point origin ", i32 call bind.4(score_point)(Point plex(x: i32 2, y: i32 3, name: string "origin"))))
  expr void call bind.0(prn)(string interpolate(<unknown> "point x ", i32 call bind.4(score_point)(Point plex(x: i32 3, y: i32 4, name: string "other"))))
  expr void call bind.0(prn)(string interpolate(<unknown> "point fallback ", i32 call bind.4(score_point)(Point plex(x: i32 5, y: i32 6, name: string "fallback"))))
  expr void call bind.0(prn)(string interpolate(<unknown> "point name ", string call bind.5(name_of)(Point plex(x: i32 9, y: i32 10, name: string "named"))))
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [66 x i8] c"tests/language/061-on-structural-patterns.t\00"
@.str.m0.0 = private unnamed_addr constant [9 x i8] c"fallback\00"
@.str.m0.1 = private unnamed_addr constant [7 x i8] c"origin\00"
@.str.m0.2 = private unnamed_addr constant [1 x i8] c"\00"
@.str.m0.3 = private unnamed_addr constant [11 x i8] c"pair zero \00"
@.str.m0.4 = private unnamed_addr constant [10 x i8] c"pair one \00"
@.str.m0.5 = private unnamed_addr constant [11 x i8] c"pair nine \00"
@.str.m0.6 = private unnamed_addr constant [11 x i8] c"pair else \00"
@.str.m0.7 = private unnamed_addr constant [14 x i8] c"point origin \00"
@.str.m0.8 = private unnamed_addr constant [7 x i8] c"origin\00"
@.str.m0.9 = private unnamed_addr constant [9 x i8] c"point x \00"
@.str.m0.10 = private unnamed_addr constant [6 x i8] c"other\00"
@.str.m0.11 = private unnamed_addr constant [16 x i8] c"point fallback \00"
@.str.m0.12 = private unnamed_addr constant [9 x i8] c"fallback\00"
@.str.m0.13 = private unnamed_addr constant [12 x i8] c"point name \00"
@.str.m0.14 = private unnamed_addr constant [6 x i8] c"named\00"

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

define internal i32 @fn.0({ i32, i32 } %pair) {
  %t0 = extractvalue { i32, i32 } %pair, 0
  %t1 = icmp eq i32 %t0, 0
  %t2 = extractvalue { i32, i32 } %pair, 1
  %t3 = and i1 %t1, 1
  br i1 %t3, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t4 = extractvalue { i32, i32 } %pair, 0
  %t5 = icmp eq i32 %t4, 1
  %t6 = extractvalue { i32, i32 } %pair, 1
  %t7 = and i1 %t5, 1
  br i1 %t7, label %on.body.4, label %on.next.5
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  %t8 = extractvalue { i32, i32 } %pair, 0
  %t9 = extractvalue { i32, i32 } %pair, 1
  %t10 = icmp eq i32 %t9, 9
  %t11 = and i1 1, %t10
  br i1 %t11, label %on.body.7, label %on.next.8
on.body.7:
  %t12 = mul i32 %t8, 10
  br label %on.value.9
on.value.9:
  br label %on.end.0
on.next.8:
  br label %on.body.10
on.body.10:
  br label %on.value.12
on.value.12:
  br label %on.end.0
on.end.0:
  %t13 = phi i32 [100, %on.value.3], [%t6, %on.value.6], [%t12, %on.value.9], [0, %on.value.12]
  ret i32 %t13
}

define internal i32 @fn.1({ i32, i32, { ptr, i64 } } %point) {
  %t0 = extractvalue { i32, i32, { ptr, i64 } } %point, 2
  %t1 = alloca { ptr, i64 }
  store { ptr, i64 } %t0, ptr %t1
  %t2 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 6 }, ptr %t2
  %t3 = call i1 @string_eq(ptr %t1, ptr %t2)
  %t4 = extractvalue { i32, i32, { ptr, i64 } } %point, 0
  %t5 = and i1 %t3, 1
  br i1 %t5, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t6 = extractvalue { i32, i32, { ptr, i64 } } %point, 0
  %t7 = icmp eq i32 %t6, 3
  %t8 = extractvalue { i32, i32, { ptr, i64 } } %point, 1
  %t9 = and i1 %t7, 1
  br i1 %t9, label %on.body.4, label %on.next.5
on.body.4:
  %t10 = mul i32 %t8, 10
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  %t11 = extractvalue { i32, i32, { ptr, i64 } } %point, 2
  %t12 = alloca { ptr, i64 }
  store { ptr, i64 } %t11, ptr %t12
  %t13 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 8 }, ptr %t13
  %t14 = call i1 @string_eq(ptr %t12, ptr %t13)
  br i1 %t14, label %on.body.7, label %on.next.8
on.body.7:
  br label %on.value.9
on.value.9:
  br label %on.end.0
on.next.8:
  br label %on.body.10
on.body.10:
  br label %on.value.12
on.value.12:
  br label %on.end.0
on.end.0:
  %t15 = phi i32 [%t4, %on.value.3], [%t10, %on.value.6], [50, %on.value.9], [0, %on.value.12]
  ret i32 %t15
}

define internal { ptr, i64 } @fn.2({ i32, i32, { ptr, i64 } } %point) {
  %t0 = extractvalue { i32, i32, { ptr, i64 } } %point, 2
  br i1 1, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  br label %on.body.4
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.end.0:
  %t1 = phi { ptr, i64 } [%t0, %on.value.3], [{ ptr @.str.m0.2, i64 0 }, %on.value.6]
  ret { ptr, i64 } %t1
}

define internal i32 @fn.3() {
  %t0 = call i64 @string_builder_mark()
  %t1 = alloca { ptr, i64 }
  %t2 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 10 }, ptr %t2
  call void @to_string$string(ptr %t1, ptr %t2)
  call void @string_builder_append_string(ptr %t1)
  %t3 = insertvalue { i32, i32 } poison, i32 0, 0
  %t4 = insertvalue { i32, i32 } %t3, i32 7, 1
  %t5 = call i32 @fn.0({ i32, i32 } %t4)
  %t6 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t6, i32 %t5)
  call void @string_builder_append_string(ptr %t6)
  %t7 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t7, i64 %t0)
  %t8 = load { ptr, i64 }, ptr %t7
  call void @$prn({ ptr, i64 } %t8)
  %t9 = call i64 @string_builder_mark()
  %t10 = alloca { ptr, i64 }
  %t11 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 9 }, ptr %t11
  call void @to_string$string(ptr %t10, ptr %t11)
  call void @string_builder_append_string(ptr %t10)
  %t12 = insertvalue { i32, i32 } poison, i32 1, 0
  %t13 = insertvalue { i32, i32 } %t12, i32 8, 1
  %t14 = call i32 @fn.0({ i32, i32 } %t13)
  %t15 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t15, i32 %t14)
  call void @string_builder_append_string(ptr %t15)
  %t16 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t16, i64 %t9)
  %t17 = load { ptr, i64 }, ptr %t16
  call void @$prn({ ptr, i64 } %t17)
  %t18 = call i64 @string_builder_mark()
  %t19 = alloca { ptr, i64 }
  %t20 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.5, i64 10 }, ptr %t20
  call void @to_string$string(ptr %t19, ptr %t20)
  call void @string_builder_append_string(ptr %t19)
  %t21 = insertvalue { i32, i32 } poison, i32 4, 0
  %t22 = insertvalue { i32, i32 } %t21, i32 9, 1
  %t23 = call i32 @fn.0({ i32, i32 } %t22)
  %t24 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t24, i32 %t23)
  call void @string_builder_append_string(ptr %t24)
  %t25 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t25, i64 %t18)
  %t26 = load { ptr, i64 }, ptr %t25
  call void @$prn({ ptr, i64 } %t26)
  %t27 = call i64 @string_builder_mark()
  %t28 = alloca { ptr, i64 }
  %t29 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.6, i64 10 }, ptr %t29
  call void @to_string$string(ptr %t28, ptr %t29)
  call void @string_builder_append_string(ptr %t28)
  %t30 = insertvalue { i32, i32 } poison, i32 2, 0
  %t31 = insertvalue { i32, i32 } %t30, i32 3, 1
  %t32 = call i32 @fn.0({ i32, i32 } %t31)
  %t33 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t33, i32 %t32)
  call void @string_builder_append_string(ptr %t33)
  %t34 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t34, i64 %t27)
  %t35 = load { ptr, i64 }, ptr %t34
  call void @$prn({ ptr, i64 } %t35)
  %t36 = call i64 @string_builder_mark()
  %t37 = alloca { ptr, i64 }
  %t38 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.7, i64 13 }, ptr %t38
  call void @to_string$string(ptr %t37, ptr %t38)
  call void @string_builder_append_string(ptr %t37)
  %t39 = insertvalue { i32, i32, { ptr, i64 } } poison, i32 2, 0
  %t40 = insertvalue { i32, i32, { ptr, i64 } } %t39, i32 3, 1
  %t41 = insertvalue { i32, i32, { ptr, i64 } } %t40, { ptr, i64 } { ptr @.str.m0.8, i64 6 }, 2
  %t42 = call i32 @fn.1({ i32, i32, { ptr, i64 } } %t41)
  %t43 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t43, i32 %t42)
  call void @string_builder_append_string(ptr %t43)
  %t44 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t44, i64 %t36)
  %t45 = load { ptr, i64 }, ptr %t44
  call void @$prn({ ptr, i64 } %t45)
  %t46 = call i64 @string_builder_mark()
  %t47 = alloca { ptr, i64 }
  %t48 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.9, i64 8 }, ptr %t48
  call void @to_string$string(ptr %t47, ptr %t48)
  call void @string_builder_append_string(ptr %t47)
  %t49 = insertvalue { i32, i32, { ptr, i64 } } poison, i32 3, 0
  %t50 = insertvalue { i32, i32, { ptr, i64 } } %t49, i32 4, 1
  %t51 = insertvalue { i32, i32, { ptr, i64 } } %t50, { ptr, i64 } { ptr @.str.m0.10, i64 5 }, 2
  %t52 = call i32 @fn.1({ i32, i32, { ptr, i64 } } %t51)
  %t53 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t53, i32 %t52)
  call void @string_builder_append_string(ptr %t53)
  %t54 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t54, i64 %t46)
  %t55 = load { ptr, i64 }, ptr %t54
  call void @$prn({ ptr, i64 } %t55)
  %t56 = call i64 @string_builder_mark()
  %t57 = alloca { ptr, i64 }
  %t58 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.11, i64 15 }, ptr %t58
  call void @to_string$string(ptr %t57, ptr %t58)
  call void @string_builder_append_string(ptr %t57)
  %t59 = insertvalue { i32, i32, { ptr, i64 } } poison, i32 5, 0
  %t60 = insertvalue { i32, i32, { ptr, i64 } } %t59, i32 6, 1
  %t61 = insertvalue { i32, i32, { ptr, i64 } } %t60, { ptr, i64 } { ptr @.str.m0.12, i64 8 }, 2
  %t62 = call i32 @fn.1({ i32, i32, { ptr, i64 } } %t61)
  %t63 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t63, i32 %t62)
  call void @string_builder_append_string(ptr %t63)
  %t64 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t64, i64 %t56)
  %t65 = load { ptr, i64 }, ptr %t64
  call void @$prn({ ptr, i64 } %t65)
  %t66 = call i64 @string_builder_mark()
  %t67 = alloca { ptr, i64 }
  %t68 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.13, i64 11 }, ptr %t68
  call void @to_string$string(ptr %t67, ptr %t68)
  call void @string_builder_append_string(ptr %t67)
  %t69 = insertvalue { i32, i32, { ptr, i64 } } poison, i32 9, 0
  %t70 = insertvalue { i32, i32, { ptr, i64 } } %t69, i32 10, 1
  %t71 = insertvalue { i32, i32, { ptr, i64 } } %t70, { ptr, i64 } { ptr @.str.m0.14, i64 5 }, 2
  %t72 = call { ptr, i64 } @fn.2({ i32, i32, { ptr, i64 } } %t71)
  %t73 = alloca { ptr, i64 }
  %t74 = alloca { ptr, i64 }
  store { ptr, i64 } %t72, ptr %t74
  call void @to_string$string(ptr %t73, ptr %t74)
  call void @string_builder_append_string(ptr %t73)
  %t75 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t75, i64 %t66)
  %t76 = load { ptr, i64 }, ptr %t75
  call void @$prn({ ptr, i64 } %t76)
  ret i32 0
}

@$score_pair = internal alias i32 ({ i32, i32 }), ptr @fn.0
@$score_point = internal alias i32 ({ i32, i32, { ptr, i64 } }), ptr @fn.1
@$name_of = internal alias { ptr, i64 } ({ i32, i32, { ptr, i64 } }), ptr @fn.2
@$main = alias i32 (), ptr @fn.3
