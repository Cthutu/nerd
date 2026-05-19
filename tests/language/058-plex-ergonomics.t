use std.io

Point :: plex #c {
    x    i32
    y    i32
    name string
}

Packed :: plex #packed {
    a u8
    b i32
}

main :: fn () -> i32 {
    p := Point { x: 1, y: 2, name: "first" }
    q := p with { y: 7, name: "second" }
    pp := ^q
    prn($"{p.name} {p.x} {p.y}")
    prn($"{pp.name} {pp.x} {pp.y}")
    packed := Packed { a: 1, b: 2 }
    return q.x + q.y + packed.b
}
¬
10
¬
first 1 2
second 1 7

¬
hir 0
module module.0(058-plex-ergonomics.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind Point = type.0
bind Packed = type.1
bind main = fn.0
type type.0 = Point
type type.1 = Packed
func fn.0() -> i32 {
  let p: Point = Point plex(x: i32 1, y: i32 2, name: string "first")
  let q: Point = Point plex_update(Point local.0(p), y: i32 7, name: string "second")
  let pp: ^Point = ^Point address_of(Point local.1(q))
  expr void call bind.0(prn)(string interpolate(string field(Point local.0(p), name), <unknown> " ", i32 field(Point local.0(p), x), <unknown> " ", i32 field(Point local.0(p), y)))
  expr void call bind.0(prn)(string interpolate(string field(^Point local.2(pp), name), <unknown> " ", i32 field(^Point local.2(pp), x), <unknown> " ", i32 field(^Point local.2(pp), y)))
  let packed: Packed = Packed plex(a: u8 1, b: i32 2)
  return i32 add(i32 add(i32 field(Point local.1(q), x), i32 field(Point local.1(q), y)), i32 field(Packed local.3(packed), b))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [59 x i8] c"tests/language/058-plex-ergonomics.t\00"
@.str.m0.0 = private unnamed_addr constant [6 x i8] c"first\00"
@.str.m0.1 = private unnamed_addr constant [7 x i8] c"second\00"
@.str.m0.2 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.3 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.4 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.5 = private unnamed_addr constant [2 x i8] c" \00"

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
  %t0 = insertvalue { i32, i32, { ptr, i64 } } poison, i32 1, 0
  %t1 = insertvalue { i32, i32, { ptr, i64 } } %t0, i32 2, 1
  %t2 = insertvalue { i32, i32, { ptr, i64 } } %t1, { ptr, i64 } { ptr @.str.m0.0, i64 5 }, 2
  %t3 = insertvalue { i32, i32, { ptr, i64 } } %t2, i32 7, 1
  %t4 = insertvalue { i32, i32, { ptr, i64 } } %t3, { ptr, i64 } { ptr @.str.m0.1, i64 6 }, 2
  %local.1 = alloca { i32, i32, { ptr, i64 } }
  store { i32, i32, { ptr, i64 } } %t4, ptr %local.1
  %t5 = call i64 @string_builder_mark()
  %t6 = extractvalue { i32, i32, { ptr, i64 } } %t2, 2
  %t7 = alloca { ptr, i64 }
  %t8 = alloca { ptr, i64 }
  store { ptr, i64 } %t6, ptr %t8
  call void @to_string$string(ptr %t7, ptr %t8)
  call void @string_builder_append_string(ptr %t7)
  %t9 = alloca { ptr, i64 }
  %t10 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 1 }, ptr %t10
  call void @to_string$string(ptr %t9, ptr %t10)
  call void @string_builder_append_string(ptr %t9)
  %t11 = extractvalue { i32, i32, { ptr, i64 } } %t2, 0
  %t12 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t12, i32 %t11)
  call void @string_builder_append_string(ptr %t12)
  %t13 = alloca { ptr, i64 }
  %t14 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 1 }, ptr %t14
  call void @to_string$string(ptr %t13, ptr %t14)
  call void @string_builder_append_string(ptr %t13)
  %t15 = extractvalue { i32, i32, { ptr, i64 } } %t2, 1
  %t16 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t16, i32 %t15)
  call void @string_builder_append_string(ptr %t16)
  %t17 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t17, i64 %t5)
  %t18 = load { ptr, i64 }, ptr %t17
  call void @$prn({ ptr, i64 } %t18)
  %t19 = call i64 @string_builder_mark()
  %t20 = load { i32, i32, { ptr, i64 } }, ptr %local.1
  %t21 = extractvalue { i32, i32, { ptr, i64 } } %t20, 2
  %t22 = alloca { ptr, i64 }
  %t23 = alloca { ptr, i64 }
  store { ptr, i64 } %t21, ptr %t23
  call void @to_string$string(ptr %t22, ptr %t23)
  call void @string_builder_append_string(ptr %t22)
  %t24 = alloca { ptr, i64 }
  %t25 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 1 }, ptr %t25
  call void @to_string$string(ptr %t24, ptr %t25)
  call void @string_builder_append_string(ptr %t24)
  %t26 = load { i32, i32, { ptr, i64 } }, ptr %local.1
  %t27 = extractvalue { i32, i32, { ptr, i64 } } %t26, 0
  %t28 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t28, i32 %t27)
  call void @string_builder_append_string(ptr %t28)
  %t29 = alloca { ptr, i64 }
  %t30 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.5, i64 1 }, ptr %t30
  call void @to_string$string(ptr %t29, ptr %t30)
  call void @string_builder_append_string(ptr %t29)
  %t31 = load { i32, i32, { ptr, i64 } }, ptr %local.1
  %t32 = extractvalue { i32, i32, { ptr, i64 } } %t31, 1
  %t33 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t33, i32 %t32)
  call void @string_builder_append_string(ptr %t33)
  %t34 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t34, i64 %t19)
  %t35 = load { ptr, i64 }, ptr %t34
  call void @$prn({ ptr, i64 } %t35)
  %t36 = insertvalue { i8, i32 } poison, i8 1, 0
  %t37 = insertvalue { i8, i32 } %t36, i32 2, 1
  %t38 = load { i32, i32, { ptr, i64 } }, ptr %local.1
  %t39 = extractvalue { i32, i32, { ptr, i64 } } %t38, 0
  %t40 = load { i32, i32, { ptr, i64 } }, ptr %local.1
  %t41 = extractvalue { i32, i32, { ptr, i64 } } %t40, 1
  %t42 = add i32 %t39, %t41
  %t43 = extractvalue { i8, i32 } %t37, 1
  %t44 = add i32 %t42, %t43
  ret i32 %t44
}

@$main = alias i32 (), ptr @fn.0
