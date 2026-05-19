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

@.macro.file.m0 = private unnamed_addr constant [66 x i8] c"tests/language/119-slice-element-pointers.t\00"
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
  %t7 = extractvalue { ptr, i64 } %t6, 0
  %t8 = getelementptr inbounds i32, ptr %t7, i32 2
  store i32 32, ptr %t8
  %local.4 = alloca ptr
  store ptr null, ptr %local.4
  %t9 = load ptr, ptr %local.4
  %t10 = icmp eq ptr %t9, null
  br i1 %t10, label %dynarray.alloc.0, label %dynarray.ready.1
dynarray.alloc.0:
  %t11 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 13)
  %t12 = getelementptr inbounds { ptr, i64, i64 }, ptr %t11, i64 0, i32 0
  %t13 = getelementptr inbounds { ptr, i64, i64 }, ptr %t11, i64 0, i32 1
  %t14 = getelementptr inbounds { ptr, i64, i64 }, ptr %t11, i64 0, i32 2
  %t15 = getelementptr inbounds i8, ptr %t11, i64 24
  store ptr %t15, ptr %t12
  store i64 0, ptr %t13
  store i64 0, ptr %t14
  store ptr %t15, ptr %local.4
  br label %dynarray.ready.1
dynarray.ready.1:
  %t16 = load ptr, ptr %local.4
  %t17 = getelementptr inbounds i8, ptr %t16, i64 -24
  %t18 = getelementptr inbounds { ptr, i64, i64 }, ptr %t17, i64 0, i32 0
  %t19 = getelementptr inbounds { ptr, i64, i64 }, ptr %t17, i64 0, i32 1
  %t20 = getelementptr inbounds { ptr, i64, i64 }, ptr %t17, i64 0, i32 2
  %t21 = load ptr, ptr %t18
  %t22 = load i64, ptr %t19
  %t23 = load i64, ptr %t20
  %t24 = add i64 %t22, 1
  %t25 = icmp ugt i64 %t24, %t23
  br i1 %t25, label %dynarray.grow.2, label %dynarray.store.3
dynarray.grow.2:
  %t26 = icmp eq i64 %t23, 0
  %t27 = mul i64 %t23, 2
  %t28 = select i1 %t26, i64 1, i64 %t27
  %t29 = mul i64 %t28, 4
  %t30 = add i64 24, %t29
  %t31 = call ptr @nrt_mem_realloc(ptr %t17, i64 %t30, i64 16, ptr @.macro.file.m0, i32 13)
  %t32 = getelementptr inbounds i8, ptr %t31, i64 24
  %t33 = getelementptr inbounds { ptr, i64, i64 }, ptr %t31, i64 0, i32 0
  %t34 = getelementptr inbounds { ptr, i64, i64 }, ptr %t31, i64 0, i32 2
  store ptr %t32, ptr %t33
  store i64 %t28, ptr %t34
  store ptr %t32, ptr %local.4
  br label %dynarray.store.3
dynarray.store.3:
  %t35 = load ptr, ptr %local.4
  %t36 = getelementptr inbounds i8, ptr %t35, i64 -24
  %t37 = getelementptr inbounds { ptr, i64, i64 }, ptr %t36, i64 0, i32 0
  %t38 = getelementptr inbounds { ptr, i64, i64 }, ptr %t36, i64 0, i32 1
  %t39 = load ptr, ptr %t37
  %t40 = getelementptr inbounds i32, ptr %t39, i64 %t22
  store i32 5, ptr %t40
  store i64 %t24, ptr %t38
  %t41 = load ptr, ptr %local.4
  %t42 = icmp eq ptr %t41, null
  br i1 %t42, label %dynarray.alloc.4, label %dynarray.ready.5
dynarray.alloc.4:
  %t43 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 14)
  %t44 = getelementptr inbounds { ptr, i64, i64 }, ptr %t43, i64 0, i32 0
  %t45 = getelementptr inbounds { ptr, i64, i64 }, ptr %t43, i64 0, i32 1
  %t46 = getelementptr inbounds { ptr, i64, i64 }, ptr %t43, i64 0, i32 2
  %t47 = getelementptr inbounds i8, ptr %t43, i64 24
  store ptr %t47, ptr %t44
  store i64 0, ptr %t45
  store i64 0, ptr %t46
  store ptr %t47, ptr %local.4
  br label %dynarray.ready.5
dynarray.ready.5:
  %t48 = load ptr, ptr %local.4
  %t49 = getelementptr inbounds i8, ptr %t48, i64 -24
  %t50 = getelementptr inbounds { ptr, i64, i64 }, ptr %t49, i64 0, i32 0
  %t51 = getelementptr inbounds { ptr, i64, i64 }, ptr %t49, i64 0, i32 1
  %t52 = getelementptr inbounds { ptr, i64, i64 }, ptr %t49, i64 0, i32 2
  %t53 = load ptr, ptr %t50
  %t54 = load i64, ptr %t51
  %t55 = load i64, ptr %t52
  %t56 = add i64 %t54, 1
  %t57 = icmp ugt i64 %t56, %t55
  br i1 %t57, label %dynarray.grow.6, label %dynarray.store.7
dynarray.grow.6:
  %t58 = icmp eq i64 %t55, 0
  %t59 = mul i64 %t55, 2
  %t60 = select i1 %t58, i64 1, i64 %t59
  %t61 = mul i64 %t60, 4
  %t62 = add i64 24, %t61
  %t63 = call ptr @nrt_mem_realloc(ptr %t49, i64 %t62, i64 16, ptr @.macro.file.m0, i32 14)
  %t64 = getelementptr inbounds i8, ptr %t63, i64 24
  %t65 = getelementptr inbounds { ptr, i64, i64 }, ptr %t63, i64 0, i32 0
  %t66 = getelementptr inbounds { ptr, i64, i64 }, ptr %t63, i64 0, i32 2
  store ptr %t64, ptr %t65
  store i64 %t60, ptr %t66
  store ptr %t64, ptr %local.4
  br label %dynarray.store.7
dynarray.store.7:
  %t67 = load ptr, ptr %local.4
  %t68 = getelementptr inbounds i8, ptr %t67, i64 -24
  %t69 = getelementptr inbounds { ptr, i64, i64 }, ptr %t68, i64 0, i32 0
  %t70 = getelementptr inbounds { ptr, i64, i64 }, ptr %t68, i64 0, i32 1
  %t71 = load ptr, ptr %t69
  %t72 = getelementptr inbounds i32, ptr %t71, i64 %t54
  store i32 6, ptr %t72
  store i64 %t56, ptr %t70
  %t73 = load ptr, ptr %local.4
  %t74 = getelementptr inbounds i8, ptr %t73, i64 -24
  %t75 = getelementptr inbounds { ptr, i64, i64 }, ptr %t74, i64 0, i32 0
  %t76 = load ptr, ptr %t75
  %t77 = getelementptr inbounds i32, ptr %t76, i32 1
  store i32 7, ptr %t77
  %t78 = extractvalue { ptr, i64 } { ptr @.str.m0.0, i64 3 }, 0
  %t79 = getelementptr inbounds i8, ptr %t78, i32 1
  %t80 = call i64 @string_builder_mark()
  %t81 = load [3 x i32], ptr %local.0
  %t82 = extractvalue [3 x i32] %t81, 1
  %t83 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t83, i32 %t82)
  call void @string_builder_append_string(ptr %t83)
  %t84 = alloca { ptr, i64 }
  %t85 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 1 }, ptr %t85
  call void @to_string$string(ptr %t84, ptr %t85)
  call void @string_builder_append_string(ptr %t84)
  %t86 = extractvalue { ptr, i64 } %t6, 0
  %t87 = getelementptr inbounds i32, ptr %t86, i32 2
  %t88 = load i32, ptr %t87
  %t89 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t89, i32 %t88)
  call void @string_builder_append_string(ptr %t89)
  %t90 = alloca { ptr, i64 }
  %t91 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 1 }, ptr %t91
  call void @to_string$string(ptr %t90, ptr %t91)
  call void @string_builder_append_string(ptr %t90)
  %t92 = load ptr, ptr %local.4
  %t93 = getelementptr inbounds i8, ptr %t92, i64 -24
  %t94 = getelementptr inbounds { ptr, i64, i64 }, ptr %t93, i64 0, i32 0
  %t95 = load ptr, ptr %t94
  %t96 = getelementptr inbounds i32, ptr %t95, i32 1
  %t97 = load i32, ptr %t96
  %t98 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t98, i32 %t97)
  call void @string_builder_append_string(ptr %t98)
  %t99 = alloca { ptr, i64 }
  %t100 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 1 }, ptr %t100
  call void @to_string$string(ptr %t99, ptr %t100)
  call void @string_builder_append_string(ptr %t99)
  %t101 = load i8, ptr %t79
  %t102 = alloca { ptr, i64 }
  call void @to_string$u8(ptr %t102, i8 %t101)
  call void @string_builder_append_string(ptr %t102)
  %t103 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t103, i64 %t80)
  %t104 = load { ptr, i64 }, ptr %t103
  call void @$prn({ ptr, i64 } %t104)
  %t105 = load [3 x i32], ptr %local.0
  %t106 = extractvalue [3 x i32] %t105, 1
  %t107 = extractvalue { ptr, i64 } %t6, 0
  %t108 = getelementptr inbounds i32, ptr %t107, i32 2
  %t109 = load i32, ptr %t108
  %t110 = add i32 %t106, %t109
  %t111 = load ptr, ptr %local.4
  %t112 = getelementptr inbounds i8, ptr %t111, i64 -24
  %t113 = getelementptr inbounds { ptr, i64, i64 }, ptr %t112, i64 0, i32 0
  %t114 = load ptr, ptr %t113
  %t115 = getelementptr inbounds i32, ptr %t114, i32 1
  %t116 = load i32, ptr %t115
  %t117 = add i32 %t110, %t116
  %t118 = load i8, ptr %t79
  %t119 = zext i8 %t118 to i32
  %t120 = add i32 %t117, %t119
  %t121 = load ptr, ptr %local.4
  %t122 = icmp eq ptr %t121, null
  br i1 %t122, label %dynarray.free.done.9, label %dynarray.free.8
dynarray.free.8:
  %t123 = getelementptr inbounds i8, ptr %t121, i64 -24
  call void @nrt_mem_free(ptr %t123)
  store ptr null, ptr %local.4
  br label %dynarray.free.done.9
dynarray.free.done.9:
  ret i32 %t120
}

@$main = alias i32 (), ptr @fn.0
