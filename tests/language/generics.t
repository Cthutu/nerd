=== case: 127-generic-types ===
Box :: plex [T] {
    value T
}

Cell :: union [T] {
    number T
    text   string
}

Maybe :: enum [T] { None Some(T) }

main :: fn () {
    b: Box[i32]
    b.value = 7

    c: Cell[i32]
    c.number = b.value + 1

    m: Maybe[i32] = Some(c.number)
    on m {
        Some(value) => prn($"generic {value}")
        None => prn("none")
    }
}
¬
0
¬
generic 8

¬
hir 0
module module.generics.input(generics.input)
import import.0 prn from module.core(core).decl.N: <unknown>
import import.1 prn_text from module.core(core).decl.N: fn (string) -> void
import import.2 prn_empty from module.core(core).decl.N: fn () -> void
bind prn = import.0
bind prn_text = import.1
bind prn_empty = import.2
bind Box = type.0
bind Cell = type.1
bind Maybe = type.2
bind main = fn.0
generic type type.0 = <unknown>
generic type type.1 = <unknown>
generic type type.2 = <unknown>
func fn.0() -> void {
  let b: plex { i32 value } = <unknown> default
  assign i32 field(plex { i32 value } local.0(b), value) = i32 7
  let c: union { i32 number, string text } = <unknown> default
  assign i32 field(union { i32 number, string text } local.1(c), number) = i32 add(i32 field(plex { i32 value } local.0(b), value), i32 1)
  let m: enum { None, Some(i32) } = enum { None, Some(i32) } call Some(i32 field(union { i32 number, string text } local.1(c), number))
  expr void on enum { None, Some(i32) } local.2(m) {
    enum_variant(Some, as value) => {
      expr void call bind.1(prn_text)(string interpolate(<unknown> "generic ", i32 local.3(value)))
    }
    value(enum { None, Some(i32) } None) => {
      expr void call bind.1(prn_text)(string "none")
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [26 x i8] c"tests/language/generics.t\00"
@.str.m0.0 = private unnamed_addr constant [9 x i8] c"generic \00"
@.str.m0.1 = private unnamed_addr constant [5 x i8] c"none\00"

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

declare void @$prn_text({ ptr, i64 })
declare void @$prn_empty()

define internal void @fn.0() {
  %local.0 = alloca { i32 }
  %local.1 = alloca [2 x i64]
  %local.2 = alloca { i64, i32 }
  %t16 = alloca { ptr, i64 }
  store { i32 } zeroinitializer, ptr %local.0
  %t0 = getelementptr inbounds { i32 }, ptr %local.0, i64 0, i32 0
  store i32 7, ptr %t0
  store [2 x i64] zeroinitializer, ptr %local.1
  %t1 = load { i32 }, ptr %local.0
  %t2 = extractvalue { i32 } %t1, 0
  %t3 = add i32 %t2, 1
  store i32 %t3, ptr %local.1
  %t4 = load [2 x i64], ptr %local.1
  %t5 = alloca [2 x i64]
  store [2 x i64] %t4, ptr %t5
  %t6 = load i32, ptr %t5
  %t7 = insertvalue { i64, i32 } poison, i64 1, 0
  %t8 = insertvalue { i64, i32 } %t7, i32 %t6, 1
  store { i64, i32 } %t8, ptr %local.2
  %t9 = load { i64, i32 }, ptr %local.2
  %t10 = extractvalue { i64, i32 } %t9, 0
  %t11 = icmp eq i64 %t10, 1
  %t12 = extractvalue { i64, i32 } %t9, 1
  %t13 = and i1 %t11, 1
  br i1 %t13, label %on.body.1, label %on.next.2
on.body.1:
  %t14 = call i64 @nrt_string_builder_mark()
  %t15 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 8 }, ptr %t16
  call void @nrt_to_string_string(ptr %t15, ptr %t16)
  call void @nrt_string_builder_append_string(ptr %t15)
  %t17 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t17, i32 %t12)
  call void @nrt_string_builder_append_string(ptr %t17)
  %t18 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t18, i64 %t14)
  %t19 = load { ptr, i64 }, ptr %t18
  call void @$prn_text({ ptr, i64 } %t19)
  br label %on.end.0
on.next.2:
  %t20 = insertvalue { i64, i32 } poison, i64 0, 0
  %t21 = insertvalue { i64, i32 } %t20, i32 0, 1
  %t22 = extractvalue { i64, i32 } %t9, 0
  %t23 = extractvalue { i64, i32 } %t21, 0
  %t24 = icmp eq i64 %t22, %t23
  br i1 %t24, label %on.body.3, label %on.end.0
on.body.3:
  call void @$prn_text({ ptr, i64 } { ptr @.str.m0.1, i64 4 })
  br label %on.end.0
on.end.0:
  ret void
}

@$main = hidden alias void (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)

=== case: 128-generic-functions ===
id :: fn [T] (value: T) => value

choose :: fn [T] (left: T, right: T, use_left: bool) => on use_left {
    yes => left
    else => right
}

Box :: plex [T] {
    value T
}

wrap :: fn [T] (value: T) -> Box[T] {
    box: Box[T]
    box.value = value
    return box
}

main :: fn () {
    number := id(7)
    text := id("ok")
    explicit := id[i32](number + 5)

    int_id := id[i32]
    from_value := int_id(explicit + 30)

    box := wrap[string](text)
    chosen := choose[string](box.value, "bad", yes)

    prn($"{number} {text} {explicit} {from_value} {chosen}")
}
¬
0
¬
7 ok 12 42 ok

¬
hir 0
bind Box = type.0
bind main = fn.0
generic type type.0 = <unknown>
func fn.0() -> void {
inst func fn.1(value: i32) -> i32 {
inst func fn.2(value: string) -> string {
¬
; nerd llvm-ir 0
define internal void @fn.0() {
define internal i32 @fn.1(i32 %value) {
define internal { ptr, i64 } @fn.2({ ptr, i64 } %value) {
@$main = hidden alias void (), ptr @fn.0

=== case: 169-generic-trait-declaration ===
Iterator :: trait [Item] {
    next :: fn (self: Self) -> Item
}

Mapper :: trait [Input, Output] for This {
    map :: fn (value: This, other: Input) -> Output
}

main :: fn () -> i32 {
    return 0
}
¬
0
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  return i32 0
}
¬

=== case: 189-generic-enum-impl-methods ===
Maybe :: enum [T] {
    None
    Some(T)
}

impl [T] Maybe[T] {
    expect :: fn (self: Self, fallback: T) -> T {
        return on self {
            Some(value) => value
            None        => fallback
        }
    }
}

main :: fn () -> i32 {
    first  : Maybe[i32] = Some(42)
    second : Maybe[i32] = None
    return first.expect(0) + second.expect(1) - 1
}
¬
42
¬

¬

¬

=== case: 192-explicit-generic-impls ===
Stack :: plex [U] {
    value U
}

impl [T] Stack[i32] {
    identity :: fn (self: Self, value: T) -> T {
        copy: T = value
        return copy
    }
}

impl [T] []T
where T: Eq {
    contains :: fn (self: ^Self, value: T) -> bool {
        for i in [0 .. self.count] {
            on self^[i] == value => return yes
        }
        return no
    }
}

main :: fn () -> i32 {
    stack  : Stack[i32] = { value: 0 }
    number := stack.identity(40)
    word   := stack.identity[string]("ok")
    values := [1, 2, 3]

    on !values.contains(2) => return 1
    on values.contains(4) => return 2
    on [].contains(1) => return 3
    return number + word.count.as(i32) - 42
}
¬
0
¬

¬

¬

=== case: 237-generic-callback-methods ===
Dispatch :: plex { calls i32 }
impl Dispatch {
    apply :: fn [A, R] (self: ^Self, callback: fn (argument: A) -> R, argument: A) -> R {
        self.calls += 1
        return callback(argument)
    }
}
square :: fn (number: ^i32) -> i32 { return number^ * number^ }
length :: fn (text: string) -> usize { return text.count }
erased :: fn (pointer: ^void) -> i32 { return pointer.as(^i32)^ }
main :: fn () -> i32 {
    dispatch : Dispatch
    number : i32 = 7
    assert dispatch.apply(square, ^number) == 49
    assert dispatch.apply(length, "typed") == 5
    assert dispatch.apply(erased, ^number) == 7
    assert dispatch.apply[(^i32, i32)](square, ^number) == 49
    assert dispatch.calls == 4
    prn("Generic callback method inference passed")
    return 0
}
¬
0
¬
Generic callback method inference passed

¬
hir 0
¬
; nerd llvm-ir 0

=== case: 238-generic-erased-callback-specialization ===
Invocation :: plex [A] {
    callback fn (argument: A) -> i32
    argument A
}
run :: fn [A] (context: ^void) -> i32 {
    invocation := context.as(^Invocation[A])
    return invocation.callback(invocation.argument)
}
from_pointer :: fn (number: ^i32) -> i32 { return number^ }
from_value :: fn (number: i32) -> i32 { return number * number }
from_float :: fn (number: f64) -> i32 { return (number + 1.0).as(i32) }
main :: fn () -> i32 {
    number : i32 = 7
    pointer : Invocation[^i32]
    pointer.callback = from_pointer
    pointer.argument = ^number
    integer : Invocation[i32]
    integer.callback = from_value
    integer.argument = 9
    decimal : Invocation[f64]
    decimal.callback = from_float
    decimal.argument = 10.0
    first: fn (context: ^void) -> i32 = run[^i32]
    second: fn (context: ^void) -> i32 = run[i32]
    third: fn (context: ^void) -> i32 = run[f64]
    assert first(^pointer) == 7
    assert second(^integer) == 81
    assert third(^decimal) == 11
    prn("Explicit erased callback specializations passed")
    return 0
}
¬
0
¬
Explicit erased callback specializations passed

¬
hir 0
¬
; nerd llvm-ir 0

=== case: 181-run-generic-method-user-type-arg ===
Rect :: plex {
    x u32
}

main :: fn () -> i32 {
    rects := temp_arena.alloc_array[Rect](1)
    rects[0].x = 42
    return rects[0].x.as(i32)
}
¬
42
¬

¬

¬

=== case: 129-cross-module-generics ===
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
module module.generics.input(generics.input)
import module.std.io(std.io)
import module.test.generics(test.generics)
import import.0 prn from module.core(core).decl.N: <unknown>
import import.1 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.2 Stack from module.test.generics(test.generics).decl.N: <unknown>
import import.3 stack_push from module.test.generics(test.generics).decl.N: <unknown>
import import.4 stack_pop from module.test.generics(test.generics).decl.N: <unknown>
import import.5 Box from module.test.generics(test.generics).decl.N: <unknown>
import import.6 make_box from module.test.generics(test.generics).decl.N: <unknown>
import import.7 pick from module.test.generics(test.generics).decl.N: <unknown>
import import.8 prn_text from module.core(core).decl.N: fn (string) -> void
import import.9 prn_empty from module.core(core).decl.N: fn () -> void
bind prn = import.0
bind input = import.1
bind Stack = import.2
bind stack_push = import.3
bind stack_pop = import.4
bind Box = import.5
bind make_box = import.6
bind pick = import.7
bind prn_text = import.8
bind prn_empty = import.9
bind main = fn.0
bind Stack = type.0
bind Box = type.1
generic type type.0 = <unknown>
generic type type.1 = <unknown>
func fn.0() -> void {
  let stack: plex { [..]i32 data } = <unknown> default
  expr void call bind.3(stack_push_g_5df2a8da)(^plex { [..]i32 data } address_of(plex { [..]i32 data } local.0(stack)), i32 42)
  expr void call bind.3(stack_push_g_5df2a8da)(^plex { [..]i32 data } address_of(plex { [..]i32 data } local.0(stack)), i32 13)
  let last: i32 = i32 call bind.4(stack_pop_g_5d10045a)(^plex { [..]i32 data } address_of(plex { [..]i32 data } local.0(stack)))
  let first: i32 = i32 call bind.4(stack_pop_g_5d10045a)(^plex { [..]i32 data } address_of(plex { [..]i32 data } local.0(stack)))
  let box: plex { string value } = plex { string value } call bind.6(make_box_g_5b339127)(string "ok")
  expr void call bind.8(prn_text)(string interpolate(i32 local.1(last), <unknown> " ", i32 local.2(first), <unknown> " ", string field(plex { string value } local.3(box), value)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [43 x i8] c"tests/language/generics.t\00"
@.str.m0.0 = private unnamed_addr constant [3 x i8] c"ok\00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.2 = private unnamed_addr constant [2 x i8] c" \00"

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
declare void @mN.fn.0(ptr %stack, i32 %elem)
declare i32 @mN.fn.1(ptr %stack)
declare { { ptr, i64 } } @mN.fn.2({ ptr, i64 } %value)

define internal void @fn.0() {
  %local.0 = alloca { ptr }
  %t6 = alloca { ptr, i64 }
  %t9 = alloca { ptr, i64 }
  %t12 = alloca { ptr, i64 }
  store { ptr } zeroinitializer, ptr %local.0
  call void @mN.fn.0(ptr %local.0, i32 42)
  call void @mN.fn.0(ptr %local.0, i32 13)
  %t0 = call i32 @mN.fn.1(ptr %local.0)
  %t1 = call i32 @mN.fn.1(ptr %local.0)
  %t2 = call { { ptr, i64 } } @mN.fn.2({ ptr, i64 } { ptr @.str.m0.0, i64 2 })
  %t3 = call i64 @nrt_string_builder_mark()
  %t4 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t4, i32 %t0)
  call void @nrt_string_builder_append_string(ptr %t4)
  %t5 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 1 }, ptr %t6
  call void @nrt_to_string_string(ptr %t5, ptr %t6)
  call void @nrt_string_builder_append_string(ptr %t5)
  %t7 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t7, i32 %t1)
  call void @nrt_string_builder_append_string(ptr %t7)
  %t8 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 1 }, ptr %t9
  call void @nrt_to_string_string(ptr %t8, ptr %t9)
  call void @nrt_string_builder_append_string(ptr %t8)
  %t10 = extractvalue { { ptr, i64 } } %t2, 0
  %t11 = alloca { ptr, i64 }
  store { ptr, i64 } %t10, ptr %t12
  call void @nrt_to_string_string(ptr %t11, ptr %t12)
  call void @nrt_string_builder_append_string(ptr %t11)
  %t13 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t13, i64 %t3)
  %t14 = load { ptr, i64 }, ptr %t13
  call void @$prn_text({ ptr, i64 } %t14)
  ret void
}

@$main = hidden alias void (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)

=== case: 170-where-constraints ===
Display :: trait {
    show :: fn (self: Self) -> string
}

Point :: plex {
    x i32
    y i32
}

impl Display for Point {
    show :: fn (self: Self) => $"Point({self.x}, {self.y})"
}

id :: fn [T] (value: T) -> T
where T: Display {
    return value
}

describe :: fn [T] (value: T) -> string
where T: Display {
    return value.show()
}

describe_explicit :: fn [T] (value: T) -> string
where T: Display {
    return Display.show(value)
}

Box :: plex [T] {
    value T
}

impl [T] Box[T]
where T: Display {
    get :: fn (self: Self) -> T {
        return self.value
    }
}

main :: fn () -> i32 {
    point := id(Point { x: 3, y: 4 })
    return point.x + point.y + describe(point).count.as(i32) +
        describe_explicit(point).count.as(i32)
}
¬
29
¬

¬
hir 0
module module.generics.input(generics.input)
import import.0 __trait_Display_for_c_string_show from module.core(core).decl.N: fn (^^i8) -> string
bind __trait_Display_for_c_string_show = import.0
bind Point = type.0
bind __trait_Display_for_Point_show = fn.0
bind Box = type.1
bind main = fn.1
type type.0 = Point
generic type type.1 = <unknown>
func fn.0(self: Point) -> string {
  return string interpolate(<unknown> "Point(", i32 field(Point local.0(self), x), <unknown> ", ", i32 field(Point local.0(self), y), <unknown> ")")
}
func fn.1() -> i32 {
  let point: Point = Point call decl.3(id_g_55ae2fd0)(Point plex(x: i32 3, y: i32 4))
  return i32 add(i32 add(i32 add(i32 field(Point local.1(point), x), i32 field(Point local.1(point), y)), i32 cast(usize field(string call decl.4(describe_g_55a3e630)(Point local.1(point)), count) as i32)), i32 cast(usize field(string call decl.5(describe_explicit_g_54ec9b50)(Point local.1(point)), count) as i32))
}
inst func fn.2(value: Point) -> Point {
  return Point local.2(value)
}
inst func fn.3(value: Point) -> string {
  return string call bind.2(__trait_Display_for_Point_show)(Point local.3(value))
}
inst func fn.4(value: Point) -> string {
  return string call bind.2(__trait_Display_for_Point_show)(Point local.4(value))
}
¬

=== case: 171-generic-trait-impl ===
Display :: trait {
    show :: fn (self: Self) -> string
}

Point :: plex {
    x i32
    y i32
}

impl Display for Point {
    show :: fn (self: Self) => $"Point({self.x}, {self.y})"
}

Box :: plex [T] {
    value T
}

impl [T] Display for Box[T]
where T: Display {
    show :: fn (self: Self) -> string {
        return self.value.show()
    }
}

describe :: fn [T] (value: T) -> string
where T: Display {
    return value.show()
}

main :: fn () -> i32 {
    point := Point { x: 3, y: 4 }
    boxed: Box[Point]
    boxed.value = point
    return describe(boxed).count.as(i32) + boxed.show().count.as(i32)
}
¬
22
¬

¬
hir 0
¬

=== case: 190-generic-implicit-array-slice ===
use test.generics

pick_local :: fn [T] (values: []T, fallback: T) -> T {
    on values.count == 0 => return fallback
    return values[1]
}

main :: fn () -> i32 {
    values := [10, 20, 30]
    local_fixed    := pick_local(values, 0)
    local_empty    := pick_local([], 22)
    imported_fixed := pick(values, 0)
    imported_empty := pick([], 22)
    return local_fixed + local_empty + imported_fixed + imported_empty
}
¬
84
¬

¬

¬

=== case: 236-generic-task-shapes ===
Box :: plex [T] {
    callback fn (context: ^void) -> T
    context ^void
    result ?T
}
impl [T] Box[T] {
    run :: fn (self: ^Self) -> ?T {
        self.result = self.callback(self.context)
        return self.result
    }
}
pointer :: fn [T] (value: ^T) -> ?^T {
    return value
}
Optional :: plex [T] { value ?T }
impl [T] Optional[T] {
    get :: fn (self: ^Self) -> ?T { return self.value }
}
Error :: enum { Failed }
Outcome :: plex [T] { value T\Error }
impl [T] Outcome[T] {
    get :: fn (self: ^Self) -> T\Error { return self.value }
}
Link :: plex { value ^i32 }
callback :: fn (_context: ^void) -> i32 { return -1 }
read :: fn (context: ^void) -> i32 { return context.as(^i32)^ }
main :: fn () -> i32 {
    assert callback(nil) == -1
    number : i32 = 42
    task : Box[i32]
    task.callback = read
    task.context = (^number).as(^void)
    on task.run() { result => { assert result == 42 } else => { assert no } }
    on pointer(^number) { p => { assert p^ == 42 } else => { assert no } }
    optional : Optional[i32]
    optional.value = number
    on optional.get() { n => { assert n == 42 } else => { assert no } }
    outcome : Outcome[i32]
    outcome.value = number
    on outcome.get() { n => { assert n == 42 } else => { assert no } }
    -- Address publication through a constructor must not reinitialise its
    -- target on each iteration in release LLVM builds.
    count : i32
    links : [4]Link
    for i in [0 .. 4] {
        links[i] = Link { value: ^count }
        links[i].value^ += 1
    }
    assert count == 4
    prn("Generic task shapes passed")
    return 0
}
¬
0
¬
Generic task shapes passed

¬
hir 0
¬
; nerd llvm-ir 0

=== case: 239-generic-pointer-method-inference ===
use std.io
Cell :: plex [T] { value T }
Reader :: plex { }
impl Reader {
    read :: fn [T] (self: ^Self, cell: ^Cell[T]) -> T { return cell.value }
    pair :: fn [T] (self: ^Self, fixed: i32, cell: ^Cell[T]) -> T {
        assert fixed == 7
        return cell.value
    }
}
main :: fn () -> i32 {
    reader : Reader
    integer : Cell[i32]
    decimal : Cell[f64]
    integer.value = 42
    decimal.value = 1.5
    assert reader.read(^integer) == 42
    assert reader.read(^decimal) == 1.5
    assert reader.pair(7, ^integer) == 42
    assert reader.read[i32](^integer) == 42
    prn("Generic pointer method inference passed")
    return 0
}
¬
0
¬
Generic pointer method inference passed

¬
hir 0
¬
; nerd llvm-ir 0

=== case: 246-nested-type-applications ===
support :: use test.compiler_constructs
Cell :: plex [T] { value T }
Nested :: Cell[Cell[i32]]
Deep :: Cell[Cell[Cell[i32]]]
Holder :: plex { value Cell[Cell[i32]] }
identity :: fn (value: Cell[Cell[i32]]) -> Cell[Cell[i32]] { return value }
main :: fn () -> i32 {
    nested: Nested = { value: { value: 42 } }
    deep: Deep = { value: { value: { value: 7 } } }
    holder: Holder = { value: nested }
    assert deep.value.value.value == 7
    foreign: support.Cell[support.Cell[i32]] = { value: { value: 42 } }
    assert foreign.value.value == 42
    return identity(holder.value).value.value
}
¬
42
¬

¬

¬

=== case: 247-explicit-type-arguments ===
support :: use test.compiler_constructs
Pair :: plex [A, B] { first A
    second B }
Cell :: plex [T] { value T }
make_pair :: fn [A, B] (first: A, second: B) -> Pair[A, B] { return { first, second } }
Maker :: plex { }
impl Maker {
    pair :: fn [A, B] (self: ^Self, first: A, second: B) -> Pair[A, B] { return { first, second } }
}
main :: fn () -> i32 {
    pair := make_pair[i32, string](42, "answer")
    maker: Maker
    other := maker.pair[i32, string](7, "seven")
    cell: Cell[i32] = { value: 9 }
    nested := make_pair[Cell[i32], string](cell, "nested")
    values := [1, 2, 3]
    assert other.first == 7 && nested.first.value == 9 && values[1] == 2
    foreign := support.make_pair[i32, string](5, "five")
    assert foreign.first == 5
    prn(pair.second)
    return pair.first
}
¬
42
¬
answer
¬

¬

=== case: 248-applied-associated-calls ===
support :: use test.compiler_constructs
Cell :: plex [T] { value T }
impl [T] Cell[T] { init :: fn (value: T) -> Self { return { value } } }
Pair :: plex [A, B] { first A
    second B }
impl [A, B] Pair[A, B] { init :: fn (first: A, second: B) -> Self { return { first, second } } }
main :: fn () -> i32 {
    cell := Cell[i32].init(42)
    pair := Pair[i32, string].init(9, "nine")
    nested := Cell[Cell[i32]].init(cell)
    assert pair.first == 9 && pair.second == "nine" && nested.value.value == 42
    foreign := support.Cell[i32].init(42)
    foreign_pair := support.Pair[i32, string].init(8, "eight")
    assert foreign.value == 42 && foreign_pair.first == 8 && foreign_pair.second == "eight"
    return cell.value
}
¬
42
¬

¬

¬

=== case: 334-generic-contextual-pointer-literal ===
-- test-release
use inferred_fill

Info :: plex { value i32 other i32 }
Other :: plex { enabled bool }
local_fill :: fn [T] (items: []T, value: ^T) {
    for item in items { item^ = value^ }
}
main :: fn () {
    count := 3
    items: [count]Info
    calls := 0
    fill(items, ^{ value: calls += 1, ... })
    assert calls == 1
    for item in items {
        assert item.value == 1
        assert item.other == 0
    }
    local_fill(items, ^{ value: 7, ... })
    assert items[2].value == 7
    fixed: [2]Other
    fill(fixed, ^{ enabled: yes })
    assert fixed[0].enabled
    local_fill(fixed, ^{ enabled: no })
    assert !fixed[1].enabled
    view := items[..]
    fill(view, ^{ value: 9, ... })
    assert items[0].value == 9
    items.fill_context(^{ value: 11, ... })
    assert items[1].value == 11
    fixed.fill_context(^{ enabled: yes })
    assert fixed[1].enabled
    prn("Generic argument context passed")
}

¬
0
¬
Generic argument context passed

¬

¬

=== case: 371-qualified-generics ===
cells :: use test.qualified_generics
same_cells :: use test.qualified_generics
collections :: use test.generics

IntegerCell :: cells.Cell[i32]

Container :: plex {
    item cells.Cell[i64]
    nested cells.Cell[cells.Cell[i32]]
}

read :: fn (cell: ^cells.Cell[i32]) -> i32 { return cell.value }
pass :: fn [T] (value: T) -> cells.Cell[T] {
    return cells.wrap(value)
}

main :: fn () {
    cell: cells.Cell[i32]
    cell.value = 42
    assert read(^cell) == 42
    literal := cells.Cell[i32] { value: 43 }
    assert literal.get() == 43
    aliased: IntegerCell
    aliased.value = 44
    assert aliased.get() == 44
    same: same_cells.Cell[i32] = literal
    assert same.value == 43

    container: Container
    container.item.value = 5_000_000_000
    container.nested.value.value = 45
    assert container.item.get() == 5_000_000_000
    assert container.nested.value.get() == 45

    assert cells.identity(46) == 46
    assert cells.identity("qualified") == "qualified"
    wide: i64 = 5_000_000_000
    assert cells.identity(wide) == wide
    assert cells.identity[i64](5_000_000_001) == wide + 1
    assert cells.identity[i32](47) == 47
    identity := cells.identity[i32]
    assert identity(48) == 48

    constructed := cells.Cell[i64].init(5_000_000_002)
    assert constructed.get() == wide + 2
    wrapped := cells.wrap(49)
    assert wrapped.get() == 49
    forwarded := pass(50)
    assert forwarded.get() == 50

    box: collections.Box[i64]
    box.value = wide
    made := collections.make_box[i64](wide + 3)
    assert made.value == wide + 3
    assert collections.make_box(wide).value == box.value

    local_cells :: use test.qualified_generics
    assert local_cells.identity[i32](51) == 51
    local_cell: local_cells.Cell[i32]
    local_cell.value = 52
    assert local_cell.get() == 52

    -- Brackets on exported array values still index storage.
    index := 1
    assert cells.values[index] == 13
    cells.values[index] = 53
    assert same_cells.values[index] == 53
}
¬
0
¬

¬

¬

=== case: generic-impl-builtin-casts ===
Cell :: plex [T] { value T }
impl [T] Cell[T] {
    wide :: fn (self: Self) -> f64 { return self.value.as(f64) }
    narrow :: fn (self: Self) -> f32 { return self.value.as(f32) }
    same :: fn (self: Self) -> T { return self.value.as(T) }
}
main :: fn () {
    single := Cell[f32] { value: 4.5 }
    double := Cell[f64] { value: 8.25 }
    assert single.wide() == 4.5
    assert double.narrow() == 8.25
    assert single.same() == 4.5 && double.same() == 8.25
    prn("generic impl casts")
}
¬
0
¬
generic impl casts

¬
¬
=== case: generic-forward-field-concrete-alias ===
Outer :: plex [T] { inner Inner[T] }
OuterF32 :: Outer[f32]
OuterF64 :: Outer[f64]
Inner :: plex [T] { value T }
main :: fn () {
    single := OuterF32 { inner: { value: 42.0 } }
    double := OuterF64 { inner: { value: 24.0 } }
    assert single.inner.value == 42.0 && double.inner.value == 24.0
    prn("forward generic fields")
}
¬
0
¬
forward generic fields

¬
¬
=== case: generic-array-tuple-item-indices ===
unrelated :: fn (other: i32) -> i32 { return other + 1 }
make :: fn [T] (value: T) -> T {
    items := [value, value]
    pair := (items[0], items[1])
    return pair.0
}
main :: fn () {
    assert make(42) == 42
    assert make(4.5) == 4.5
    assert unrelated(0) == 1
    prn("generic array and tuple references")
}
¬
0
¬
generic array and tuple references
¬
¬
