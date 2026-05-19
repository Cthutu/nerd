use std.io

Stack :: plex [T] {
    data [..]T
}

impl Stack[T] {
    push :: fn (self: ^Stack[T], elem: T) {
        self.data.push(elem)
    }

    pop :: fn (self: ^Stack[T]) -> T {
        return self.data.pop()
    }
}

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

    prn($"{stack.pop()} {stack.pop()} {counter.get()}")
}
¬
0
¬
30 12 12

¬
hir 0
module module.0(130-inherent-impl-methods.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind Stack = type.0
bind Counter = type.1
bind __impl_46_inc = fn.0
bind __impl_46_get = fn.1
bind main = fn.2
generic type type.0 = <unknown>
type type.1 = Counter
func fn.0(self: ^Counter, amount: i32) -> void {
  assign i32 field(^Counter local.0(self), value) = i32 add(i32 field(^Counter local.0(self), value), i32 local.1(amount))
}
func fn.1(self: Counter) -> i32 {
  return i32 field(Counter local.2(self), value)
}
func fn.2() -> void {
  expr <unknown> default
  let counter: Counter = <unknown> default
  expr void call bind.4(__impl_46_inc)(^Counter address_of(Counter local.3(counter)), i32 7)
  expr void call bind.4(__impl_46_inc)(^Counter address_of(Counter local.3(counter)), i32 5)
  expr <unknown> default
  let stack: plex { [..]i32[..]i32 = <unknown> default
  expr void call decl.1(__impl_9_push_g_2ffac2fa)(^plex { [..]i32[..]i32 address_of(plex { [..]i32[..]i32 local.4(stack)), i32 call bind.5(__impl_46_get)(Counter local.3(counter)))
  expr void call decl.1(__impl_9_push_g_2ffac2fa)(^plex { [..]i32[..]i32 address_of(plex { [..]i32[..]i32 local.4(stack)), i32 30)
  expr void call bind.0(prn)(string interpolate(i32 call decl.2(__impl_9_pop_g_2c3b981a)(^plex { [..]i32[..]i32 address_of(plex { [..]i32[..]i32 local.4(stack))), <unknown> " ", i32 call decl.2(__impl_9_pop_g_2c3b981a)(^plex { [..]i32[..]i32 address_of(plex { [..]i32[..]i32 local.4(stack))), <unknown> " ", i32 call bind.5(__impl_46_get)(Counter local.3(counter))))
}
inst func fn.3(self: ^plex { [..]i32[..]i32, elem: i32) -> void {
  expr void call fn (i32) -> void field([..]i32 field(^plex { [..]i32[..]i32 local.5(self), data), push)(i32 local.6(elem))
}
inst func fn.4(self: ^plex { [..]i32[..]i32) -> i32 {
  return i32 call fn () -> i32 field([..]i32 field(^plex { [..]i32[..]i32 local.7(self), data), pop)()
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [65 x i8] c"tests/language/130-inherent-impl-methods.t\00"
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
declare ptr @nrt_mem_alloc(i64, i64, ptr, i32)
declare ptr @nrt_mem_realloc(ptr, i64, i64, ptr, i32)
declare void @nrt_mem_free(ptr)

declare void @$prn({ ptr, i64 })
declare ptr @$input({ ptr, i64 })

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
  call void @fn.3(ptr %local.4, i32 %t1)
  call void @fn.3(ptr %local.4, i32 30)
  %t2 = call i64 @string_builder_mark()
  %t3 = call i32 @fn.4(ptr %local.4)
  %t4 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t4, i32 %t3)
  call void @string_builder_append_string(ptr %t4)
  %t5 = alloca { ptr, i64 }
  %t6 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 1 }, ptr %t6
  call void @to_string$string(ptr %t5, ptr %t6)
  call void @string_builder_append_string(ptr %t5)
  %t7 = call i32 @fn.4(ptr %local.4)
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

define internal void @fn.3(ptr %self, i32 %elem) {
  %t0 = getelementptr inbounds { ptr }, ptr %self, i64 0, i32 0
  %t1 = load ptr, ptr %t0
  %t2 = icmp eq ptr %t1, null
  br i1 %t2, label %dynarray.alloc.0, label %dynarray.ready.1
dynarray.alloc.0:
  %t3 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 9)
  %t4 = getelementptr inbounds { ptr, i64, i64 }, ptr %t3, i64 0, i32 0
  %t5 = getelementptr inbounds { ptr, i64, i64 }, ptr %t3, i64 0, i32 1
  %t6 = getelementptr inbounds { ptr, i64, i64 }, ptr %t3, i64 0, i32 2
  %t7 = getelementptr inbounds i8, ptr %t3, i64 24
  store ptr %t7, ptr %t4
  store i64 0, ptr %t5
  store i64 0, ptr %t6
  store ptr %t7, ptr %t0
  br label %dynarray.ready.1
dynarray.ready.1:
  %t8 = load ptr, ptr %t0
  %t9 = getelementptr inbounds i8, ptr %t8, i64 -24
  %t10 = getelementptr inbounds { ptr, i64, i64 }, ptr %t9, i64 0, i32 0
  %t11 = getelementptr inbounds { ptr, i64, i64 }, ptr %t9, i64 0, i32 1
  %t12 = getelementptr inbounds { ptr, i64, i64 }, ptr %t9, i64 0, i32 2
  %t13 = load ptr, ptr %t10
  %t14 = load i64, ptr %t11
  %t15 = load i64, ptr %t12
  %t16 = add i64 %t14, 1
  %t17 = icmp ugt i64 %t16, %t15
  br i1 %t17, label %dynarray.grow.2, label %dynarray.store.3
dynarray.grow.2:
  %t18 = icmp eq i64 %t15, 0
  %t19 = mul i64 %t15, 2
  %t20 = select i1 %t18, i64 1, i64 %t19
  %t21 = mul i64 %t20, 4
  %t22 = add i64 24, %t21
  %t23 = call ptr @nrt_mem_realloc(ptr %t9, i64 %t22, i64 16, ptr @.macro.file.m0, i32 9)
  %t24 = getelementptr inbounds i8, ptr %t23, i64 24
  %t25 = getelementptr inbounds { ptr, i64, i64 }, ptr %t23, i64 0, i32 0
  %t26 = getelementptr inbounds { ptr, i64, i64 }, ptr %t23, i64 0, i32 2
  store ptr %t24, ptr %t25
  store i64 %t20, ptr %t26
  store ptr %t24, ptr %t0
  br label %dynarray.store.3
dynarray.store.3:
  %t27 = load ptr, ptr %t0
  %t28 = getelementptr inbounds i8, ptr %t27, i64 -24
  %t29 = getelementptr inbounds { ptr, i64, i64 }, ptr %t28, i64 0, i32 0
  %t30 = getelementptr inbounds { ptr, i64, i64 }, ptr %t28, i64 0, i32 1
  %t31 = load ptr, ptr %t29
  %t32 = getelementptr inbounds i32, ptr %t31, i64 %t14
  store i32 %elem, ptr %t32
  store i64 %t16, ptr %t30
  ret void
}

define internal i32 @fn.4(ptr %self) {
  %t0 = load { ptr }, ptr %self
  %t1 = extractvalue { ptr } %t0, 0
  %t2 = getelementptr inbounds i8, ptr %t1, i64 -24
  %t3 = getelementptr inbounds { ptr, i64, i64 }, ptr %t2, i64 0, i32 1
  %t4 = load i64, ptr %t3
  %t5 = sub i64 %t4, 1
  %t6 = getelementptr inbounds i32, ptr %t1, i64 %t5
  %t7 = load i32, ptr %t6
  store i64 %t5, ptr %t3
  ret i32 %t7
}

@$__impl_46_inc = internal alias void (ptr, i32), ptr @fn.0
@$__impl_46_get = internal alias i32 ({ i32 }), ptr @fn.1
@$main = alias void (), ptr @fn.2
