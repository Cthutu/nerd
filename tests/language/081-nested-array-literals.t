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
  %t25 = call { ptr, i64 } @to_string$bool(i1 %t22)
  call void @string_builder_append_string({ ptr, i64 } %t25)
  %t26 = call { ptr, i64 } @string_builder_finish(i64 %t0)
  call void @$prn({ ptr, i64 } %t26)
  %t27 = call i64 @string_builder_mark()
  %t28 = insertvalue { i64, i64 } poison, i64 0, 0
  %t29 = insertvalue { i64, i64 } %t28, i64 0, 1
  %t30 = insertvalue { i64, i64 } poison, i64 1, 0
  %t31 = insertvalue { i64, i64 } %t30, i64 0, 1
  %t32 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t29, 0
  %t33 = insertvalue [2 x { i64, i64 }] %t32, { i64, i64 } %t31, 1
  %t34 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t33, 0
  %t35 = insertvalue { i64, i64 } poison, i64 2, 0
  %t36 = insertvalue { i64, i64 } %t35, i64 0, 1
  %t37 = insertvalue { i64, i64 } poison, i64 0, 0
  %t38 = insertvalue { i64, i64 } %t37, i64 0, 1
  %t39 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t36, 0
  %t40 = insertvalue [2 x { i64, i64 }] %t39, { i64, i64 } %t38, 1
  %t41 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t40, 0
  %t42 = insertvalue [2 x { [2 x { i64, i64 }] }] poison, { [2 x { i64, i64 }] } %t34, 0
  %t43 = insertvalue [2 x { [2 x { i64, i64 }] }] %t42, { [2 x { i64, i64 }] } %t41, 1
  %t44 = extractvalue [2 x { [2 x { i64, i64 }] }] %t43, 0
  %t45 = extractvalue { [2 x { i64, i64 }] } %t44, 0
  %t46 = extractvalue [2 x { i64, i64 }] %t45, 1
  %t47 = insertvalue { i64, i64 } poison, i64 1, 0
  %t48 = insertvalue { i64, i64 } %t47, i64 0, 1
  %t50 = extractvalue { i64, i64 } %t46, 0
  %t51 = extractvalue { i64, i64 } %t48, 0
  %t49 = icmp eq i64 %t50, %t51
  %t52 = call { ptr, i64 } @to_string$bool(i1 %t49)
  call void @string_builder_append_string({ ptr, i64 } %t52)
  %t53 = call { ptr, i64 } @string_builder_finish(i64 %t27)
  call void @$prn({ ptr, i64 } %t53)
  %t54 = call i64 @string_builder_mark()
  %t55 = insertvalue { i64, i64 } poison, i64 0, 0
  %t56 = insertvalue { i64, i64 } %t55, i64 0, 1
  %t57 = insertvalue { i64, i64 } poison, i64 1, 0
  %t58 = insertvalue { i64, i64 } %t57, i64 0, 1
  %t59 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t56, 0
  %t60 = insertvalue [2 x { i64, i64 }] %t59, { i64, i64 } %t58, 1
  %t61 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t60, 0
  %t62 = insertvalue { i64, i64 } poison, i64 2, 0
  %t63 = insertvalue { i64, i64 } %t62, i64 0, 1
  %t64 = insertvalue { i64, i64 } poison, i64 0, 0
  %t65 = insertvalue { i64, i64 } %t64, i64 0, 1
  %t66 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t63, 0
  %t67 = insertvalue [2 x { i64, i64 }] %t66, { i64, i64 } %t65, 1
  %t68 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t67, 0
  %t69 = insertvalue [2 x { [2 x { i64, i64 }] }] poison, { [2 x { i64, i64 }] } %t61, 0
  %t70 = insertvalue [2 x { [2 x { i64, i64 }] }] %t69, { [2 x { i64, i64 }] } %t68, 1
  %t71 = extractvalue [2 x { [2 x { i64, i64 }] }] %t70, 1
  %t72 = extractvalue { [2 x { i64, i64 }] } %t71, 0
  %t73 = extractvalue [2 x { i64, i64 }] %t72, 0
  %t74 = insertvalue { i64, i64 } poison, i64 2, 0
  %t75 = insertvalue { i64, i64 } %t74, i64 0, 1
  %t77 = extractvalue { i64, i64 } %t73, 0
  %t78 = extractvalue { i64, i64 } %t75, 0
  %t76 = icmp eq i64 %t77, %t78
  %t79 = call { ptr, i64 } @to_string$bool(i1 %t76)
  call void @string_builder_append_string({ ptr, i64 } %t79)
  %t80 = call { ptr, i64 } @string_builder_finish(i64 %t54)
  call void @$prn({ ptr, i64 } %t80)
  %t81 = call i64 @string_builder_mark()
  %t82 = insertvalue { i64, i64 } poison, i64 0, 0
  %t83 = insertvalue { i64, i64 } %t82, i64 0, 1
  %t84 = insertvalue { i64, i64 } poison, i64 1, 0
  %t85 = insertvalue { i64, i64 } %t84, i64 0, 1
  %t86 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t83, 0
  %t87 = insertvalue [2 x { i64, i64 }] %t86, { i64, i64 } %t85, 1
  %t88 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t87, 0
  %t89 = insertvalue { i64, i64 } poison, i64 2, 0
  %t90 = insertvalue { i64, i64 } %t89, i64 0, 1
  %t91 = insertvalue { i64, i64 } poison, i64 0, 0
  %t92 = insertvalue { i64, i64 } %t91, i64 0, 1
  %t93 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t90, 0
  %t94 = insertvalue [2 x { i64, i64 }] %t93, { i64, i64 } %t92, 1
  %t95 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t94, 0
  %t96 = insertvalue [2 x { [2 x { i64, i64 }] }] poison, { [2 x { i64, i64 }] } %t88, 0
  %t97 = insertvalue [2 x { [2 x { i64, i64 }] }] %t96, { [2 x { i64, i64 }] } %t95, 1
  %t98 = extractvalue [2 x { [2 x { i64, i64 }] }] %t97, 1
  %t99 = extractvalue { [2 x { i64, i64 }] } %t98, 0
  %t100 = extractvalue [2 x { i64, i64 }] %t99, 1
  %t101 = insertvalue { i64, i64 } poison, i64 0, 0
  %t102 = insertvalue { i64, i64 } %t101, i64 0, 1
  %t104 = extractvalue { i64, i64 } %t100, 0
  %t105 = extractvalue { i64, i64 } %t102, 0
  %t103 = icmp eq i64 %t104, %t105
  %t106 = call { ptr, i64 } @to_string$bool(i1 %t103)
  call void @string_builder_append_string({ ptr, i64 } %t106)
  %t107 = call { ptr, i64 } @string_builder_finish(i64 %t81)
  call void @$prn({ ptr, i64 } %t107)
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
