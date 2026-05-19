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
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind Point = type.0
bind main = fn.0
type type.0 = Point
func fn.0() -> i32 {
  let p: Point = Point plex(x: i32 3, y: i32 4, name: string "origin")
  let q: Point = Point plex(name: string "other", x: i32 5, y: i32 6)
  expr void call bind.0(prn)(string interpolate(<unknown> "p ", string field(Point local.0(p), name), <unknown> ": ", i32 field(Point local.0(p), x), <unknown> ", ", i32 field(Point local.0(p), y)))
  expr void call bind.0(prn)(string interpolate(<unknown> "q ", string field(Point local.1(q), name), <unknown> ": ", i32 field(Point local.1(q), x), <unknown> ", ", i32 field(Point local.1(q), y)))
  return i32 add(i32 add(i32 add(i32 field(Point local.0(p), x), i32 field(Point local.0(p), y)), i32 field(Point local.1(q), x)), i32 field(Point local.1(q), y))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [50 x i8] c"tests/language/056-plexes.t\00"
@.str.m0.0 = private unnamed_addr constant [7 x i8] c"origin\00"
@.str.m0.1 = private unnamed_addr constant [6 x i8] c"other\00"
@.str.m0.2 = private unnamed_addr constant [3 x i8] c"p \00"
@.str.m0.3 = private unnamed_addr constant [3 x i8] c": \00"
@.str.m0.4 = private unnamed_addr constant [3 x i8] c", \00"
@.str.m0.5 = private unnamed_addr constant [3 x i8] c"q \00"
@.str.m0.6 = private unnamed_addr constant [3 x i8] c": \00"
@.str.m0.7 = private unnamed_addr constant [3 x i8] c", \00"

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
  %t0 = insertvalue { i32, i32, { ptr, i64 } } poison, i32 3, 0
  %t1 = insertvalue { i32, i32, { ptr, i64 } } %t0, i32 4, 1
  %t2 = insertvalue { i32, i32, { ptr, i64 } } %t1, { ptr, i64 } { ptr @.str.m0.0, i64 6 }, 2
  %t3 = insertvalue { i32, i32, { ptr, i64 } } poison, i32 5, 0
  %t4 = insertvalue { i32, i32, { ptr, i64 } } %t3, i32 6, 1
  %t5 = insertvalue { i32, i32, { ptr, i64 } } %t4, { ptr, i64 } { ptr @.str.m0.1, i64 5 }, 2
  %t6 = call i64 @string_builder_mark()
  %t7 = alloca { ptr, i64 }
  %t8 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 2 }, ptr %t8
  call void @to_string$string(ptr %t7, ptr %t8)
  call void @string_builder_append_string(ptr %t7)
  %t9 = extractvalue { i32, i32, { ptr, i64 } } %t2, 2
  %t10 = alloca { ptr, i64 }
  %t11 = alloca { ptr, i64 }
  store { ptr, i64 } %t9, ptr %t11
  call void @to_string$string(ptr %t10, ptr %t11)
  call void @string_builder_append_string(ptr %t10)
  %t12 = alloca { ptr, i64 }
  %t13 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 2 }, ptr %t13
  call void @to_string$string(ptr %t12, ptr %t13)
  call void @string_builder_append_string(ptr %t12)
  %t14 = extractvalue { i32, i32, { ptr, i64 } } %t2, 0
  %t15 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t15, i32 %t14)
  call void @string_builder_append_string(ptr %t15)
  %t16 = alloca { ptr, i64 }
  %t17 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 2 }, ptr %t17
  call void @to_string$string(ptr %t16, ptr %t17)
  call void @string_builder_append_string(ptr %t16)
  %t18 = extractvalue { i32, i32, { ptr, i64 } } %t2, 1
  %t19 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t19, i32 %t18)
  call void @string_builder_append_string(ptr %t19)
  %t20 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t20, i64 %t6)
  %t21 = load { ptr, i64 }, ptr %t20
  call void @$prn({ ptr, i64 } %t21)
  %t22 = call i64 @string_builder_mark()
  %t23 = alloca { ptr, i64 }
  %t24 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.5, i64 2 }, ptr %t24
  call void @to_string$string(ptr %t23, ptr %t24)
  call void @string_builder_append_string(ptr %t23)
  %t25 = extractvalue { i32, i32, { ptr, i64 } } %t5, 2
  %t26 = alloca { ptr, i64 }
  %t27 = alloca { ptr, i64 }
  store { ptr, i64 } %t25, ptr %t27
  call void @to_string$string(ptr %t26, ptr %t27)
  call void @string_builder_append_string(ptr %t26)
  %t28 = alloca { ptr, i64 }
  %t29 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.6, i64 2 }, ptr %t29
  call void @to_string$string(ptr %t28, ptr %t29)
  call void @string_builder_append_string(ptr %t28)
  %t30 = extractvalue { i32, i32, { ptr, i64 } } %t5, 0
  %t31 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t31, i32 %t30)
  call void @string_builder_append_string(ptr %t31)
  %t32 = alloca { ptr, i64 }
  %t33 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.7, i64 2 }, ptr %t33
  call void @to_string$string(ptr %t32, ptr %t33)
  call void @string_builder_append_string(ptr %t32)
  %t34 = extractvalue { i32, i32, { ptr, i64 } } %t5, 1
  %t35 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t35, i32 %t34)
  call void @string_builder_append_string(ptr %t35)
  %t36 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t36, i64 %t22)
  %t37 = load { ptr, i64 }, ptr %t36
  call void @$prn({ ptr, i64 } %t37)
  %t38 = extractvalue { i32, i32, { ptr, i64 } } %t2, 0
  %t39 = extractvalue { i32, i32, { ptr, i64 } } %t2, 1
  %t40 = add i32 %t38, %t39
  %t41 = extractvalue { i32, i32, { ptr, i64 } } %t5, 0
  %t42 = add i32 %t40, %t41
  %t43 = extractvalue { i32, i32, { ptr, i64 } } %t5, 1
  %t44 = add i32 %t42, %t43
  ret i32 %t44
}

@$main = alias i32 (), ptr @fn.0
