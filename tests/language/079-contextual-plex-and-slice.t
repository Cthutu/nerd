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
import import.0 pr from module.1(std.io).decl.6: fn (string) -> void
import import.1 epr from module.1(std.io).decl.7: fn (string) -> void
import import.2 prn from module.1(std.io).decl.8: fn (string) -> void
import import.3 eprn from module.1(std.io).decl.9: fn (string) -> void
import import.4 input from module.1(std.io).decl.10: fn (string) -> string
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

declare void @$pr({ ptr, i64 })
declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

define internal void @fn.0({ ptr, i64 } %items) {
  %t0 = extractvalue { ptr, i64 } %items, 1
  %t1 = sub i64 %t0, 1
  %t2 = extractvalue { ptr, i64 } %items, 0
  %t3 = getelementptr inbounds { i32, i32 }, ptr %t2, i64 %t1
  %t4 = load { i32, i32 }, ptr %t3
  %t5 = call i64 @string_builder_mark()
  %t6 = extractvalue { i32, i32 } %t4, 0
  %t7 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t7, i32 %t6)
  call void @string_builder_append_string(ptr %t7)
  %t8 = alloca { ptr, i64 }
  %t9 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 1 }, ptr %t9
  call void @to_string$string(ptr %t8, ptr %t9)
  call void @string_builder_append_string(ptr %t8)
  %t10 = extractvalue { i32, i32 } %t4, 1
  %t11 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t11, i32 %t10)
  call void @string_builder_append_string(ptr %t11)
  %t12 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t12, i64 %t5)
  %t13 = load { ptr, i64 }, ptr %t12
  call void @$prn({ ptr, i64 } %t13)
  ret void
}

define internal void @fn.1() {
  %t0 = insertvalue { i32, i32 } poison, i32 5, 0
  %t1 = insertvalue { i32, i32 } %t0, i32 6, 1
  %t2 = call i64 @string_builder_mark()
  %t3 = insertvalue { i32, i32 } poison, i32 9, 0
  %t4 = insertvalue { i32, i32 } %t3, i32 8, 1
  %t5 = extractvalue { i32, i32 } %t4, 0
  %t6 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t6, i32 %t5)
  call void @string_builder_append_string(ptr %t6)
  %t7 = alloca { ptr, i64 }
  %t8 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 1 }, ptr %t8
  call void @to_string$string(ptr %t7, ptr %t8)
  call void @string_builder_append_string(ptr %t7)
  %t9 = insertvalue { i32, i32 } poison, i32 9, 0
  %t10 = insertvalue { i32, i32 } %t9, i32 8, 1
  %t11 = extractvalue { i32, i32 } %t10, 1
  %t12 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t12, i32 %t11)
  call void @string_builder_append_string(ptr %t12)
  %t13 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t13, i64 %t2)
  %t14 = load { ptr, i64 }, ptr %t13
  call void @$prn({ ptr, i64 } %t14)
  %t15 = call i64 @string_builder_mark()
  %t16 = insertvalue { i32, i32 } poison, i32 1, 0
  %t17 = insertvalue { i32, i32 } %t16, i32 2, 1
  %t18 = insertvalue { i32, i32 } poison, i32 3, 0
  %t19 = insertvalue { i32, i32 } %t18, i32 4, 1
  %t20 = insertvalue [2 x { i32, i32 }] poison, { i32, i32 } %t17, 0
  %t21 = insertvalue [2 x { i32, i32 }] %t20, { i32, i32 } %t19, 1
  %t22 = alloca [2 x { i32, i32 }]
  store [2 x { i32, i32 }] %t21, ptr %t22
  %t23 = getelementptr inbounds [2 x { i32, i32 }], ptr %t22, i64 0, i64 0
  %t24 = insertvalue { ptr, i64 } poison, ptr %t23, 0
  %t25 = insertvalue { ptr, i64 } %t24, i64 2, 1
  %t26 = extractvalue { ptr, i64 } %t25, 0
  %t27 = getelementptr inbounds { i32, i32 }, ptr %t26, i32 0
  %t28 = load { i32, i32 }, ptr %t27
  %t29 = extractvalue { i32, i32 } %t28, 0
  %t30 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t30, i32 %t29)
  call void @string_builder_append_string(ptr %t30)
  %t31 = alloca { ptr, i64 }
  %t32 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 1 }, ptr %t32
  call void @to_string$string(ptr %t31, ptr %t32)
  call void @string_builder_append_string(ptr %t31)
  %t33 = insertvalue { i32, i32 } poison, i32 1, 0
  %t34 = insertvalue { i32, i32 } %t33, i32 2, 1
  %t35 = insertvalue { i32, i32 } poison, i32 3, 0
  %t36 = insertvalue { i32, i32 } %t35, i32 4, 1
  %t37 = insertvalue [2 x { i32, i32 }] poison, { i32, i32 } %t34, 0
  %t38 = insertvalue [2 x { i32, i32 }] %t37, { i32, i32 } %t36, 1
  %t39 = alloca [2 x { i32, i32 }]
  store [2 x { i32, i32 }] %t38, ptr %t39
  %t40 = getelementptr inbounds [2 x { i32, i32 }], ptr %t39, i64 0, i64 0
  %t41 = insertvalue { ptr, i64 } poison, ptr %t40, 0
  %t42 = insertvalue { ptr, i64 } %t41, i64 2, 1
  %t43 = extractvalue { ptr, i64 } %t42, 0
  %t44 = getelementptr inbounds { i32, i32 }, ptr %t43, i32 1
  %t45 = load { i32, i32 }, ptr %t44
  %t46 = extractvalue { i32, i32 } %t45, 1
  %t47 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t47, i32 %t46)
  call void @string_builder_append_string(ptr %t47)
  %t48 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t48, i64 %t15)
  %t49 = load { ptr, i64 }, ptr %t48
  call void @$prn({ ptr, i64 } %t49)
  %t50 = call i64 @string_builder_mark()
  %t51 = extractvalue { i32, i32 } %t1, 0
  %t52 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t52, i32 %t51)
  call void @string_builder_append_string(ptr %t52)
  %t53 = alloca { ptr, i64 }
  %t54 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 1 }, ptr %t54
  call void @to_string$string(ptr %t53, ptr %t54)
  call void @string_builder_append_string(ptr %t53)
  %t55 = extractvalue { i32, i32 } %t1, 1
  %t56 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t56, i32 %t55)
  call void @string_builder_append_string(ptr %t56)
  %t57 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t57, i64 %t50)
  %t58 = load { ptr, i64 }, ptr %t57
  call void @$prn({ ptr, i64 } %t58)
  %t59 = insertvalue { i32, i32 } poison, i32 13, 0
  %t60 = insertvalue { i32, i32 } %t59, i32 14, 1
  %t61 = insertvalue { i32, i32 } poison, i32 15, 0
  %t62 = insertvalue { i32, i32 } %t61, i32 16, 1
  %t63 = insertvalue [2 x { i32, i32 }] poison, { i32, i32 } %t60, 0
  %t64 = insertvalue [2 x { i32, i32 }] %t63, { i32, i32 } %t62, 1
  %t65 = alloca [2 x { i32, i32 }]
  store [2 x { i32, i32 }] %t64, ptr %t65
  %t66 = getelementptr inbounds [2 x { i32, i32 }], ptr %t65, i64 0, i64 0
  %t67 = insertvalue { ptr, i64 } poison, ptr %t66, 0
  %t68 = insertvalue { ptr, i64 } %t67, i64 2, 1
  call void @fn.0({ ptr, i64 } %t68)
  ret void
}

@$show_last = internal alias void ({ ptr, i64 }), ptr @fn.0
@$main = alias void (), ptr @fn.1
