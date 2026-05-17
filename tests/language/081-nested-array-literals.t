use std.io

RoomType :: enum { NONE HALL KITCHEN }

Room :: plex {
    exits [2]RoomType
}

rooms : [2]Room : [
    { exits: [NONE, HALL] },
    { exits: [KITCHEN, NONE] },
]

main :: fn () -> i32 {
    prn($"{rooms[0].exits[0] == NONE}")
    prn($"{rooms[0].exits[1] == HALL}")
    prn($"{rooms[1].exits[0] == KITCHEN}")
    prn($"{rooms[1].exits[1] == NONE}")
    return 0
}
¬
0
¬
yes
yes
yes
yes

¬
hir 0
module module.0(081-nested-array-literals.input)
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
bind RoomType = type.0
bind Room = type.1
bind rooms = value.0
bind main = fn.0
type type.0 = RoomType
type type.1 = Room
const value.0: [2]Room = [2]Room array(Room plex(exits: [2]RoomType array(RoomType NONE, RoomType HALL)), Room plex(exits: [2]RoomType array(RoomType KITCHEN, RoomType NONE)))
func fn.0() -> i32 {
  expr void call bind.2(prn)(string interpolate(bool equal(RoomType index([2]RoomType field(Room index([2]Room bind.7(rooms), untyped integer 0), exits), untyped integer 0), RoomType NONE)))
  expr void call bind.2(prn)(string interpolate(bool equal(RoomType index([2]RoomType field(Room index([2]Room bind.7(rooms), untyped integer 0), exits), untyped integer 1), RoomType HALL)))
  expr void call bind.2(prn)(string interpolate(bool equal(RoomType index([2]RoomType field(Room index([2]Room bind.7(rooms), untyped integer 1), exits), untyped integer 0), RoomType KITCHEN)))
  expr void call bind.2(prn)(string interpolate(bool equal(RoomType index([2]RoomType field(Room index([2]Room bind.7(rooms), untyped integer 1), exits), untyped integer 1), RoomType NONE)))
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

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

define internal i32 @fn.0() {
  %t0 = call i64 @string_builder_mark()
  %t1 = insertvalue { i64, i64 } poison, i64 0, 0
  %t2 = insertvalue { i64, i64 } %t1, i64 0, 1
  %t3 = insertvalue { i64, i64 } poison, i64 1, 0
  %t4 = insertvalue { i64, i64 } %t3, i64 0, 1
  %t5 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t2, 0
  %t6 = insertvalue [2 x { i64, i64 }] %t5, { i64, i64 } %t4, 1
  %t7 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t6, 0
  %t8 = insertvalue { i64, i64 } poison, i64 2, 0
  %t9 = insertvalue { i64, i64 } %t8, i64 0, 1
  %t10 = insertvalue { i64, i64 } poison, i64 0, 0
  %t11 = insertvalue { i64, i64 } %t10, i64 0, 1
  %t12 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t9, 0
  %t13 = insertvalue [2 x { i64, i64 }] %t12, { i64, i64 } %t11, 1
  %t14 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t13, 0
  %t15 = insertvalue [2 x { [2 x { i64, i64 }] }] poison, { [2 x { i64, i64 }] } %t7, 0
  %t16 = insertvalue [2 x { [2 x { i64, i64 }] }] %t15, { [2 x { i64, i64 }] } %t14, 1
  %t17 = extractvalue [2 x { [2 x { i64, i64 }] }] %t16, 0
  %t18 = extractvalue { [2 x { i64, i64 }] } %t17, 0
  %t19 = extractvalue [2 x { i64, i64 }] %t18, 0
  %t20 = insertvalue { i64, i64 } poison, i64 0, 0
  %t21 = insertvalue { i64, i64 } %t20, i64 0, 1
  %t23 = extractvalue { i64, i64 } %t19, 0
  %t24 = extractvalue { i64, i64 } %t21, 0
  %t22 = icmp eq i64 %t23, %t24
  %t25 = alloca { ptr, i64 }
  call void @to_string$bool(ptr %t25, i1 %t22)
  call void @string_builder_append_string(ptr %t25)
  %t26 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t26, i64 %t0)
  %t27 = load { ptr, i64 }, ptr %t26
  call void @$prn({ ptr, i64 } %t27)
  %t28 = call i64 @string_builder_mark()
  %t29 = insertvalue { i64, i64 } poison, i64 0, 0
  %t30 = insertvalue { i64, i64 } %t29, i64 0, 1
  %t31 = insertvalue { i64, i64 } poison, i64 1, 0
  %t32 = insertvalue { i64, i64 } %t31, i64 0, 1
  %t33 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t30, 0
  %t34 = insertvalue [2 x { i64, i64 }] %t33, { i64, i64 } %t32, 1
  %t35 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t34, 0
  %t36 = insertvalue { i64, i64 } poison, i64 2, 0
  %t37 = insertvalue { i64, i64 } %t36, i64 0, 1
  %t38 = insertvalue { i64, i64 } poison, i64 0, 0
  %t39 = insertvalue { i64, i64 } %t38, i64 0, 1
  %t40 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t37, 0
  %t41 = insertvalue [2 x { i64, i64 }] %t40, { i64, i64 } %t39, 1
  %t42 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t41, 0
  %t43 = insertvalue [2 x { [2 x { i64, i64 }] }] poison, { [2 x { i64, i64 }] } %t35, 0
  %t44 = insertvalue [2 x { [2 x { i64, i64 }] }] %t43, { [2 x { i64, i64 }] } %t42, 1
  %t45 = extractvalue [2 x { [2 x { i64, i64 }] }] %t44, 0
  %t46 = extractvalue { [2 x { i64, i64 }] } %t45, 0
  %t47 = extractvalue [2 x { i64, i64 }] %t46, 1
  %t48 = insertvalue { i64, i64 } poison, i64 1, 0
  %t49 = insertvalue { i64, i64 } %t48, i64 0, 1
  %t51 = extractvalue { i64, i64 } %t47, 0
  %t52 = extractvalue { i64, i64 } %t49, 0
  %t50 = icmp eq i64 %t51, %t52
  %t53 = alloca { ptr, i64 }
  call void @to_string$bool(ptr %t53, i1 %t50)
  call void @string_builder_append_string(ptr %t53)
  %t54 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t54, i64 %t28)
  %t55 = load { ptr, i64 }, ptr %t54
  call void @$prn({ ptr, i64 } %t55)
  %t56 = call i64 @string_builder_mark()
  %t57 = insertvalue { i64, i64 } poison, i64 0, 0
  %t58 = insertvalue { i64, i64 } %t57, i64 0, 1
  %t59 = insertvalue { i64, i64 } poison, i64 1, 0
  %t60 = insertvalue { i64, i64 } %t59, i64 0, 1
  %t61 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t58, 0
  %t62 = insertvalue [2 x { i64, i64 }] %t61, { i64, i64 } %t60, 1
  %t63 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t62, 0
  %t64 = insertvalue { i64, i64 } poison, i64 2, 0
  %t65 = insertvalue { i64, i64 } %t64, i64 0, 1
  %t66 = insertvalue { i64, i64 } poison, i64 0, 0
  %t67 = insertvalue { i64, i64 } %t66, i64 0, 1
  %t68 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t65, 0
  %t69 = insertvalue [2 x { i64, i64 }] %t68, { i64, i64 } %t67, 1
  %t70 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t69, 0
  %t71 = insertvalue [2 x { [2 x { i64, i64 }] }] poison, { [2 x { i64, i64 }] } %t63, 0
  %t72 = insertvalue [2 x { [2 x { i64, i64 }] }] %t71, { [2 x { i64, i64 }] } %t70, 1
  %t73 = extractvalue [2 x { [2 x { i64, i64 }] }] %t72, 1
  %t74 = extractvalue { [2 x { i64, i64 }] } %t73, 0
  %t75 = extractvalue [2 x { i64, i64 }] %t74, 0
  %t76 = insertvalue { i64, i64 } poison, i64 2, 0
  %t77 = insertvalue { i64, i64 } %t76, i64 0, 1
  %t79 = extractvalue { i64, i64 } %t75, 0
  %t80 = extractvalue { i64, i64 } %t77, 0
  %t78 = icmp eq i64 %t79, %t80
  %t81 = alloca { ptr, i64 }
  call void @to_string$bool(ptr %t81, i1 %t78)
  call void @string_builder_append_string(ptr %t81)
  %t82 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t82, i64 %t56)
  %t83 = load { ptr, i64 }, ptr %t82
  call void @$prn({ ptr, i64 } %t83)
  %t84 = call i64 @string_builder_mark()
  %t85 = insertvalue { i64, i64 } poison, i64 0, 0
  %t86 = insertvalue { i64, i64 } %t85, i64 0, 1
  %t87 = insertvalue { i64, i64 } poison, i64 1, 0
  %t88 = insertvalue { i64, i64 } %t87, i64 0, 1
  %t89 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t86, 0
  %t90 = insertvalue [2 x { i64, i64 }] %t89, { i64, i64 } %t88, 1
  %t91 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t90, 0
  %t92 = insertvalue { i64, i64 } poison, i64 2, 0
  %t93 = insertvalue { i64, i64 } %t92, i64 0, 1
  %t94 = insertvalue { i64, i64 } poison, i64 0, 0
  %t95 = insertvalue { i64, i64 } %t94, i64 0, 1
  %t96 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t93, 0
  %t97 = insertvalue [2 x { i64, i64 }] %t96, { i64, i64 } %t95, 1
  %t98 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t97, 0
  %t99 = insertvalue [2 x { [2 x { i64, i64 }] }] poison, { [2 x { i64, i64 }] } %t91, 0
  %t100 = insertvalue [2 x { [2 x { i64, i64 }] }] %t99, { [2 x { i64, i64 }] } %t98, 1
  %t101 = extractvalue [2 x { [2 x { i64, i64 }] }] %t100, 1
  %t102 = extractvalue { [2 x { i64, i64 }] } %t101, 0
  %t103 = extractvalue [2 x { i64, i64 }] %t102, 1
  %t104 = insertvalue { i64, i64 } poison, i64 0, 0
  %t105 = insertvalue { i64, i64 } %t104, i64 0, 1
  %t107 = extractvalue { i64, i64 } %t103, 0
  %t108 = extractvalue { i64, i64 } %t105, 0
  %t106 = icmp eq i64 %t107, %t108
  %t109 = alloca { ptr, i64 }
  call void @to_string$bool(ptr %t109, i1 %t106)
  call void @string_builder_append_string(ptr %t109)
  %t110 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t110, i64 %t84)
  %t111 = load { ptr, i64 }, ptr %t110
  call void @$prn({ ptr, i64 } %t111)
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
