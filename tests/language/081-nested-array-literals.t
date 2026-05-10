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

define i32 @fn.0() {
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
  %t17 = insertvalue { i64, i64 } poison, i64 0, 0
  %t18 = insertvalue { i64, i64 } %t17, i64 0, 1
  %t19 = insertvalue { i64, i64 } poison, i64 1, 0
  %t20 = insertvalue { i64, i64 } %t19, i64 0, 1
  %t21 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t18, 0
  %t22 = insertvalue [2 x { i64, i64 }] %t21, { i64, i64 } %t20, 1
  %t23 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t22, 0
  %t24 = insertvalue { i64, i64 } poison, i64 2, 0
  %t25 = insertvalue { i64, i64 } %t24, i64 0, 1
  %t26 = insertvalue { i64, i64 } poison, i64 0, 0
  %t27 = insertvalue { i64, i64 } %t26, i64 0, 1
  %t28 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t25, 0
  %t29 = insertvalue [2 x { i64, i64 }] %t28, { i64, i64 } %t27, 1
  %t30 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t29, 0
  %t31 = insertvalue [2 x { [2 x { i64, i64 }] }] poison, { [2 x { i64, i64 }] } %t23, 0
  %t32 = insertvalue [2 x { [2 x { i64, i64 }] }] %t31, { [2 x { i64, i64 }] } %t30, 1
  %t33 = alloca [2 x { [2 x { i64, i64 }] }]
  store [2 x { [2 x { i64, i64 }] }] %t32, ptr %t33
  %t34 = getelementptr inbounds [2 x { [2 x { i64, i64 }] }], ptr %t33, i64 0, i32 0
  %t35 = load { [2 x { i64, i64 }] }, ptr %t34
  %t36 = extractvalue { [2 x { i64, i64 }] } %t35, 0
  %t37 = insertvalue { i64, i64 } poison, i64 0, 0
  %t38 = insertvalue { i64, i64 } %t37, i64 0, 1
  %t39 = insertvalue { i64, i64 } poison, i64 1, 0
  %t40 = insertvalue { i64, i64 } %t39, i64 0, 1
  %t41 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t38, 0
  %t42 = insertvalue [2 x { i64, i64 }] %t41, { i64, i64 } %t40, 1
  %t43 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t42, 0
  %t44 = insertvalue { i64, i64 } poison, i64 2, 0
  %t45 = insertvalue { i64, i64 } %t44, i64 0, 1
  %t46 = insertvalue { i64, i64 } poison, i64 0, 0
  %t47 = insertvalue { i64, i64 } %t46, i64 0, 1
  %t48 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t45, 0
  %t49 = insertvalue [2 x { i64, i64 }] %t48, { i64, i64 } %t47, 1
  %t50 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t49, 0
  %t51 = insertvalue [2 x { [2 x { i64, i64 }] }] poison, { [2 x { i64, i64 }] } %t43, 0
  %t52 = insertvalue [2 x { [2 x { i64, i64 }] }] %t51, { [2 x { i64, i64 }] } %t50, 1
  %t53 = alloca [2 x { [2 x { i64, i64 }] }]
  store [2 x { [2 x { i64, i64 }] }] %t52, ptr %t53
  %t54 = getelementptr inbounds [2 x { [2 x { i64, i64 }] }], ptr %t53, i64 0, i32 0
  %t55 = getelementptr inbounds { [2 x { i64, i64 }] }, ptr %t54, i64 0, i32 0
  %t56 = getelementptr inbounds [2 x { i64, i64 }], ptr %t55, i64 0, i32 0
  %t57 = load { i64, i64 }, ptr %t56
  %t58 = insertvalue { i64, i64 } poison, i64 0, 0
  %t59 = insertvalue { i64, i64 } %t58, i64 0, 1
  %t61 = extractvalue { i64, i64 } %t57, 0
  %t62 = extractvalue { i64, i64 } %t59, 0
  %t60 = icmp eq i64 %t61, %t62
  %t63 = call { ptr, i64 } @to_string$bool(i1 %t60)
  call void @string_builder_append_string({ ptr, i64 } %t63)
  %t64 = call { ptr, i64 } @string_builder_finish(i64 %t0)
  call void @$prn({ ptr, i64 } %t64)
  %t65 = call i64 @string_builder_mark()
  %t66 = insertvalue { i64, i64 } poison, i64 0, 0
  %t67 = insertvalue { i64, i64 } %t66, i64 0, 1
  %t68 = insertvalue { i64, i64 } poison, i64 1, 0
  %t69 = insertvalue { i64, i64 } %t68, i64 0, 1
  %t70 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t67, 0
  %t71 = insertvalue [2 x { i64, i64 }] %t70, { i64, i64 } %t69, 1
  %t72 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t71, 0
  %t73 = insertvalue { i64, i64 } poison, i64 2, 0
  %t74 = insertvalue { i64, i64 } %t73, i64 0, 1
  %t75 = insertvalue { i64, i64 } poison, i64 0, 0
  %t76 = insertvalue { i64, i64 } %t75, i64 0, 1
  %t77 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t74, 0
  %t78 = insertvalue [2 x { i64, i64 }] %t77, { i64, i64 } %t76, 1
  %t79 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t78, 0
  %t80 = insertvalue [2 x { [2 x { i64, i64 }] }] poison, { [2 x { i64, i64 }] } %t72, 0
  %t81 = insertvalue [2 x { [2 x { i64, i64 }] }] %t80, { [2 x { i64, i64 }] } %t79, 1
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
  %t98 = alloca [2 x { [2 x { i64, i64 }] }]
  store [2 x { [2 x { i64, i64 }] }] %t97, ptr %t98
  %t99 = getelementptr inbounds [2 x { [2 x { i64, i64 }] }], ptr %t98, i64 0, i32 0
  %t100 = load { [2 x { i64, i64 }] }, ptr %t99
  %t101 = extractvalue { [2 x { i64, i64 }] } %t100, 0
  %t102 = insertvalue { i64, i64 } poison, i64 0, 0
  %t103 = insertvalue { i64, i64 } %t102, i64 0, 1
  %t104 = insertvalue { i64, i64 } poison, i64 1, 0
  %t105 = insertvalue { i64, i64 } %t104, i64 0, 1
  %t106 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t103, 0
  %t107 = insertvalue [2 x { i64, i64 }] %t106, { i64, i64 } %t105, 1
  %t108 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t107, 0
  %t109 = insertvalue { i64, i64 } poison, i64 2, 0
  %t110 = insertvalue { i64, i64 } %t109, i64 0, 1
  %t111 = insertvalue { i64, i64 } poison, i64 0, 0
  %t112 = insertvalue { i64, i64 } %t111, i64 0, 1
  %t113 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t110, 0
  %t114 = insertvalue [2 x { i64, i64 }] %t113, { i64, i64 } %t112, 1
  %t115 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t114, 0
  %t116 = insertvalue [2 x { [2 x { i64, i64 }] }] poison, { [2 x { i64, i64 }] } %t108, 0
  %t117 = insertvalue [2 x { [2 x { i64, i64 }] }] %t116, { [2 x { i64, i64 }] } %t115, 1
  %t118 = alloca [2 x { [2 x { i64, i64 }] }]
  store [2 x { [2 x { i64, i64 }] }] %t117, ptr %t118
  %t119 = getelementptr inbounds [2 x { [2 x { i64, i64 }] }], ptr %t118, i64 0, i32 0
  %t120 = getelementptr inbounds { [2 x { i64, i64 }] }, ptr %t119, i64 0, i32 0
  %t121 = getelementptr inbounds [2 x { i64, i64 }], ptr %t120, i64 0, i32 1
  %t122 = load { i64, i64 }, ptr %t121
  %t123 = insertvalue { i64, i64 } poison, i64 1, 0
  %t124 = insertvalue { i64, i64 } %t123, i64 0, 1
  %t126 = extractvalue { i64, i64 } %t122, 0
  %t127 = extractvalue { i64, i64 } %t124, 0
  %t125 = icmp eq i64 %t126, %t127
  %t128 = call { ptr, i64 } @to_string$bool(i1 %t125)
  call void @string_builder_append_string({ ptr, i64 } %t128)
  %t129 = call { ptr, i64 } @string_builder_finish(i64 %t65)
  call void @$prn({ ptr, i64 } %t129)
  %t130 = call i64 @string_builder_mark()
  %t131 = insertvalue { i64, i64 } poison, i64 0, 0
  %t132 = insertvalue { i64, i64 } %t131, i64 0, 1
  %t133 = insertvalue { i64, i64 } poison, i64 1, 0
  %t134 = insertvalue { i64, i64 } %t133, i64 0, 1
  %t135 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t132, 0
  %t136 = insertvalue [2 x { i64, i64 }] %t135, { i64, i64 } %t134, 1
  %t137 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t136, 0
  %t138 = insertvalue { i64, i64 } poison, i64 2, 0
  %t139 = insertvalue { i64, i64 } %t138, i64 0, 1
  %t140 = insertvalue { i64, i64 } poison, i64 0, 0
  %t141 = insertvalue { i64, i64 } %t140, i64 0, 1
  %t142 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t139, 0
  %t143 = insertvalue [2 x { i64, i64 }] %t142, { i64, i64 } %t141, 1
  %t144 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t143, 0
  %t145 = insertvalue [2 x { [2 x { i64, i64 }] }] poison, { [2 x { i64, i64 }] } %t137, 0
  %t146 = insertvalue [2 x { [2 x { i64, i64 }] }] %t145, { [2 x { i64, i64 }] } %t144, 1
  %t147 = insertvalue { i64, i64 } poison, i64 0, 0
  %t148 = insertvalue { i64, i64 } %t147, i64 0, 1
  %t149 = insertvalue { i64, i64 } poison, i64 1, 0
  %t150 = insertvalue { i64, i64 } %t149, i64 0, 1
  %t151 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t148, 0
  %t152 = insertvalue [2 x { i64, i64 }] %t151, { i64, i64 } %t150, 1
  %t153 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t152, 0
  %t154 = insertvalue { i64, i64 } poison, i64 2, 0
  %t155 = insertvalue { i64, i64 } %t154, i64 0, 1
  %t156 = insertvalue { i64, i64 } poison, i64 0, 0
  %t157 = insertvalue { i64, i64 } %t156, i64 0, 1
  %t158 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t155, 0
  %t159 = insertvalue [2 x { i64, i64 }] %t158, { i64, i64 } %t157, 1
  %t160 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t159, 0
  %t161 = insertvalue [2 x { [2 x { i64, i64 }] }] poison, { [2 x { i64, i64 }] } %t153, 0
  %t162 = insertvalue [2 x { [2 x { i64, i64 }] }] %t161, { [2 x { i64, i64 }] } %t160, 1
  %t163 = alloca [2 x { [2 x { i64, i64 }] }]
  store [2 x { [2 x { i64, i64 }] }] %t162, ptr %t163
  %t164 = getelementptr inbounds [2 x { [2 x { i64, i64 }] }], ptr %t163, i64 0, i32 1
  %t165 = load { [2 x { i64, i64 }] }, ptr %t164
  %t166 = extractvalue { [2 x { i64, i64 }] } %t165, 0
  %t167 = insertvalue { i64, i64 } poison, i64 0, 0
  %t168 = insertvalue { i64, i64 } %t167, i64 0, 1
  %t169 = insertvalue { i64, i64 } poison, i64 1, 0
  %t170 = insertvalue { i64, i64 } %t169, i64 0, 1
  %t171 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t168, 0
  %t172 = insertvalue [2 x { i64, i64 }] %t171, { i64, i64 } %t170, 1
  %t173 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t172, 0
  %t174 = insertvalue { i64, i64 } poison, i64 2, 0
  %t175 = insertvalue { i64, i64 } %t174, i64 0, 1
  %t176 = insertvalue { i64, i64 } poison, i64 0, 0
  %t177 = insertvalue { i64, i64 } %t176, i64 0, 1
  %t178 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t175, 0
  %t179 = insertvalue [2 x { i64, i64 }] %t178, { i64, i64 } %t177, 1
  %t180 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t179, 0
  %t181 = insertvalue [2 x { [2 x { i64, i64 }] }] poison, { [2 x { i64, i64 }] } %t173, 0
  %t182 = insertvalue [2 x { [2 x { i64, i64 }] }] %t181, { [2 x { i64, i64 }] } %t180, 1
  %t183 = alloca [2 x { [2 x { i64, i64 }] }]
  store [2 x { [2 x { i64, i64 }] }] %t182, ptr %t183
  %t184 = getelementptr inbounds [2 x { [2 x { i64, i64 }] }], ptr %t183, i64 0, i32 1
  %t185 = getelementptr inbounds { [2 x { i64, i64 }] }, ptr %t184, i64 0, i32 0
  %t186 = getelementptr inbounds [2 x { i64, i64 }], ptr %t185, i64 0, i32 0
  %t187 = load { i64, i64 }, ptr %t186
  %t188 = insertvalue { i64, i64 } poison, i64 2, 0
  %t189 = insertvalue { i64, i64 } %t188, i64 0, 1
  %t191 = extractvalue { i64, i64 } %t187, 0
  %t192 = extractvalue { i64, i64 } %t189, 0
  %t190 = icmp eq i64 %t191, %t192
  %t193 = call { ptr, i64 } @to_string$bool(i1 %t190)
  call void @string_builder_append_string({ ptr, i64 } %t193)
  %t194 = call { ptr, i64 } @string_builder_finish(i64 %t130)
  call void @$prn({ ptr, i64 } %t194)
  %t195 = call i64 @string_builder_mark()
  %t196 = insertvalue { i64, i64 } poison, i64 0, 0
  %t197 = insertvalue { i64, i64 } %t196, i64 0, 1
  %t198 = insertvalue { i64, i64 } poison, i64 1, 0
  %t199 = insertvalue { i64, i64 } %t198, i64 0, 1
  %t200 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t197, 0
  %t201 = insertvalue [2 x { i64, i64 }] %t200, { i64, i64 } %t199, 1
  %t202 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t201, 0
  %t203 = insertvalue { i64, i64 } poison, i64 2, 0
  %t204 = insertvalue { i64, i64 } %t203, i64 0, 1
  %t205 = insertvalue { i64, i64 } poison, i64 0, 0
  %t206 = insertvalue { i64, i64 } %t205, i64 0, 1
  %t207 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t204, 0
  %t208 = insertvalue [2 x { i64, i64 }] %t207, { i64, i64 } %t206, 1
  %t209 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t208, 0
  %t210 = insertvalue [2 x { [2 x { i64, i64 }] }] poison, { [2 x { i64, i64 }] } %t202, 0
  %t211 = insertvalue [2 x { [2 x { i64, i64 }] }] %t210, { [2 x { i64, i64 }] } %t209, 1
  %t212 = insertvalue { i64, i64 } poison, i64 0, 0
  %t213 = insertvalue { i64, i64 } %t212, i64 0, 1
  %t214 = insertvalue { i64, i64 } poison, i64 1, 0
  %t215 = insertvalue { i64, i64 } %t214, i64 0, 1
  %t216 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t213, 0
  %t217 = insertvalue [2 x { i64, i64 }] %t216, { i64, i64 } %t215, 1
  %t218 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t217, 0
  %t219 = insertvalue { i64, i64 } poison, i64 2, 0
  %t220 = insertvalue { i64, i64 } %t219, i64 0, 1
  %t221 = insertvalue { i64, i64 } poison, i64 0, 0
  %t222 = insertvalue { i64, i64 } %t221, i64 0, 1
  %t223 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t220, 0
  %t224 = insertvalue [2 x { i64, i64 }] %t223, { i64, i64 } %t222, 1
  %t225 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t224, 0
  %t226 = insertvalue [2 x { [2 x { i64, i64 }] }] poison, { [2 x { i64, i64 }] } %t218, 0
  %t227 = insertvalue [2 x { [2 x { i64, i64 }] }] %t226, { [2 x { i64, i64 }] } %t225, 1
  %t228 = alloca [2 x { [2 x { i64, i64 }] }]
  store [2 x { [2 x { i64, i64 }] }] %t227, ptr %t228
  %t229 = getelementptr inbounds [2 x { [2 x { i64, i64 }] }], ptr %t228, i64 0, i32 1
  %t230 = load { [2 x { i64, i64 }] }, ptr %t229
  %t231 = extractvalue { [2 x { i64, i64 }] } %t230, 0
  %t232 = insertvalue { i64, i64 } poison, i64 0, 0
  %t233 = insertvalue { i64, i64 } %t232, i64 0, 1
  %t234 = insertvalue { i64, i64 } poison, i64 1, 0
  %t235 = insertvalue { i64, i64 } %t234, i64 0, 1
  %t236 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t233, 0
  %t237 = insertvalue [2 x { i64, i64 }] %t236, { i64, i64 } %t235, 1
  %t238 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t237, 0
  %t239 = insertvalue { i64, i64 } poison, i64 2, 0
  %t240 = insertvalue { i64, i64 } %t239, i64 0, 1
  %t241 = insertvalue { i64, i64 } poison, i64 0, 0
  %t242 = insertvalue { i64, i64 } %t241, i64 0, 1
  %t243 = insertvalue [2 x { i64, i64 }] poison, { i64, i64 } %t240, 0
  %t244 = insertvalue [2 x { i64, i64 }] %t243, { i64, i64 } %t242, 1
  %t245 = insertvalue { [2 x { i64, i64 }] } poison, [2 x { i64, i64 }] %t244, 0
  %t246 = insertvalue [2 x { [2 x { i64, i64 }] }] poison, { [2 x { i64, i64 }] } %t238, 0
  %t247 = insertvalue [2 x { [2 x { i64, i64 }] }] %t246, { [2 x { i64, i64 }] } %t245, 1
  %t248 = alloca [2 x { [2 x { i64, i64 }] }]
  store [2 x { [2 x { i64, i64 }] }] %t247, ptr %t248
  %t249 = getelementptr inbounds [2 x { [2 x { i64, i64 }] }], ptr %t248, i64 0, i32 1
  %t250 = getelementptr inbounds { [2 x { i64, i64 }] }, ptr %t249, i64 0, i32 0
  %t251 = getelementptr inbounds [2 x { i64, i64 }], ptr %t250, i64 0, i32 1
  %t252 = load { i64, i64 }, ptr %t251
  %t253 = insertvalue { i64, i64 } poison, i64 0, 0
  %t254 = insertvalue { i64, i64 } %t253, i64 0, 1
  %t256 = extractvalue { i64, i64 } %t252, 0
  %t257 = extractvalue { i64, i64 } %t254, 0
  %t255 = icmp eq i64 %t256, %t257
  %t258 = call { ptr, i64 } @to_string$bool(i1 %t255)
  call void @string_builder_append_string({ ptr, i64 } %t258)
  %t259 = call { ptr, i64 } @string_builder_finish(i64 %t195)
  call void @$prn({ ptr, i64 } %t259)
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0