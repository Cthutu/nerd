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
bind main = fn.0
func fn.0() -> i32 {
  let fixed: [3]i32 = [3]i32 array(i32 10, i32 20, i32 30)
  let fixed_ptr: ^i32 = ^i32 address_of(i32 index([3]i32 local.0(fixed), untyped integer 1))
  assign i32 deref(^i32 local.1(fixed_ptr)) = i32 21
  let slice: []i32 = []i32 slice([3]i32 local.0(fixed), <none>, <none>)
  let slice_ptr: ^i32 = ^i32 address_of(i32 index([]i32 local.2(slice), untyped integer 2))
  assign i32 deref(^i32 local.3(slice_ptr)) = i32 32
  expr <unknown> <unsupported>
  let dyn: [..]i32 = <unknown> <unsupported>
  expr void call fn (i32) -> void field([..]i32 local.4(dyn), push)(i32 5)
  expr void call fn (i32) -> void field([..]i32 local.4(dyn), push)(i32 6)
  let dyn_ptr: ^i32 = ^i32 address_of(i32 index([..]i32 local.4(dyn), untyped integer 1))
  assign i32 deref(^i32 local.5(dyn_ptr)) = i32 7
  let text: string = string "abc"
  let char_ptr: ^u8 = ^u8 address_of(u8 index(string local.6(text), untyped integer 1))
  expr void call bind.2(prn)(string interpolate(i32 index([3]i32 local.0(fixed), untyped integer 1), <unknown> " ", i32 index([]i32 local.2(slice), untyped integer 2), <unknown> " ", i32 index([..]i32 local.4(dyn), untyped integer 1), <unknown> " ", u8 deref(^u8 local.7(char_ptr))))
  let result: i32 = i32 add(i32 add(i32 add(i32 index([3]i32 local.0(fixed), untyped integer 1), i32 index([]i32 local.2(slice), untyped integer 2)), i32 index([..]i32 local.4(dyn), untyped integer 1)), i32 cast(u8 deref(^u8 local.7(char_ptr)) as i32))
  expr void call fn () -> void field([..]i32 local.4(dyn), free)()
  return i32 local.8(result)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [4 x i8] c"abc\00"
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
declare ptr @malloc(i64)
declare ptr @realloc(ptr, i64)
declare void @free(ptr)

declare void @$pr({ ptr, i64 })
declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

define i32 @fn.0() {
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
  %t11 = call ptr @malloc(i64 24)
  %t12 = getelementptr inbounds { ptr, i64, i64 }, ptr %t11, i64 0, i32 0
  %t13 = getelementptr inbounds { ptr, i64, i64 }, ptr %t11, i64 0, i32 1
  %t14 = getelementptr inbounds { ptr, i64, i64 }, ptr %t11, i64 0, i32 2
  store ptr null, ptr %t12
  store i64 0, ptr %t13
  store i64 0, ptr %t14
  store ptr %t11, ptr %local.4
  br label %dynarray.ready.1
dynarray.ready.1:
  %t15 = load ptr, ptr %local.4
  %t16 = getelementptr inbounds { ptr, i64, i64 }, ptr %t15, i64 0, i32 0
  %t17 = getelementptr inbounds { ptr, i64, i64 }, ptr %t15, i64 0, i32 1
  %t18 = getelementptr inbounds { ptr, i64, i64 }, ptr %t15, i64 0, i32 2
  %t19 = load ptr, ptr %t16
  %t20 = load i64, ptr %t17
  %t21 = load i64, ptr %t18
  %t22 = add i64 %t20, 1
  %t23 = icmp ugt i64 %t22, %t21
  br i1 %t23, label %dynarray.grow.2, label %dynarray.store.3
dynarray.grow.2:
  %t24 = icmp eq i64 %t21, 0
  %t25 = mul i64 %t21, 2
  %t26 = select i1 %t24, i64 1, i64 %t25
  %t27 = mul i64 %t26, 4
  %t28 = call ptr @realloc(ptr %t19, i64 %t27)
  store ptr %t28, ptr %t16
  store i64 %t26, ptr %t18
  br label %dynarray.store.3
dynarray.store.3:
  %t29 = load ptr, ptr %t16
  %t30 = getelementptr inbounds i32, ptr %t29, i64 %t20
  store i32 5, ptr %t30
  store i64 %t22, ptr %t17
  %t31 = load ptr, ptr %local.4
  %t32 = icmp eq ptr %t31, null
  br i1 %t32, label %dynarray.alloc.4, label %dynarray.ready.5
dynarray.alloc.4:
  %t33 = call ptr @malloc(i64 24)
  %t34 = getelementptr inbounds { ptr, i64, i64 }, ptr %t33, i64 0, i32 0
  %t35 = getelementptr inbounds { ptr, i64, i64 }, ptr %t33, i64 0, i32 1
  %t36 = getelementptr inbounds { ptr, i64, i64 }, ptr %t33, i64 0, i32 2
  store ptr null, ptr %t34
  store i64 0, ptr %t35
  store i64 0, ptr %t36
  store ptr %t33, ptr %local.4
  br label %dynarray.ready.5
dynarray.ready.5:
  %t37 = load ptr, ptr %local.4
  %t38 = getelementptr inbounds { ptr, i64, i64 }, ptr %t37, i64 0, i32 0
  %t39 = getelementptr inbounds { ptr, i64, i64 }, ptr %t37, i64 0, i32 1
  %t40 = getelementptr inbounds { ptr, i64, i64 }, ptr %t37, i64 0, i32 2
  %t41 = load ptr, ptr %t38
  %t42 = load i64, ptr %t39
  %t43 = load i64, ptr %t40
  %t44 = add i64 %t42, 1
  %t45 = icmp ugt i64 %t44, %t43
  br i1 %t45, label %dynarray.grow.6, label %dynarray.store.7
dynarray.grow.6:
  %t46 = icmp eq i64 %t43, 0
  %t47 = mul i64 %t43, 2
  %t48 = select i1 %t46, i64 1, i64 %t47
  %t49 = mul i64 %t48, 4
  %t50 = call ptr @realloc(ptr %t41, i64 %t49)
  store ptr %t50, ptr %t38
  store i64 %t48, ptr %t40
  br label %dynarray.store.7
dynarray.store.7:
  %t51 = load ptr, ptr %t38
  %t52 = getelementptr inbounds i32, ptr %t51, i64 %t42
  store i32 6, ptr %t52
  store i64 %t44, ptr %t39
  %t53 = load ptr, ptr %local.4
  %t54 = getelementptr inbounds { ptr, i64, i64 }, ptr %t53, i64 0, i32 0
  %t55 = load ptr, ptr %t54
  %t56 = getelementptr inbounds i32, ptr %t55, i32 1
  store i32 7, ptr %t56
  %t57 = extractvalue { ptr, i64 } { ptr @.str.m0.0, i64 3 }, 0
  %t58 = getelementptr inbounds i8, ptr %t57, i32 1
  %t59 = call i64 @string_builder_mark()
  %t60 = load [3 x i32], ptr %local.0
  %t61 = extractvalue [3 x i32] %t60, 1
  %t62 = call { ptr, i64 } @to_string$i32(i32 %t61)
  call void @string_builder_append_string({ ptr, i64 } %t62)
  %t63 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t63)
  %t64 = extractvalue { ptr, i64 } %t6, 0
  %t65 = getelementptr inbounds i32, ptr %t64, i32 2
  %t66 = load i32, ptr %t65
  %t67 = call { ptr, i64 } @to_string$i32(i32 %t66)
  call void @string_builder_append_string({ ptr, i64 } %t67)
  %t68 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t68)
  %t69 = load ptr, ptr %local.4
  %t70 = getelementptr inbounds { ptr, i64, i64 }, ptr %t69, i64 0, i32 0
  %t71 = load ptr, ptr %t70
  %t72 = getelementptr inbounds i32, ptr %t71, i32 1
  %t73 = load i32, ptr %t72
  %t74 = call { ptr, i64 } @to_string$i32(i32 %t73)
  call void @string_builder_append_string({ ptr, i64 } %t74)
  %t75 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.3, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t75)
  %t76 = load i8, ptr %t58
  %t77 = call { ptr, i64 } @to_string$u8(i8 %t76)
  call void @string_builder_append_string({ ptr, i64 } %t77)
  %t78 = call { ptr, i64 } @string_builder_finish(i64 %t59)
  call void @$prn({ ptr, i64 } %t78)
  %t79 = load [3 x i32], ptr %local.0
  %t80 = extractvalue [3 x i32] %t79, 1
  %t81 = extractvalue { ptr, i64 } %t6, 0
  %t82 = getelementptr inbounds i32, ptr %t81, i32 2
  %t83 = load i32, ptr %t82
  %t84 = add i32 %t80, %t83
  %t85 = load ptr, ptr %local.4
  %t86 = getelementptr inbounds { ptr, i64, i64 }, ptr %t85, i64 0, i32 0
  %t87 = load ptr, ptr %t86
  %t88 = getelementptr inbounds i32, ptr %t87, i32 1
  %t89 = load i32, ptr %t88
  %t90 = add i32 %t84, %t89
  %t91 = load i8, ptr %t58
  %t92 = zext i8 %t91 to i32
  %t93 = add i32 %t90, %t92
  %t94 = load ptr, ptr %local.4
  %t95 = icmp eq ptr %t94, null
  br i1 %t95, label %dynarray.free.done.9, label %dynarray.free.8
dynarray.free.8:
  %t96 = getelementptr inbounds { ptr, i64, i64 }, ptr %t94, i64 0, i32 0
  %t97 = load ptr, ptr %t96
  call void @free(ptr %t97)
  call void @free(ptr %t94)
  store ptr null, ptr %local.4
  br label %dynarray.free.done.9
dynarray.free.done.9:
  ret i32 %t93
}

@$main = alias i32 (), ptr @fn.0