use std.io

Point :: plex { x i32 y i32 }

origin : Point : { x: 9, y: 8 }
points : []Point : [
    { x: 1, y: 2 },
    { x: 3, y: 4 },
]

show_last :: fn (items: []Point) {
    last :: items[items.count - 1]
    prn($"{last.x} {last.y}")
}

main :: fn () {
    p : Point = { x: 5, y: 6 }
    prn($"{origin.x} {origin.y}")
    prn($"{points[0].x} {points[1].y}")
    prn($"{p.x} {p.y}")
    show_last([
        { x: 13, y: 14 },
        { x: 15, y: 16 },
    ])
}
¬
0
¬
9 8
1 4
5 6
15 16

¬
hir 0
module module.0(079-contextual-plex-and-slice.input)
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
bind origin = value.0
bind points = value.1
bind show_last = fn.0
bind main = fn.1
type type.0 = Point
const value.0: Point = Point plex(x: i32 9, y: i32 8)
const value.1: []Point = []Point array(Point plex(x: i32 1, y: i32 2), Point plex(x: i32 3, y: i32 4))
func fn.0(items: []Point) -> void {
  let last: Point = Point index([]Point local.0(items), usize subtract(usize field([]Point local.0(items), count), usize 1))
  expr void call bind.2(prn)(string interpolate(i32 field(Point local.1(last), x), <unknown> " ", i32 field(Point local.1(last), y)))
}
func fn.1() -> void {
  let p: Point = Point plex(x: i32 5, y: i32 6)
  expr void call bind.2(prn)(string interpolate(i32 field(Point bind.6(origin), x), <unknown> " ", i32 field(Point bind.6(origin), y)))
  expr void call bind.2(prn)(string interpolate(i32 field(Point index([]Point bind.7(points), untyped integer 0), x), <unknown> " ", i32 field(Point index([]Point bind.7(points), untyped integer 1), y)))
  expr void call bind.2(prn)(string interpolate(i32 field(Point local.2(p), x), <unknown> " ", i32 field(Point local.2(p), y)))
  expr void call bind.8(show_last)([]Point array(Point plex(x: i32 13, y: i32 14), Point plex(x: i32 15, y: i32 16)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.2 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.3 = private unnamed_addr constant [2 x i8] c" \00"

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

define void @fn.0({ ptr, i64 } %items) {
  %t0 = extractvalue { ptr, i64 } %items, 1
  %t1 = sub i64 %t0, 1
  %t2 = extractvalue { ptr, i64 } %items, 0
  %t3 = getelementptr inbounds { i32, i32 }, ptr %t2, i64 %t1
  %t4 = load { i32, i32 }, ptr %t3
  %t5 = call i64 @string_builder_mark()
  %t6 = extractvalue { i32, i32 } %t4, 0
  %t7 = call { ptr, i64 } @to_string$i32(i32 %t6)
  call void @string_builder_append_string({ ptr, i64 } %t7)
  %t8 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t8)
  %t9 = extractvalue { i32, i32 } %t4, 1
  %t10 = call { ptr, i64 } @to_string$i32(i32 %t9)
  call void @string_builder_append_string({ ptr, i64 } %t10)
  %t11 = call { ptr, i64 } @string_builder_finish(i64 %t5)
  call void @$prn({ ptr, i64 } %t11)
  ret void
}

define void @fn.1() {
  %t0 = insertvalue { i32, i32 } poison, i32 5, 0
  %t1 = insertvalue { i32, i32 } %t0, i32 6, 1
  %t2 = call i64 @string_builder_mark()
  %t3 = insertvalue { i32, i32 } poison, i32 9, 0
  %t4 = insertvalue { i32, i32 } %t3, i32 8, 1
  %t5 = extractvalue { i32, i32 } %t4, 0
  %t6 = call { ptr, i64 } @to_string$i32(i32 %t5)
  call void @string_builder_append_string({ ptr, i64 } %t6)
  %t7 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t7)
  %t8 = insertvalue { i32, i32 } poison, i32 9, 0
  %t9 = insertvalue { i32, i32 } %t8, i32 8, 1
  %t10 = extractvalue { i32, i32 } %t9, 1
  %t11 = call { ptr, i64 } @to_string$i32(i32 %t10)
  call void @string_builder_append_string({ ptr, i64 } %t11)
  %t12 = call { ptr, i64 } @string_builder_finish(i64 %t2)
  call void @$prn({ ptr, i64 } %t12)
  %t13 = call i64 @string_builder_mark()
  %t14 = insertvalue { i32, i32 } poison, i32 1, 0
  %t15 = insertvalue { i32, i32 } %t14, i32 2, 1
  %t16 = insertvalue { i32, i32 } poison, i32 3, 0
  %t17 = insertvalue { i32, i32 } %t16, i32 4, 1
  %t18 = insertvalue [2 x { i32, i32 }] poison, { i32, i32 } %t15, 0
  %t19 = insertvalue [2 x { i32, i32 }] %t18, { i32, i32 } %t17, 1
  %t20 = alloca [2 x { i32, i32 }]
  store [2 x { i32, i32 }] %t19, ptr %t20
  %t21 = getelementptr inbounds [2 x { i32, i32 }], ptr %t20, i64 0, i64 0
  %t22 = insertvalue { ptr, i64 } poison, ptr %t21, 0
  %t23 = insertvalue { ptr, i64 } %t22, i64 2, 1
  %t24 = extractvalue { ptr, i64 } %t23, 0
  %t25 = getelementptr inbounds { i32, i32 }, ptr %t24, i32 0
  %t26 = load { i32, i32 }, ptr %t25
  %t27 = extractvalue { i32, i32 } %t26, 0
  %t28 = call { ptr, i64 } @to_string$i32(i32 %t27)
  call void @string_builder_append_string({ ptr, i64 } %t28)
  %t29 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t29)
  %t30 = insertvalue { i32, i32 } poison, i32 1, 0
  %t31 = insertvalue { i32, i32 } %t30, i32 2, 1
  %t32 = insertvalue { i32, i32 } poison, i32 3, 0
  %t33 = insertvalue { i32, i32 } %t32, i32 4, 1
  %t34 = insertvalue [2 x { i32, i32 }] poison, { i32, i32 } %t31, 0
  %t35 = insertvalue [2 x { i32, i32 }] %t34, { i32, i32 } %t33, 1
  %t36 = alloca [2 x { i32, i32 }]
  store [2 x { i32, i32 }] %t35, ptr %t36
  %t37 = getelementptr inbounds [2 x { i32, i32 }], ptr %t36, i64 0, i64 0
  %t38 = insertvalue { ptr, i64 } poison, ptr %t37, 0
  %t39 = insertvalue { ptr, i64 } %t38, i64 2, 1
  %t40 = extractvalue { ptr, i64 } %t39, 0
  %t41 = getelementptr inbounds { i32, i32 }, ptr %t40, i32 1
  %t42 = load { i32, i32 }, ptr %t41
  %t43 = extractvalue { i32, i32 } %t42, 1
  %t44 = call { ptr, i64 } @to_string$i32(i32 %t43)
  call void @string_builder_append_string({ ptr, i64 } %t44)
  %t45 = call { ptr, i64 } @string_builder_finish(i64 %t13)
  call void @$prn({ ptr, i64 } %t45)
  %t46 = call i64 @string_builder_mark()
  %t47 = extractvalue { i32, i32 } %t1, 0
  %t48 = call { ptr, i64 } @to_string$i32(i32 %t47)
  call void @string_builder_append_string({ ptr, i64 } %t48)
  %t49 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.3, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t49)
  %t50 = extractvalue { i32, i32 } %t1, 1
  %t51 = call { ptr, i64 } @to_string$i32(i32 %t50)
  call void @string_builder_append_string({ ptr, i64 } %t51)
  %t52 = call { ptr, i64 } @string_builder_finish(i64 %t46)
  call void @$prn({ ptr, i64 } %t52)
  %t53 = insertvalue { i32, i32 } poison, i32 13, 0
  %t54 = insertvalue { i32, i32 } %t53, i32 14, 1
  %t55 = insertvalue { i32, i32 } poison, i32 15, 0
  %t56 = insertvalue { i32, i32 } %t55, i32 16, 1
  %t57 = insertvalue [2 x { i32, i32 }] poison, { i32, i32 } %t54, 0
  %t58 = insertvalue [2 x { i32, i32 }] %t57, { i32, i32 } %t56, 1
  %t59 = alloca [2 x { i32, i32 }]
  store [2 x { i32, i32 }] %t58, ptr %t59
  %t60 = getelementptr inbounds [2 x { i32, i32 }], ptr %t59, i64 0, i64 0
  %t61 = insertvalue { ptr, i64 } poison, ptr %t60, 0
  %t62 = insertvalue { ptr, i64 } %t61, i64 2, 1
  call void @fn.0({ ptr, i64 } %t62)
  ret void
}

@$show_last = internal alias void ({ ptr, i64 }), ptr @fn.0
@$main = alias void (), ptr @fn.1
