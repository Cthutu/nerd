use std.io

Point :: plex {
    x i32
    y i32
    name string
}

main :: fn () -> i32 {
    p: Point = Point { x: 3, y: 4, name: "origin" }
    q := Point { name: "other", x: 5, y: 6 }
    prn($"p {p.name}: {p.x}, {p.y}")
    prn($"q {q.name}: {q.x}, {q.y}")
    return p.x + p.y + q.x + q.y
}
¬
18
¬
p origin: 3, 4
q other: 5, 6

¬
hir 0
module module.0(056-plexes.input)
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
bind main = fn.0
type type.0 = Point
func fn.0() -> i32 {
  let p: Point = Point plex(x: i32 3, y: i32 4, name: string "origin")
  let q: Point = Point plex(name: string "other", x: i32 5, y: i32 6)
  expr void call bind.2(prn)(string interpolate(<unknown> "p ", string field(Point local.0(p), name), <unknown> ": ", i32 field(Point local.0(p), x), <unknown> ", ", i32 field(Point local.0(p), y)))
  expr void call bind.2(prn)(string interpolate(<unknown> "q ", string field(Point local.1(q), name), <unknown> ": ", i32 field(Point local.1(q), x), <unknown> ", ", i32 field(Point local.1(q), y)))
  return i32 add(i32 add(i32 add(i32 field(Point local.0(p), x), i32 field(Point local.0(p), y)), i32 field(Point local.1(q), x)), i32 field(Point local.1(q), y))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [7 x i8] c"origin\00"
@.str.m0.1 = private unnamed_addr constant [6 x i8] c"other\00"
@.str.m0.2 = private unnamed_addr constant [3 x i8] c"p \00"
@.str.m0.3 = private unnamed_addr constant [3 x i8] c": \00"
@.str.m0.4 = private unnamed_addr constant [3 x i8] c", \00"
@.str.m0.5 = private unnamed_addr constant [3 x i8] c"q \00"
@.str.m0.6 = private unnamed_addr constant [3 x i8] c": \00"
@.str.m0.7 = private unnamed_addr constant [3 x i8] c", \00"

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
  %t0 = insertvalue { i32, i32, { ptr, i64 } } poison, i32 3, 0
  %t1 = insertvalue { i32, i32, { ptr, i64 } } %t0, i32 4, 1
  %t2 = insertvalue { i32, i32, { ptr, i64 } } %t1, { ptr, i64 } { ptr @.str.m0.0, i64 6 }, 2
  %t3 = insertvalue { i32, i32, { ptr, i64 } } poison, i32 5, 0
  %t4 = insertvalue { i32, i32, { ptr, i64 } } %t3, i32 6, 1
  %t5 = insertvalue { i32, i32, { ptr, i64 } } %t4, { ptr, i64 } { ptr @.str.m0.1, i64 5 }, 2
  %t6 = call i64 @string_builder_mark()
  %t7 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 2 })
  call void @string_builder_append_string({ ptr, i64 } %t7)
  %t8 = extractvalue { i32, i32, { ptr, i64 } } %t2, 2
  %t9 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t8)
  call void @string_builder_append_string({ ptr, i64 } %t9)
  %t10 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.3, i64 2 })
  call void @string_builder_append_string({ ptr, i64 } %t10)
  %t11 = extractvalue { i32, i32, { ptr, i64 } } %t2, 0
  %t12 = call { ptr, i64 } @to_string$i32(i32 %t11)
  call void @string_builder_append_string({ ptr, i64 } %t12)
  %t13 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.4, i64 2 })
  call void @string_builder_append_string({ ptr, i64 } %t13)
  %t14 = extractvalue { i32, i32, { ptr, i64 } } %t2, 1
  %t15 = call { ptr, i64 } @to_string$i32(i32 %t14)
  call void @string_builder_append_string({ ptr, i64 } %t15)
  %t16 = call { ptr, i64 } @string_builder_finish(i64 %t6)
  call void @$prn({ ptr, i64 } %t16)
  %t17 = call i64 @string_builder_mark()
  %t18 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.5, i64 2 })
  call void @string_builder_append_string({ ptr, i64 } %t18)
  %t19 = extractvalue { i32, i32, { ptr, i64 } } %t5, 2
  %t20 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t19)
  call void @string_builder_append_string({ ptr, i64 } %t20)
  %t21 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.6, i64 2 })
  call void @string_builder_append_string({ ptr, i64 } %t21)
  %t22 = extractvalue { i32, i32, { ptr, i64 } } %t5, 0
  %t23 = call { ptr, i64 } @to_string$i32(i32 %t22)
  call void @string_builder_append_string({ ptr, i64 } %t23)
  %t24 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.7, i64 2 })
  call void @string_builder_append_string({ ptr, i64 } %t24)
  %t25 = extractvalue { i32, i32, { ptr, i64 } } %t5, 1
  %t26 = call { ptr, i64 } @to_string$i32(i32 %t25)
  call void @string_builder_append_string({ ptr, i64 } %t26)
  %t27 = call { ptr, i64 } @string_builder_finish(i64 %t17)
  call void @$prn({ ptr, i64 } %t27)
  %t28 = extractvalue { i32, i32, { ptr, i64 } } %t2, 0
  %t29 = extractvalue { i32, i32, { ptr, i64 } } %t2, 1
  %t30 = add i32 %t28, %t29
  %t31 = extractvalue { i32, i32, { ptr, i64 } } %t5, 0
  %t32 = add i32 %t30, %t31
  %t33 = extractvalue { i32, i32, { ptr, i64 } } %t5, 1
  %t34 = add i32 %t32, %t33
  ret i32 %t34
}

@$main = alias i32 (), ptr @fn.0
