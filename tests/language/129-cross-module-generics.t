use std.io
use test.generics

main :: fn () {
    stack: Stack[i32]
    stack_push(^stack, 42)
    stack_push(^stack, 13)

    last := stack_pop(^stack)
    first := stack_pop(^stack)
    box := make_box("ok")

    prn($"{last} {first} {box.value}")
}
¬
0
¬
13 42 ok

¬
hir 0
module module.0(129-cross-module-generics.input)
import module.1(std.io)
import module.2(test.generics)
import import.0 pr from module.1(std.io).decl.9: fn (string) -> void
import import.1 epr from module.1(std.io).decl.10: fn (string) -> void
import import.2 prn from module.1(std.io).decl.11: fn (string) -> void
import import.3 eprn from module.1(std.io).decl.12: fn (string) -> void
import import.4 input from module.1(std.io).decl.13: fn (string) -> string
import import.5 Stack from module.2(test.generics).decl.0: <unknown>
import import.6 stack_push from module.2(test.generics).decl.1: <unknown>
import import.7 stack_pop from module.2(test.generics).decl.2: <unknown>
import import.8 Box from module.2(test.generics).decl.3: <unknown>
import import.9 make_box from module.2(test.generics).decl.4: <unknown>
bind pr = import.0
bind epr = import.1
bind prn = import.2
bind eprn = import.3
bind input = import.4
bind Stack = import.5
bind stack_push = import.6
bind stack_pop = import.7
bind Box = import.8
bind make_box = import.9
bind main = fn.0
bind Stack = type.0
bind Box = type.1
generic type type.0 = <unknown>
generic type type.1 = <unknown>
func fn.0() -> void {
  expr <unknown> default
  let stack: plex { [..]i32[..]i32 = <unknown> default
  expr void call bind.6(stack_push)(^plex { [..]i32[..]i32 address_of(plex { [..]i32[..]i32 local.0(stack)), untyped integer 42)
  expr void call bind.6(stack_push)(^plex { [..]i32[..]i32 address_of(plex { [..]i32[..]i32 local.0(stack)), untyped integer 13)
  let last: i32 = i32 call bind.7(stack_pop)(^plex { [..]i32[..]i32 address_of(plex { [..]i32[..]i32 local.0(stack)))
  let first: i32 = i32 call bind.7(stack_pop)(^plex { [..]i32[..]i32 address_of(plex { [..]i32[..]i32 local.0(stack)))
  let box: plex { string value } = plex { string value } call bind.9(make_box)(string "ok")
  expr void call bind.2(prn)(string interpolate(i32 local.1(last), <unknown> " ", i32 local.2(first), <unknown> " ", string field(plex { string value } local.3(box), value)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [3 x i8] c"ok\00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.2 = private unnamed_addr constant [2 x i8] c" \00"

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
declare void @m2.fn.0(ptr %stack, i32 %elem)
declare i32 @m2.fn.1(ptr %stack)
declare { { ptr, i64 } } @m2.fn.2({ ptr, i64 } %value)

define internal void @fn.0() {
  %local.0 = alloca { ptr }
  store { ptr } zeroinitializer, ptr %local.0
  call void @m2.fn.0(ptr %local.0, i32 42)
  call void @m2.fn.0(ptr %local.0, i32 13)
  %t0 = call i32 @m2.fn.1(ptr %local.0)
  %t1 = call i32 @m2.fn.1(ptr %local.0)
  %t2 = call { { ptr, i64 } } @m2.fn.2({ ptr, i64 } { ptr @.str.m0.0, i64 2 })
  %t3 = call i64 @string_builder_mark()
  %t4 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t4, i32 %t0)
  call void @string_builder_append_string(ptr %t4)
  %t5 = alloca { ptr, i64 }
  %t6 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 1 }, ptr %t6
  call void @to_string$string(ptr %t5, ptr %t6)
  call void @string_builder_append_string(ptr %t5)
  %t7 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t7, i32 %t1)
  call void @string_builder_append_string(ptr %t7)
  %t8 = alloca { ptr, i64 }
  %t9 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 1 }, ptr %t9
  call void @to_string$string(ptr %t8, ptr %t9)
  call void @string_builder_append_string(ptr %t8)
  %t10 = extractvalue { { ptr, i64 } } %t2, 0
  %t11 = alloca { ptr, i64 }
  %t12 = alloca { ptr, i64 }
  store { ptr, i64 } %t10, ptr %t12
  call void @to_string$string(ptr %t11, ptr %t12)
  call void @string_builder_append_string(ptr %t11)
  %t13 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t13, i64 %t3)
  %t14 = load { ptr, i64 }, ptr %t13
  call void @$prn({ ptr, i64 } %t14)
  ret void
}

@$main = alias void (), ptr @fn.0
