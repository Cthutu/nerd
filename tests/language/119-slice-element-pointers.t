use std.io

main :: fn () -> i32 {
    fixed: [3]i32 = [10, 20, 30]
    fixed_ptr: ^i32 = ^fixed[1]
    fixed_ptr^ = 21

    slice: []i32 = fixed[..]
    slice_ptr: ^i32 = ^slice[2]
    slice_ptr^ = 32

    dyn: [..]i32
    dyn.push(5)
    dyn.push(6)
    dyn_ptr: ^i32 = ^dyn[1]
    dyn_ptr^ = 7

    text := "abc"
    char_ptr: ^u8 = ^text[1]

    prn($"{fixed[1]} {slice[2]} {dyn[1]} {char_ptr^}")

    result := fixed[1] + slice[2] + dyn[1] + char_ptr^.as(i32)
    dyn.free()
    return result
}
¬
158
¬
21 32 7 98

¬
hir 0
module module.0(119-slice-element-pointers.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind main = fn.0
func fn.0() -> i32 {
  let fixed: [3]i32 = [3]i32 array(i32 10, i32 20, i32 30)
  let fixed_ptr: ^i32 = ^i32 address_of(i32 index([3]i32 local.0(fixed), untyped integer 1))
  assign i32 deref(^i32 local.1(fixed_ptr)) = i32 21
  let slice: []i32 = []i32 slice([3]i32 local.0(fixed), <none>, <none>)
  let slice_ptr: ^i32 = ^i32 address_of(i32 index([]i32 local.2(slice), untyped integer 2))
  assign i32 deref(^i32 local.3(slice_ptr)) = i32 32
  expr <unknown> default
  let dyn: [..]i32 = <unknown> default
  expr void call fn (i32) -> void field([..]i32 local.4(dyn), push)(i32 5)
  expr void call fn (i32) -> void field([..]i32 local.4(dyn), push)(i32 6)
  let dyn_ptr: ^i32 = ^i32 address_of(i32 index([..]i32 local.4(dyn), untyped integer 1))
  assign i32 deref(^i32 local.5(dyn_ptr)) = i32 7
  let text: string = string "abc"
  let char_ptr: ^u8 = ^u8 address_of(u8 index(string local.6(text), untyped integer 1))
  expr void call bind.0(prn)(string interpolate(i32 index([3]i32 local.0(fixed), untyped integer 1), <unknown> " ", i32 index([]i32 local.2(slice), untyped integer 2), <unknown> " ", i32 index([..]i32 local.4(dyn), untyped integer 1), <unknown> " ", u8 deref(^u8 local.7(char_ptr))))
  let result: i32 = i32 add(i32 add(i32 add(i32 index([3]i32 local.0(fixed), untyped integer 1), i32 index([]i32 local.2(slice), untyped integer 2)), i32 index([..]i32 local.4(dyn), untyped integer 1)), i32 cast(u8 deref(^u8 local.7(char_ptr)) as i32))
  expr void call fn () -> void field([..]i32 local.4(dyn), free)()
  return i32 local.8(result)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [44 x i8] c"tests/language/119-slice-element-pointers.t\00"
@.str.m0.0 = private unnamed_addr constant [4 x i8] c"abc\00"
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
declare ptr @nrt_mem_alloc(i64, i64, ptr, i32)
declare ptr @nrt_mem_realloc(ptr, i64, i64, ptr, i32)
declare void @nrt_mem_free(ptr)

declare void @$prn({ ptr, i64 })
declare ptr @$input({ ptr, i64 })

define internal i32 @fn.0() {
  %t0 = insertvalue [3 x i32] poison, i32 10, 0
  %t1 = insertvalue [3 x i32] %t0, i32 20, 1
  %t2 = insertvalue [3 x i32] %t1, i32 30, 2
  %local.0 = alloca [3 x i32]
  store [3 x i32] %t2, ptr %local.0
  %t3 = getelementptr inbounds [3 x i32], ptr %local.0, i64 0, i32 1
  store i32 21, ptr %t3
  %t4 = getelementptr inbounds [3 x i32], ptr %local.0, i64 0, i64 0
  %t5 = insertvalue { ptr, i64 } poison, ptr %t4, 0
  %t6 = insertvalue { ptr, i64 } %t5, i64 3, 1
  %local.2 = alloca { ptr, i64 }
  store { ptr, i64 } %t6, ptr %local.2
  %t7 = load { ptr, i64 }, ptr %local.2
  %t8 = extractvalue { ptr, i64 } %t7, 0
  %t9 = getelementptr inbounds i32, ptr %t8, i32 2
  store i32 32, ptr %t9
  %local.4 = alloca ptr
  store ptr null, ptr %local.4
  %t10 = load ptr, ptr %local.4
  %t11 = icmp eq ptr %t10, null
  br i1 %t11, label %dynarray.alloc.0, label %dynarray.ready.1
dynarray.alloc.0:
  %t12 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 13)
  %t13 = getelementptr inbounds { ptr, i64, i64 }, ptr %t12, i64 0, i32 0
  %t14 = getelementptr inbounds { ptr, i64, i64 }, ptr %t12, i64 0, i32 1
  %t15 = getelementptr inbounds { ptr, i64, i64 }, ptr %t12, i64 0, i32 2
  %t16 = getelementptr inbounds i8, ptr %t12, i64 24
  store ptr %t16, ptr %t13
  store i64 0, ptr %t14
  store i64 0, ptr %t15
  store ptr %t16, ptr %local.4
  br label %dynarray.ready.1
dynarray.ready.1:
  %t17 = load ptr, ptr %local.4
  %t18 = getelementptr inbounds i8, ptr %t17, i64 -24
  %t19 = getelementptr inbounds { ptr, i64, i64 }, ptr %t18, i64 0, i32 0
  %t20 = getelementptr inbounds { ptr, i64, i64 }, ptr %t18, i64 0, i32 1
  %t21 = getelementptr inbounds { ptr, i64, i64 }, ptr %t18, i64 0, i32 2
  %t22 = load ptr, ptr %t19
  %t23 = load i64, ptr %t20
  %t24 = load i64, ptr %t21
  %t25 = add i64 %t23, 1
  %t26 = icmp ugt i64 %t25, %t24
  br i1 %t26, label %dynarray.grow.2, label %dynarray.store.3
dynarray.grow.2:
  %t27 = icmp eq i64 %t24, 0
  %t28 = mul i64 %t24, 2
  %t29 = select i1 %t27, i64 1, i64 %t28
  %t30 = mul i64 %t29, 4
  %t31 = add i64 24, %t30
  %t32 = call ptr @nrt_mem_realloc(ptr %t18, i64 %t31, i64 16, ptr @.macro.file.m0, i32 13)
  %t33 = getelementptr inbounds i8, ptr %t32, i64 24
  %t34 = getelementptr inbounds { ptr, i64, i64 }, ptr %t32, i64 0, i32 0
  %t35 = getelementptr inbounds { ptr, i64, i64 }, ptr %t32, i64 0, i32 2
  store ptr %t33, ptr %t34
  store i64 %t29, ptr %t35
  store ptr %t33, ptr %local.4
  br label %dynarray.store.3
dynarray.store.3:
  %t36 = load ptr, ptr %local.4
  %t37 = getelementptr inbounds i8, ptr %t36, i64 -24
  %t38 = getelementptr inbounds { ptr, i64, i64 }, ptr %t37, i64 0, i32 0
  %t39 = getelementptr inbounds { ptr, i64, i64 }, ptr %t37, i64 0, i32 1
  %t40 = load ptr, ptr %t38
  %t41 = getelementptr inbounds i32, ptr %t40, i64 %t23
  store i32 5, ptr %t41
  store i64 %t25, ptr %t39
  %t42 = load ptr, ptr %local.4
  %t43 = icmp eq ptr %t42, null
  br i1 %t43, label %dynarray.alloc.4, label %dynarray.ready.5
dynarray.alloc.4:
  %t44 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 14)
  %t45 = getelementptr inbounds { ptr, i64, i64 }, ptr %t44, i64 0, i32 0
  %t46 = getelementptr inbounds { ptr, i64, i64 }, ptr %t44, i64 0, i32 1
  %t47 = getelementptr inbounds { ptr, i64, i64 }, ptr %t44, i64 0, i32 2
  %t48 = getelementptr inbounds i8, ptr %t44, i64 24
  store ptr %t48, ptr %t45
  store i64 0, ptr %t46
  store i64 0, ptr %t47
  store ptr %t48, ptr %local.4
  br label %dynarray.ready.5
dynarray.ready.5:
  %t49 = load ptr, ptr %local.4
  %t50 = getelementptr inbounds i8, ptr %t49, i64 -24
  %t51 = getelementptr inbounds { ptr, i64, i64 }, ptr %t50, i64 0, i32 0
  %t52 = getelementptr inbounds { ptr, i64, i64 }, ptr %t50, i64 0, i32 1
  %t53 = getelementptr inbounds { ptr, i64, i64 }, ptr %t50, i64 0, i32 2
  %t54 = load ptr, ptr %t51
  %t55 = load i64, ptr %t52
  %t56 = load i64, ptr %t53
  %t57 = add i64 %t55, 1
  %t58 = icmp ugt i64 %t57, %t56
  br i1 %t58, label %dynarray.grow.6, label %dynarray.store.7
dynarray.grow.6:
  %t59 = icmp eq i64 %t56, 0
  %t60 = mul i64 %t56, 2
  %t61 = select i1 %t59, i64 1, i64 %t60
  %t62 = mul i64 %t61, 4
  %t63 = add i64 24, %t62
  %t64 = call ptr @nrt_mem_realloc(ptr %t50, i64 %t63, i64 16, ptr @.macro.file.m0, i32 14)
  %t65 = getelementptr inbounds i8, ptr %t64, i64 24
  %t66 = getelementptr inbounds { ptr, i64, i64 }, ptr %t64, i64 0, i32 0
  %t67 = getelementptr inbounds { ptr, i64, i64 }, ptr %t64, i64 0, i32 2
  store ptr %t65, ptr %t66
  store i64 %t61, ptr %t67
  store ptr %t65, ptr %local.4
  br label %dynarray.store.7
dynarray.store.7:
  %t68 = load ptr, ptr %local.4
  %t69 = getelementptr inbounds i8, ptr %t68, i64 -24
  %t70 = getelementptr inbounds { ptr, i64, i64 }, ptr %t69, i64 0, i32 0
  %t71 = getelementptr inbounds { ptr, i64, i64 }, ptr %t69, i64 0, i32 1
  %t72 = load ptr, ptr %t70
  %t73 = getelementptr inbounds i32, ptr %t72, i64 %t55
  store i32 6, ptr %t73
  store i64 %t57, ptr %t71
  %t74 = load ptr, ptr %local.4
  %t75 = getelementptr inbounds i8, ptr %t74, i64 -24
  %t76 = getelementptr inbounds { ptr, i64, i64 }, ptr %t75, i64 0, i32 0
  %t77 = load ptr, ptr %t76
  %t78 = getelementptr inbounds i32, ptr %t77, i32 1
  store i32 7, ptr %t78
  %local.6 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 3 }, ptr %local.6
  %t79 = load { ptr, i64 }, ptr %local.6
  %t80 = extractvalue { ptr, i64 } %t79, 0
  %t81 = getelementptr inbounds i8, ptr %t80, i32 1
  %t82 = call i64 @string_builder_mark()
  %t83 = load [3 x i32], ptr %local.0
  %t84 = extractvalue [3 x i32] %t83, 1
  %t85 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t85, i32 %t84)
  call void @string_builder_append_string(ptr %t85)
  %t86 = alloca { ptr, i64 }
  %t87 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 1 }, ptr %t87
  call void @to_string$string(ptr %t86, ptr %t87)
  call void @string_builder_append_string(ptr %t86)
  %t88 = load { ptr, i64 }, ptr %local.2
  %t89 = extractvalue { ptr, i64 } %t88, 0
  %t90 = getelementptr inbounds i32, ptr %t89, i32 2
  %t91 = load i32, ptr %t90
  %t92 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t92, i32 %t91)
  call void @string_builder_append_string(ptr %t92)
  %t93 = alloca { ptr, i64 }
  %t94 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 1 }, ptr %t94
  call void @to_string$string(ptr %t93, ptr %t94)
  call void @string_builder_append_string(ptr %t93)
  %t95 = load ptr, ptr %local.4
  %t96 = getelementptr inbounds i8, ptr %t95, i64 -24
  %t97 = getelementptr inbounds { ptr, i64, i64 }, ptr %t96, i64 0, i32 0
  %t98 = load ptr, ptr %t97
  %t99 = getelementptr inbounds i32, ptr %t98, i32 1
  %t100 = load i32, ptr %t99
  %t101 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t101, i32 %t100)
  call void @string_builder_append_string(ptr %t101)
  %t102 = alloca { ptr, i64 }
  %t103 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 1 }, ptr %t103
  call void @to_string$string(ptr %t102, ptr %t103)
  call void @string_builder_append_string(ptr %t102)
  %t104 = load i8, ptr %t81
  %t105 = alloca { ptr, i64 }
  call void @to_string$u8(ptr %t105, i8 %t104)
  call void @string_builder_append_string(ptr %t105)
  %t106 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t106, i64 %t82)
  %t107 = load { ptr, i64 }, ptr %t106
  call void @$prn({ ptr, i64 } %t107)
  %t108 = load [3 x i32], ptr %local.0
  %t109 = extractvalue [3 x i32] %t108, 1
  %t110 = load { ptr, i64 }, ptr %local.2
  %t111 = extractvalue { ptr, i64 } %t110, 0
  %t112 = getelementptr inbounds i32, ptr %t111, i32 2
  %t113 = load i32, ptr %t112
  %t114 = add i32 %t109, %t113
  %t115 = load ptr, ptr %local.4
  %t116 = getelementptr inbounds i8, ptr %t115, i64 -24
  %t117 = getelementptr inbounds { ptr, i64, i64 }, ptr %t116, i64 0, i32 0
  %t118 = load ptr, ptr %t117
  %t119 = getelementptr inbounds i32, ptr %t118, i32 1
  %t120 = load i32, ptr %t119
  %t121 = add i32 %t114, %t120
  %t122 = load i8, ptr %t81
  %t123 = zext i8 %t122 to i32
  %t124 = add i32 %t121, %t123
  %t125 = load ptr, ptr %local.4
  %t126 = icmp eq ptr %t125, null
  br i1 %t126, label %dynarray.free.done.9, label %dynarray.free.8
dynarray.free.8:
  %t127 = getelementptr inbounds i8, ptr %t125, i64 -24
  call void @nrt_mem_free(ptr %t127)
  store ptr null, ptr %local.4
  br label %dynarray.free.done.9
dynarray.free.done.9:
  ret i32 %t124
}

@$main = alias i32 (), ptr @fn.0

