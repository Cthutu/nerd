use std.io

Point :: plex {
    x i32
    y i32
}

impl Display for Point {
    show :: fn (self: Self) => $"Point({self.x}, {self.y})"
}

Display :: trait {
    show :: fn (Self) -> string
}

main :: fn () -> i32 {
    point := Point { x: 3, y: 4 }
    prn(point.show())
    return point.x + point.y
}
¬
7
¬
Point(3, 4)

¬
hir 0
module module.0(162-trait-impl.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind Point = type.0
bind __impl_8_show = fn.0
bind main = fn.1
type type.0 = Point
func fn.0(self: Point) -> string {
  return string interpolate(<unknown> "Point(", i32 field(Point local.0(self), x), <unknown> ", ", i32 field(Point local.0(self), y), <unknown> ")")
}
func fn.1() -> i32 {
  let point: Point = Point plex(x: i32 3, y: i32 4)
  expr void call bind.0(prn)(string call bind.3(__impl_8_show)(Point local.1(point)))
  return i32 add(i32 field(Point local.1(point), x), i32 field(Point local.1(point), y))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [54 x i8] c"tests/language/162-trait-impl.t\00"
@.str.m0.0 = private unnamed_addr constant [7 x i8] c"Point(\00"
@.str.m0.1 = private unnamed_addr constant [3 x i8] c", \00"
@.str.m0.2 = private unnamed_addr constant [2 x i8] c")\00"

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

define internal { ptr, i64 } @fn.0({ i32, i32 } %self) {
  %t0 = call i64 @string_builder_mark()
  %t1 = alloca { ptr, i64 }
  %t2 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 6 }, ptr %t2
  call void @to_string$string(ptr %t1, ptr %t2)
  call void @string_builder_append_string(ptr %t1)
  %t3 = extractvalue { i32, i32 } %self, 0
  %t4 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t4, i32 %t3)
  call void @string_builder_append_string(ptr %t4)
  %t5 = alloca { ptr, i64 }
  %t6 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 2 }, ptr %t6
  call void @to_string$string(ptr %t5, ptr %t6)
  call void @string_builder_append_string(ptr %t5)
  %t7 = extractvalue { i32, i32 } %self, 1
  %t8 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t8, i32 %t7)
  call void @string_builder_append_string(ptr %t8)
  %t9 = alloca { ptr, i64 }
  %t10 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 1 }, ptr %t10
  call void @to_string$string(ptr %t9, ptr %t10)
  call void @string_builder_append_string(ptr %t9)
  %t11 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t11, i64 %t0)
  %t12 = load { ptr, i64 }, ptr %t11
  ret { ptr, i64 } %t12
}

define internal i32 @fn.1() {
  %t0 = insertvalue { i32, i32 } poison, i32 3, 0
  %t1 = insertvalue { i32, i32 } %t0, i32 4, 1
  %t2 = call { ptr, i64 } @fn.0({ i32, i32 } %t1)
  call void @$prn({ ptr, i64 } %t2)
  %t3 = extractvalue { i32, i32 } %t1, 0
  %t4 = extractvalue { i32, i32 } %t1, 1
  %t5 = add i32 %t3, %t4
  ret i32 %t5
}

@$__impl_8_show = internal alias { ptr, i64 } ({ i32, i32 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
