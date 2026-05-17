io :: use std.io
use std.collections

Counter :: plex {
    value i32
}

impl Counter {
    inc :: fn (self: ^Self, amount: i32) {
        self.value += amount
    }

    get :: fn (self: Self) -> i32 {
        return self.value
    }
}

main :: fn () {
    counter: Counter
    counter.inc(7)
    counter.inc(5)

    stack: Stack[i32]
    stack.push(counter.get())
    stack.push(30)

    io.prn($"{stack.pop()} {stack.pop()} {counter.get()}")
}
¬
0
¬
30 12 12

¬
hir 0
module module.0(130-inherent-impl-methods.input)
import module.1(std.io)
import module.2(std.collections)
import import.0 Stack from module.2(std.collections).decl.0: <unknown>
import import.1 __impl_7_push from module.2(std.collections).decl.1: <unknown>
import import.2 __impl_7_pop from module.2(std.collections).decl.2: <unknown>
import import.3 prn from module.1(std.io).decl.11: fn (string) -> void
bind Stack = import.0
bind __impl_7_push = import.1
bind __impl_7_pop = import.2
bind prn = import.3
bind io = module.1
bind Counter = type.0
bind __impl_8_inc = fn.0
bind __impl_8_get = fn.1
bind main = fn.2
bind Stack = type.1
type type.0 = Counter
generic type type.1 = <unknown>
func fn.0(self: ^Counter, amount: i32) -> void {
  assign i32 field(^Counter local.0(self), value) = i32 add(i32 field(^Counter local.0(self), value), i32 local.1(amount))
}
func fn.1(self: Counter) -> i32 {
  return i32 field(Counter local.2(self), value)
}
func fn.2() -> void {
  expr <unknown> default
  let counter: Counter = <unknown> default
  expr void call bind.6(__impl_8_inc)(^Counter address_of(Counter local.3(counter)), i32 7)
  expr void call bind.6(__impl_8_inc)(^Counter address_of(Counter local.3(counter)), i32 5)
  expr <unknown> default
  let stack: plex { [..]i32[..]i32 = <unknown> default
  expr void call bind.1(__impl_7_push_g_5d10045a)(^plex { [..]i32[..]i32 address_of(plex { [..]i32[..]i32 local.4(stack)), i32 call bind.7(__impl_8_get)(Counter local.3(counter)))
  expr void call bind.1(__impl_7_push_g_5d10045a)(^plex { [..]i32[..]i32 address_of(plex { [..]i32[..]i32 local.4(stack)), i32 30)
  expr void call fn (string) -> void field(module bind.4(io), prn)(string interpolate(i32 call bind.2(__impl_7_pop_g_5566807a)(^plex { [..]i32[..]i32 address_of(plex { [..]i32[..]i32 local.4(stack))), <unknown> " ", i32 call bind.2(__impl_7_pop_g_5566807a)(^plex { [..]i32[..]i32 address_of(plex { [..]i32[..]i32 local.4(stack))), <unknown> " ", i32 call bind.7(__impl_8_get)(Counter local.3(counter))))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c" \00"

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
declare void @m2.fn.0(ptr %self, i32 %elem)
declare i32 @m2.fn.1(ptr %self)

define internal void @fn.0(ptr %self, i32 %amount) {
  %t0 = load { i32 }, ptr %self
  %t1 = extractvalue { i32 } %t0, 0
  %t2 = add i32 %t1, %amount
  %t3 = getelementptr inbounds { i32 }, ptr %self, i64 0, i32 0
  store i32 %t2, ptr %t3
  ret void
}

define internal i32 @fn.1({ i32 } %self) {
  %t0 = extractvalue { i32 } %self, 0
  ret i32 %t0
}

define internal void @fn.2() {
  %local.3 = alloca { i32 }
  store { i32 } zeroinitializer, ptr %local.3
  call void @fn.0(ptr %local.3, i32 7)
  call void @fn.0(ptr %local.3, i32 5)
  %local.4 = alloca { ptr }
  store { ptr } zeroinitializer, ptr %local.4
  %t0 = load { i32 }, ptr %local.3
  %t1 = call i32 @fn.1({ i32 } %t0)
  call void @m2.fn.0(ptr %local.4, i32 %t1)
  call void @m2.fn.0(ptr %local.4, i32 30)
  %t2 = call i64 @string_builder_mark()
  %t3 = call i32 @m2.fn.1(ptr %local.4)
  %t4 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t4, i32 %t3)
  call void @string_builder_append_string(ptr %t4)
  %t5 = alloca { ptr, i64 }
  %t6 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 1 }, ptr %t6
  call void @to_string$string(ptr %t5, ptr %t6)
  call void @string_builder_append_string(ptr %t5)
  %t7 = call i32 @m2.fn.1(ptr %local.4)
  %t8 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t8, i32 %t7)
  call void @string_builder_append_string(ptr %t8)
  %t9 = alloca { ptr, i64 }
  %t10 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 1 }, ptr %t10
  call void @to_string$string(ptr %t9, ptr %t10)
  call void @string_builder_append_string(ptr %t9)
  %t11 = load { i32 }, ptr %local.3
  %t12 = call i32 @fn.1({ i32 } %t11)
  %t13 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t13, i32 %t12)
  call void @string_builder_append_string(ptr %t13)
  %t14 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t14, i64 %t2)
  %t15 = load { ptr, i64 }, ptr %t14
  call void @$prn({ ptr, i64 } %t15)
  ret void
}

@$__impl_8_inc = internal alias void (ptr, i32), ptr @fn.0
@$__impl_8_get = internal alias i32 ({ i32 }), ptr @fn.1
@$main = alias void (), ptr @fn.2
