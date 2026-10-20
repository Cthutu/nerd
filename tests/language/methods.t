=== case: 130-inherent-impl-methods ===
use std.io

Stack :: plex [T] {
    data [..]T
}

impl [T] Stack[T] {
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
module module.methods.input(methods.input)
import module.std.io(std.io)
import import.0 prn from module.core(core).decl.N: <unknown>
import import.1 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.2 prn_text from module.core(core).decl.N: fn (string) -> void
import import.3 prn_empty from module.core(core).decl.N: fn () -> void
bind prn = import.0
bind input = import.1
bind prn_text = import.2
bind prn_empty = import.3
bind Stack = type.0
bind Counter = type.1
bind __impl_Counter_inc = fn.0
bind __impl_Counter_get = fn.1
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
  let counter: Counter = <unknown> default
  expr void call bind.6(__impl_Counter_inc)(^Counter address_of(Counter local.3(counter)), i32 7)
  expr void call bind.6(__impl_Counter_inc)(^Counter address_of(Counter local.3(counter)), i32 5)
  let stack: plex { [..]i32 data } = <unknown> default
  expr void call decl.1(__impl_Stack_T_push_g_2ffac2fa)(^plex { [..]i32 data } address_of(plex { [..]i32 data } local.4(stack)), i32 call bind.7(__impl_Counter_get)(Counter local.3(counter)))
  expr void call decl.1(__impl_Stack_T_push_g_2ffac2fa)(^plex { [..]i32 data } address_of(plex { [..]i32 data } local.4(stack)), i32 30)
  expr void call bind.2(prn_text)(string interpolate(i32 call decl.2(__impl_Stack_T_pop_g_2c164aaa)(^plex { [..]i32 data } address_of(plex { [..]i32 data } local.4(stack))), <unknown> " ", i32 call decl.2(__impl_Stack_T_pop_g_2c164aaa)(^plex { [..]i32 data } address_of(plex { [..]i32 data } local.4(stack))), <unknown> " ", i32 call bind.7(__impl_Counter_get)(Counter local.3(counter))))
}
inst func fn.3(self: ^plex { [..]i32 data }, elem: i32) -> void {
  expr void call fn (i32) -> void field([..]i32 field(^plex { [..]i32 data } local.5(self), data), push)(i32 local.6(elem))
}
inst func fn.4(self: ^plex { [..]i32 data }) -> i32 {
  return i32 call fn () -> i32 field([..]i32 field(^plex { [..]i32 data } local.7(self), data), pop)()
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [43 x i8] c"tests/language/methods.t\00"
@.str.m0.0 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c" \00"

declare i1 @nrt_string_eq(ptr, ptr)
declare void @nrt_string_builder_reset()
declare i64 @nrt_string_builder_mark()
declare void @nrt_string_builder_append_string(ptr)
declare void @nrt_string_builder_append_byte(i8)
declare void @nrt_string_builder_finish(ptr, i64)
declare void @nrt_string_builder_finish_in(ptr, i64, ptr)
declare void @nrt_to_string_string(ptr, ptr)
declare void @nrt_to_string_bool(ptr, i1)
declare void @nrt_to_string_i8(ptr, i8)
declare void @nrt_to_string_i16(ptr, i16)
declare void @nrt_to_string_i32(ptr, i32)
declare void @nrt_to_string_i64(ptr, i64)
declare void @nrt_to_string_u8(ptr, i8)
declare void @nrt_to_string_u16(ptr, i16)
declare void @nrt_to_string_u32(ptr, i32)
declare void @nrt_to_string_u64(ptr, i64)
declare void @nrt_to_string_isize(ptr, i64)
declare void @nrt_to_string_usize(ptr, i64)
declare void @nrt_to_string_f32(ptr, float)
declare void @nrt_to_string_f64(ptr, double)
declare ptr @nrt_mem_alloc(i64, i64, ptr, i32)
declare ptr @nrt_mem_realloc(ptr, i64, i64, ptr, i32)
declare void @nrt_mem_free(ptr)
declare i64 @nrt_mem_size(ptr)

declare ptr @$input({ ptr, i64 })
declare void @$prn_text({ ptr, i64 })
declare void @$prn_empty()

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
  %local.4 = alloca { ptr }
  %t6 = alloca { ptr, i64 }
  %t10 = alloca { ptr, i64 }
  store { i32 } zeroinitializer, ptr %local.3
  call void @fn.0(ptr %local.3, i32 7)
  call void @fn.0(ptr %local.3, i32 5)
  store { ptr } zeroinitializer, ptr %local.4
  %t0 = load { i32 }, ptr %local.3
  %t1 = call i32 @fn.1({ i32 } %t0)
  call void @fn.3(ptr %local.4, i32 %t1)
  call void @fn.3(ptr %local.4, i32 30)
  %t2 = call i64 @nrt_string_builder_mark()
  %t3 = call i32 @fn.4(ptr %local.4)
  %t4 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t4, i32 %t3)
  call void @nrt_string_builder_append_string(ptr %t4)
  %t5 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 1 }, ptr %t6
  call void @nrt_to_string_string(ptr %t5, ptr %t6)
  call void @nrt_string_builder_append_string(ptr %t5)
  %t7 = call i32 @fn.4(ptr %local.4)
  %t8 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t8, i32 %t7)
  call void @nrt_string_builder_append_string(ptr %t8)
  %t9 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 1 }, ptr %t10
  call void @nrt_to_string_string(ptr %t9, ptr %t10)
  call void @nrt_string_builder_append_string(ptr %t9)
  %t11 = load { i32 }, ptr %local.3
  %t12 = call i32 @fn.1({ i32 } %t11)
  %t13 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t13, i32 %t12)
  call void @nrt_string_builder_append_string(ptr %t13)
  %t14 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t14, i64 %t2)
  %t15 = load { ptr, i64 }, ptr %t14
  call void @$prn_text({ ptr, i64 } %t15)
  ret void
}

define internal void @fn.3(ptr %self, i32 %elem) {
  %t0 = getelementptr inbounds { ptr }, ptr %self, i64 0, i32 0
  %t1 = load ptr, ptr %t0
  %t2 = icmp eq ptr %t1, null
  br i1 %t2, label %dynarray.alloc.0, label %dynarray.ready.1
dynarray.alloc.0:
  %t3 = call ptr @nrt_mem_alloc(i64 32, i64 16, ptr @.macro.file.m0, i32 9)
  %t4 = getelementptr inbounds { ptr, i64, i64 }, ptr %t3, i64 0, i32 0
  %t5 = getelementptr inbounds { ptr, i64, i64 }, ptr %t3, i64 0, i32 1
  %t6 = getelementptr inbounds { ptr, i64, i64 }, ptr %t3, i64 0, i32 2
  %t7 = getelementptr inbounds i8, ptr %t3, i64 32
  store ptr %t7, ptr %t4
  store i64 0, ptr %t5
  store i64 0, ptr %t6
  store ptr %t7, ptr %t0
  br label %dynarray.ready.1
dynarray.ready.1:
  %t8 = load ptr, ptr %t0
  %t9 = getelementptr inbounds i8, ptr %t8, i64 -32
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
  %t22 = add i64 32, %t21
  %t23 = call ptr @nrt_mem_realloc(ptr %t9, i64 %t22, i64 16, ptr @.macro.file.m0, i32 9)
  %t24 = getelementptr inbounds i8, ptr %t23, i64 32
  %t25 = getelementptr inbounds { ptr, i64, i64 }, ptr %t23, i64 0, i32 0
  %t26 = getelementptr inbounds { ptr, i64, i64 }, ptr %t23, i64 0, i32 2
  store ptr %t24, ptr %t25
  store i64 %t20, ptr %t26
  store ptr %t24, ptr %t0
  br label %dynarray.store.3
dynarray.store.3:
  %t27 = load ptr, ptr %t0
  %t28 = getelementptr inbounds i8, ptr %t27, i64 -32
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
  %t2 = getelementptr inbounds i8, ptr %t1, i64 -32
  %t3 = getelementptr inbounds { ptr, i64, i64 }, ptr %t2, i64 0, i32 1
  %t4 = load i64, ptr %t3
  %t5 = sub i64 %t4, 1
  %t6 = getelementptr inbounds i32, ptr %t1, i64 %t5
  %t7 = load i32, ptr %t6
  store i64 %t5, ptr %t3
  ret i32 %t7
}

@$__impl_Counter_inc = internal alias void (ptr, i32), ptr @fn.0
@$__impl_Counter_get = internal alias i32 ({ i32 }), ptr @fn.1
@$main = hidden alias void (), ptr @fn.2

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)

=== case: 141-method-regressions ===
use std.io

Rect :: plex {
    width  i32
    height i32
}

impl Rect {

    area :: fn (self: Self) -> i32 {
        return self.width * self.height
    }

    grow :: fn (self: ^Self, amount: i32) {
        self.width += amount
        self.height += amount
    }

}

use_methods :: fn (rect: ^Rect) -> i32 {
    rect.grow(2)
    return rect.area()
}

main :: fn () -> i32 {
    rect: Rect
    rect.grow(3)
    result := use_methods(^rect)
    prn($"area {result}")
    return 0
}
¬
0
¬
area 25

¬
hir 0
module module.methods.input(methods.input)
import module.std.io(std.io)
import import.0 prn from module.core(core).decl.N: <unknown>
import import.1 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.2 prn_text from module.core(core).decl.N: fn (string) -> void
import import.3 prn_empty from module.core(core).decl.N: fn () -> void
bind prn = import.0
bind input = import.1
bind prn_text = import.2
bind prn_empty = import.3
bind Rect = type.0
bind __impl_Rect_area = fn.0
bind __impl_Rect_grow = fn.1
bind use_methods = fn.2
bind main = fn.3
type type.0 = Rect
func fn.0(self: Rect) -> i32 {
  return i32 multiply(i32 field(Rect local.0(self), width), i32 field(Rect local.0(self), height))
}
func fn.1(self: ^Rect, amount: i32) -> void {
  assign i32 field(^Rect local.1(self), width) = i32 add(i32 field(^Rect local.1(self), width), i32 local.2(amount))
  assign i32 field(^Rect local.1(self), height) = i32 add(i32 field(^Rect local.1(self), height), i32 local.2(amount))
}
func fn.2(rect: ^Rect) -> i32 {
  expr void call bind.6(__impl_Rect_grow)(^Rect local.3(rect), i32 2)
  return i32 call bind.5(__impl_Rect_area)(Rect deref(^Rect local.3(rect)))
}
func fn.3() -> i32 {
  let rect: Rect = <unknown> default
  expr void call bind.6(__impl_Rect_grow)(^Rect address_of(Rect local.4(rect)), i32 3)
  let result: i32 = i32 call bind.7(use_methods)(^Rect address_of(Rect local.4(rect)))
  expr void call bind.2(prn_text)(string interpolate(<unknown> "area ", i32 local.5(result)))
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [40 x i8] c"tests/language/methods.t\00"
@.str.m0.0 = private unnamed_addr constant [6 x i8] c"area \00"

declare i1 @nrt_string_eq(ptr, ptr)
declare void @nrt_string_builder_reset()
declare i64 @nrt_string_builder_mark()
declare void @nrt_string_builder_append_string(ptr)
declare void @nrt_string_builder_append_byte(i8)
declare void @nrt_string_builder_finish(ptr, i64)
declare void @nrt_string_builder_finish_in(ptr, i64, ptr)
declare void @nrt_to_string_string(ptr, ptr)
declare void @nrt_to_string_bool(ptr, i1)
declare void @nrt_to_string_i8(ptr, i8)
declare void @nrt_to_string_i16(ptr, i16)
declare void @nrt_to_string_i32(ptr, i32)
declare void @nrt_to_string_i64(ptr, i64)
declare void @nrt_to_string_u8(ptr, i8)
declare void @nrt_to_string_u16(ptr, i16)
declare void @nrt_to_string_u32(ptr, i32)
declare void @nrt_to_string_u64(ptr, i64)
declare void @nrt_to_string_isize(ptr, i64)
declare void @nrt_to_string_usize(ptr, i64)
declare void @nrt_to_string_f32(ptr, float)
declare void @nrt_to_string_f64(ptr, double)

declare ptr @$input({ ptr, i64 })
declare void @$prn_text({ ptr, i64 })
declare void @$prn_empty()

define internal i32 @fn.0({ i32, i32 } %self) {
  %t0 = extractvalue { i32, i32 } %self, 0
  %t1 = extractvalue { i32, i32 } %self, 1
  %t2 = mul i32 %t0, %t1
  ret i32 %t2
}

define internal void @fn.1(ptr %self, i32 %amount) {
  %t0 = load { i32, i32 }, ptr %self
  %t1 = extractvalue { i32, i32 } %t0, 0
  %t2 = add i32 %t1, %amount
  %t3 = getelementptr inbounds { i32, i32 }, ptr %self, i64 0, i32 0
  store i32 %t2, ptr %t3
  %t4 = load { i32, i32 }, ptr %self
  %t5 = extractvalue { i32, i32 } %t4, 1
  %t6 = add i32 %t5, %amount
  %t7 = getelementptr inbounds { i32, i32 }, ptr %self, i64 0, i32 1
  store i32 %t6, ptr %t7
  ret void
}

define internal i32 @fn.2(ptr %rect) {
  call void @fn.1(ptr %rect, i32 2)
  %t0 = load { i32, i32 }, ptr %rect
  %t1 = call i32 @fn.0({ i32, i32 } %t0)
  ret i32 %t1
}

define internal i32 @fn.3() {
  %local.4 = alloca { i32, i32 }
  %t3 = alloca { ptr, i64 }
  store { i32, i32 } zeroinitializer, ptr %local.4
  call void @fn.1(ptr %local.4, i32 3)
  %t0 = call i32 @fn.2(ptr %local.4)
  %t1 = call i64 @nrt_string_builder_mark()
  %t2 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 5 }, ptr %t3
  call void @nrt_to_string_string(ptr %t2, ptr %t3)
  call void @nrt_string_builder_append_string(ptr %t2)
  %t4 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t4, i32 %t0)
  call void @nrt_string_builder_append_string(ptr %t4)
  %t5 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t5, i64 %t1)
  %t6 = load { ptr, i64 }, ptr %t5
  call void @$prn_text({ ptr, i64 } %t6)
  ret i32 0
}

@$__impl_Rect_area = internal alias i32 ({ i32, i32 }), ptr @fn.0
@$__impl_Rect_grow = internal alias void (ptr, i32), ptr @fn.1
@$use_methods = internal alias i32 (ptr), ptr @fn.2
@$main = hidden alias i32 (), ptr @fn.3

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)

=== case: 142-associated-functions-and-type-aliases ===
Box :: plex [T] {
    value T
}

IntBox :: Box[i32]

global_box := IntBox.init(40)

impl [T] Box[T] {
    init :: fn (value: T) -> Self {
        return { value }
    }

    item_size :: fn (self: Self) -> usize {
        return T.size
    }
}

main :: fn () -> i32 {
    box := IntBox.init(2)
    return global_box.value + box.value + box.item_size().as(i32) - i32.size.as(i32)
}
¬
42
¬

¬
hir 0
bind Box = type.0
bind IntBox = type.1
bind global_box = value.0
bind main = fn.0
generic type type.0 = <unknown>
type type.1 = IntBox
global value.0: IntBox = IntBox call fn (i32) -> IntBox field(IntBox bind.1(IntBox), init)(i32 40)
func fn.0() -> i32 {
  let box: IntBox = IntBox call fn (i32) -> IntBox field(IntBox bind.1(IntBox), init)(i32 2)
  return i32 subtract(i32 add(i32 add(i32 field(IntBox bind.2(global_box), value), i32 field(IntBox local.0(box), value)), i32 cast(usize call decl.4(__impl_Box_T_item_size_g_54fbddea)(IntBox local.0(box)) as i32)), i32 cast(usize field(i32 i32, size) as i32))
}
inst func fn.1(value: i32) -> IntBox {
  return IntBox plex(value: i32 local.1(value))
}
inst func fn.2(self: IntBox) -> usize {
  return usize field(i32 T, size)
}
¬
; nerd llvm-ir 0
; generated from HIR

@$global_box = internal global { i32 } zeroinitializer

define void @m0.init() {
  %t0 = call { i32 } @fn.1(i32 40)
  store { i32 } %t0, ptr @$global_box
  ret void
}

define internal i32 @fn.0() {
  %t0 = call { i32 } @fn.1(i32 2)
  %t1 = load { i32 }, ptr @$global_box
  %t2 = extractvalue { i32 } %t1, 0
  %t3 = extractvalue { i32 } %t0, 0
  %t4 = add i32 %t2, %t3
  %t5 = call i64 @fn.2({ i32 } %t0)
  %t6 = trunc i64 %t5 to i32
  %t7 = add i32 %t4, %t6
  %t8 = trunc i64 4 to i32
  %t9 = sub i32 %t7, %t8
  ret i32 %t9
}

define internal { i32 } @fn.1(i32 %value) {
  %t0 = insertvalue { i32 } poison, i32 %value, 0
  ret { i32 } %t0
}

define internal i64 @fn.2({ i32 } %self) {
  ret i64 4
}

@$main = hidden alias i32 (), ptr @fn.0

=== case: 185-unused-method-receiver ===
Thing :: plex {
    value i32
}

impl Thing {
    touch :: fn (_thing: ^Self, _value: i32) -> bool {
        return no
    }
}

main :: fn () -> i32 {
    thing := Thing { value: 1 }
    _ := thing.touch(2)
    assert thing.value == 1
    return 0
}
¬
0
¬

¬

¬

=== case: 187-associated-function-return-containing-self ===
Box :: plex {
    value i32
}

impl Box {
    try_new :: fn (value: i32) -> ?Self {
        return Self { value }
    }
}

main :: fn () -> i32 {
    box := on Box.try_new(42) {
        value => value
        else => return 1
    }
    return box.value
}
¬
42
¬

¬

¬

=== case: 188-imported-associated-call-target ===
use test.associated_b
use test.associated_a

main :: fn () -> i32 {
    value := on A.new() {
        value  => value
        else => return 1
    }
    return value.id
}
¬
42
¬

¬

¬

=== case: 197-single-branch-extract-binder-method ===
Value :: plex {
    number i32
}

impl Value {
    increment :: fn (value: ^Self) {
        value.number += 1
    }
}

make_value :: fn () -> ?Value {
    return Value { number: 40 }
}

main :: fn () -> i32 {
    result := 0
    on make_value() => [value] {
        value.increment()
        result = value.number
    }
    return result - 41
}
¬
0
¬

¬

¬

=== case: 178-run-method-returns-pointer-to-pushed-resized-layer ===
Layer :: plex {
    width u16
    data [..]u32
}

Handle :: plex {
    index usize
}

System :: plex {
    layers [..]Layer
}

resize :: fn (layer: ^Layer, width: u16) {
    layer.width = width
    layer.data.resize_undefined_to(width.as(usize))
}

impl Layer {

    pixel_width :: fn (layer: ^Self) -> u16 {
        return layer.width
    }

}

impl System {

    init :: fn () -> Self {
        return { layers: nil }
    }

    done :: fn (system: ^Self) {
        system.layers[0].data.free()
        system.layers.free()
    }

    add :: fn (system: ^Self, width: u16) -> Handle {
        layer := Layer { width: 0 data: nil }
        resize(^layer, width)
        system.layers.push(layer)
        return Handle { index: system.layers.count - 1 }
    }

    get :: fn (system: ^Self, handle: Handle) -> ^Layer {
        on handle.index >= system.layers.count => return nil
        return ^system.layers[handle.index]
    }

}

main :: fn () {
    system := System.init()
    defer system.done()

    handle := system.add(4)
    layer := system.get(handle)

    assert layer != nil
    assert layer.pixel_width() == 4
}
¬
0
¬

¬

¬

=== case: 184-run-associated-functions-with-same-name ===
A :: plex {
    x i32
}

B :: plex {
    y i32
}

impl A {

    pub init :: fn () -> Self {
        return { x: 1 }
    }

}

impl B {

    pub init :: fn () -> Self {
        return { y: 2 }
    }

}

main :: fn () -> i32 {
    a := A.init()
    b := B.init()
    return a.x + b.y
}
¬
3
¬

¬

¬

=== case: 236-check-imported-stdlib-part-slice-method ===
use std.compress

main :: fn () -> i32 {
    compressed : [8]u8 = [120, 156, 3, 0, 0, 0, 0, 1]
    result := compressed[..].deflate_zlib()

    on result {
        output => {
            status := output.count == 0
            output.free()
            on status => return 0
        }
        else => {
        }
    }

    return 1
}
¬
0
¬

¬

¬

=== case: 237-check-pattern-bound-dynarray-slice-method ===
use std.compress

Container :: plex {
    chunks [..][]u8
}

make_container :: fn (compressed: []u8) -> Container\i32 {
    container := Container { chunks: nil }
    container.chunks.push(compressed)
    return container
}

main :: fn () -> i32 {
    compressed : [8]u8 = [120, 156, 3, 0, 0, 0, 0, 1]
    result_container := make_container(compressed[..])

    on result_container {
        value => {
            bytes : [..]u8 = nil
            direct : [..]u8\string = bytes
            on direct {
                output => {
                    status := output.count == 0
                    output.free()
                    on status => return 0
                }
                else => {
                }
            }
            value.chunks.free()
        }
        _! => {
        }
    }

    return 1
}
¬
0
¬

¬

¬
