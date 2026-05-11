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
        (1, as y) => y
        (as x, wanted) => x * 10
        else => 0
    }
}

score_point :: fn (point: Point) -> i32 {
    fallback :: "fallback"
    return on point {
        { name: "origin", x: as x } => x
        { x: 3, y: as y } => y * 10
        { name: fallback } => 50
        else => 0
    }
}

name_of :: fn (point: Point) -> string {
    return on point {
        { name: as name } => name
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
  expr void call bind.2(prn)(string interpolate(<unknown> "pair zero ", i32 call bind.6(score_pair)((i32, i32) tuple(i32 0, i32 7))))
  expr void call bind.2(prn)(string interpolate(<unknown> "pair one ", i32 call bind.6(score_pair)((i32, i32) tuple(i32 1, i32 8))))
  expr void call bind.2(prn)(string interpolate(<unknown> "pair nine ", i32 call bind.6(score_pair)((i32, i32) tuple(i32 4, i32 9))))
  expr void call bind.2(prn)(string interpolate(<unknown> "pair else ", i32 call bind.6(score_pair)((i32, i32) tuple(i32 2, i32 3))))
  expr void call bind.2(prn)(string interpolate(<unknown> "point origin ", i32 call bind.7(score_point)(Point plex(x: i32 2, y: i32 3, name: string "origin"))))
  expr void call bind.2(prn)(string interpolate(<unknown> "point x ", i32 call bind.7(score_point)(Point plex(x: i32 3, y: i32 4, name: string "other"))))
  expr void call bind.2(prn)(string interpolate(<unknown> "point fallback ", i32 call bind.7(score_point)(Point plex(x: i32 5, y: i32 6, name: string "fallback"))))
  expr void call bind.2(prn)(string interpolate(<unknown> "point name ", string call bind.8(name_of)(Point plex(x: i32 9, y: i32 10, name: string "named"))))
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

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
  %t1 = call i1 @string_eq({ ptr, i64 } %t0, { ptr, i64 } { ptr @.str.m0.1, i64 6 })
  %t2 = extractvalue { i32, i32, { ptr, i64 } } %point, 0
  %t3 = and i1 %t1, 1
  br i1 %t3, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t4 = extractvalue { i32, i32, { ptr, i64 } } %point, 0
  %t5 = icmp eq i32 %t4, 3
  %t6 = extractvalue { i32, i32, { ptr, i64 } } %point, 1
  %t7 = and i1 %t5, 1
  br i1 %t7, label %on.body.4, label %on.next.5
on.body.4:
  %t8 = mul i32 %t6, 10
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  %t9 = extractvalue { i32, i32, { ptr, i64 } } %point, 2
  %t10 = call i1 @string_eq({ ptr, i64 } %t9, { ptr, i64 } { ptr @.str.m0.0, i64 8 })
  br i1 %t10, label %on.body.7, label %on.next.8
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
  %t11 = phi i32 [%t2, %on.value.3], [%t8, %on.value.6], [50, %on.value.9], [0, %on.value.12]
  ret i32 %t11
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
  %t1 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.3, i64 10 })
  call void @string_builder_append_string({ ptr, i64 } %t1)
  %t2 = insertvalue { i32, i32 } poison, i32 0, 0
  %t3 = insertvalue { i32, i32 } %t2, i32 7, 1
  %t4 = call i32 @fn.0({ i32, i32 } %t3)
  %t5 = call { ptr, i64 } @to_string$i32(i32 %t4)
  call void @string_builder_append_string({ ptr, i64 } %t5)
  %t6 = call { ptr, i64 } @string_builder_finish(i64 %t0)
  call void @$prn({ ptr, i64 } %t6)
  %t7 = call i64 @string_builder_mark()
  %t8 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.4, i64 9 })
  call void @string_builder_append_string({ ptr, i64 } %t8)
  %t9 = insertvalue { i32, i32 } poison, i32 1, 0
  %t10 = insertvalue { i32, i32 } %t9, i32 8, 1
  %t11 = call i32 @fn.0({ i32, i32 } %t10)
  %t12 = call { ptr, i64 } @to_string$i32(i32 %t11)
  call void @string_builder_append_string({ ptr, i64 } %t12)
  %t13 = call { ptr, i64 } @string_builder_finish(i64 %t7)
  call void @$prn({ ptr, i64 } %t13)
  %t14 = call i64 @string_builder_mark()
  %t15 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.5, i64 10 })
  call void @string_builder_append_string({ ptr, i64 } %t15)
  %t16 = insertvalue { i32, i32 } poison, i32 4, 0
  %t17 = insertvalue { i32, i32 } %t16, i32 9, 1
  %t18 = call i32 @fn.0({ i32, i32 } %t17)
  %t19 = call { ptr, i64 } @to_string$i32(i32 %t18)
  call void @string_builder_append_string({ ptr, i64 } %t19)
  %t20 = call { ptr, i64 } @string_builder_finish(i64 %t14)
  call void @$prn({ ptr, i64 } %t20)
  %t21 = call i64 @string_builder_mark()
  %t22 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.6, i64 10 })
  call void @string_builder_append_string({ ptr, i64 } %t22)
  %t23 = insertvalue { i32, i32 } poison, i32 2, 0
  %t24 = insertvalue { i32, i32 } %t23, i32 3, 1
  %t25 = call i32 @fn.0({ i32, i32 } %t24)
  %t26 = call { ptr, i64 } @to_string$i32(i32 %t25)
  call void @string_builder_append_string({ ptr, i64 } %t26)
  %t27 = call { ptr, i64 } @string_builder_finish(i64 %t21)
  call void @$prn({ ptr, i64 } %t27)
  %t28 = call i64 @string_builder_mark()
  %t29 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.7, i64 13 })
  call void @string_builder_append_string({ ptr, i64 } %t29)
  %t30 = insertvalue { i32, i32, { ptr, i64 } } poison, i32 2, 0
  %t31 = insertvalue { i32, i32, { ptr, i64 } } %t30, i32 3, 1
  %t32 = insertvalue { i32, i32, { ptr, i64 } } %t31, { ptr, i64 } { ptr @.str.m0.8, i64 6 }, 2
  %t33 = call i32 @fn.1({ i32, i32, { ptr, i64 } } %t32)
  %t34 = call { ptr, i64 } @to_string$i32(i32 %t33)
  call void @string_builder_append_string({ ptr, i64 } %t34)
  %t35 = call { ptr, i64 } @string_builder_finish(i64 %t28)
  call void @$prn({ ptr, i64 } %t35)
  %t36 = call i64 @string_builder_mark()
  %t37 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.9, i64 8 })
  call void @string_builder_append_string({ ptr, i64 } %t37)
  %t38 = insertvalue { i32, i32, { ptr, i64 } } poison, i32 3, 0
  %t39 = insertvalue { i32, i32, { ptr, i64 } } %t38, i32 4, 1
  %t40 = insertvalue { i32, i32, { ptr, i64 } } %t39, { ptr, i64 } { ptr @.str.m0.10, i64 5 }, 2
  %t41 = call i32 @fn.1({ i32, i32, { ptr, i64 } } %t40)
  %t42 = call { ptr, i64 } @to_string$i32(i32 %t41)
  call void @string_builder_append_string({ ptr, i64 } %t42)
  %t43 = call { ptr, i64 } @string_builder_finish(i64 %t36)
  call void @$prn({ ptr, i64 } %t43)
  %t44 = call i64 @string_builder_mark()
  %t45 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.11, i64 15 })
  call void @string_builder_append_string({ ptr, i64 } %t45)
  %t46 = insertvalue { i32, i32, { ptr, i64 } } poison, i32 5, 0
  %t47 = insertvalue { i32, i32, { ptr, i64 } } %t46, i32 6, 1
  %t48 = insertvalue { i32, i32, { ptr, i64 } } %t47, { ptr, i64 } { ptr @.str.m0.12, i64 8 }, 2
  %t49 = call i32 @fn.1({ i32, i32, { ptr, i64 } } %t48)
  %t50 = call { ptr, i64 } @to_string$i32(i32 %t49)
  call void @string_builder_append_string({ ptr, i64 } %t50)
  %t51 = call { ptr, i64 } @string_builder_finish(i64 %t44)
  call void @$prn({ ptr, i64 } %t51)
  %t52 = call i64 @string_builder_mark()
  %t53 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.13, i64 11 })
  call void @string_builder_append_string({ ptr, i64 } %t53)
  %t54 = insertvalue { i32, i32, { ptr, i64 } } poison, i32 9, 0
  %t55 = insertvalue { i32, i32, { ptr, i64 } } %t54, i32 10, 1
  %t56 = insertvalue { i32, i32, { ptr, i64 } } %t55, { ptr, i64 } { ptr @.str.m0.14, i64 5 }, 2
  %t57 = call { ptr, i64 } @fn.2({ i32, i32, { ptr, i64 } } %t56)
  %t58 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t57)
  call void @string_builder_append_string({ ptr, i64 } %t58)
  %t59 = call { ptr, i64 } @string_builder_finish(i64 %t52)
  call void @$prn({ ptr, i64 } %t59)
  ret i32 0
}

@$score_pair = internal alias i32 ({ i32, i32 }), ptr @fn.0
@$score_point = internal alias i32 ({ i32, i32, { ptr, i64 } }), ptr @fn.1
@$name_of = internal alias { ptr, i64 } ({ i32, i32, { ptr, i64 } }), ptr @fn.2
@$main = alias i32 (), ptr @fn.3
