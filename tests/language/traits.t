=== case: 161-trait-declaration ===
Display :: trait {
    show :: fn (self: Self) -> string
}

Hasher :: trait {
    hash   :: fn (self: Self) -> u64
    finish :: fn () -> u64
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

=== case: 162-trait-impl ===
use std.io

Point :: plex {
    x i32
    y i32
}

impl Display for Point {
    show :: fn (self: Self) => $"Point({self.x}, {self.y})"
}

Display :: trait {
    show :: fn (self: Self) -> string
}

main :: fn () -> i32 {
    point := Point { x: 3, y: 4 }
    prn(point.show())
    return point.x + point.y
}
¬
7
¬
Point(3, 4)

¬
hir 0
module module.traits.input(traits.input)
import module.std.io(std.io)
import import.0 prn from module.core(core).decl.N: <unknown>
import import.1 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.2 prn_text from module.core(core).decl.N: fn (string) -> void
import import.3 prn_empty from module.core(core).decl.N: fn () -> void
import import.4 __trait_Display_for_c_string_show from module.core(core).decl.N: fn (^i8) -> string
bind prn = import.0
bind input = import.1
bind prn_text = import.2
bind prn_empty = import.3
bind __trait_Display_for_c_string_show = import.4
bind Point = type.0
bind __trait_Display_for_Point_show = fn.0
bind main = fn.1
type type.0 = Point
func fn.0(self: Point) -> string {
  return string interpolate(<unknown> "Point(", i32 field(Point local.0(self), x), <unknown> ", ", i32 field(Point local.0(self), y), <unknown> ")")
}
func fn.1() -> i32 {
  let point: Point = Point plex(x: i32 3, y: i32 4)
  expr void call bind.2(prn_text)(string call bind.6(__trait_Display_for_Point_show)(Point local.1(point)))
  return i32 add(i32 field(Point local.1(point), x), i32 field(Point local.1(point), y))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [32 x i8] c"tests/language/traits.t\00"
@.str.m0.0 = private unnamed_addr constant [7 x i8] c"Point(\00"
@.str.m0.1 = private unnamed_addr constant [3 x i8] c", \00"
@.str.m0.2 = private unnamed_addr constant [2 x i8] c")\00"

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

define internal { ptr, i64 } @fn.0({ i32, i32 } %self) {
  %t2 = alloca { ptr, i64 }
  %t6 = alloca { ptr, i64 }
  %t10 = alloca { ptr, i64 }
  %t0 = call i64 @nrt_string_builder_mark()
  %t1 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 6 }, ptr %t2
  call void @nrt_to_string_string(ptr %t1, ptr %t2)
  call void @nrt_string_builder_append_string(ptr %t1)
  %t3 = extractvalue { i32, i32 } %self, 0
  %t4 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t4, i32 %t3)
  call void @nrt_string_builder_append_string(ptr %t4)
  %t5 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 2 }, ptr %t6
  call void @nrt_to_string_string(ptr %t5, ptr %t6)
  call void @nrt_string_builder_append_string(ptr %t5)
  %t7 = extractvalue { i32, i32 } %self, 1
  %t8 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t8, i32 %t7)
  call void @nrt_string_builder_append_string(ptr %t8)
  %t9 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 1 }, ptr %t10
  call void @nrt_to_string_string(ptr %t9, ptr %t10)
  call void @nrt_string_builder_append_string(ptr %t9)
  %t11 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t11, i64 %t0)
  %t12 = load { ptr, i64 }, ptr %t11
  ret { ptr, i64 } %t12
}

define internal i32 @fn.1() {
  %t0 = insertvalue { i32, i32 } poison, i32 3, 0
  %t1 = insertvalue { i32, i32 } %t0, i32 4, 1
  %t2 = call { ptr, i64 } @fn.0({ i32, i32 } %t1)
  call void @$prn_text({ ptr, i64 } %t2)
  %t3 = extractvalue { i32, i32 } %t1, 0
  %t4 = extractvalue { i32, i32 } %t1, 1
  %t5 = add i32 %t3, %t4
  ret i32 %t5
}

@$__trait_Display_for_Point_show = internal alias { ptr, i64 } ({ i32, i32 }), ptr @fn.0
@$main = hidden alias i32 (), ptr @fn.1

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)

=== case: 163-std-traits ===
use std.io

Point :: plex {
    x i32
    y i32
}

impl Display for Point {
    show :: fn (self: Self) => $"Point({self.x}, {self.y})"
}

impl Eq for Point {
    eq :: fn (lhs: Self, rhs: Self) => lhs.x == rhs.x && lhs.y == rhs.y
}

impl Default for Point {
    default :: fn () -> Self {
        return Point { x: 0, y: 0 }
    }
}

main :: fn () -> i32 {
    point := Point { x: 3, y: 4 }
    origin := Point.default()
    prn(point.show())
    prn($"eq={point.eq(origin)}")
    return point.x + point.y + origin.x + origin.y
}
¬
7
¬
Point(3, 4)
eq=no

¬
hir 0
¬

=== case: 167-explicit-trait-call ===
use std.io

Named :: trait {
    name :: fn (self: Self) -> string
}

Labelled :: trait {
    name :: fn (self: Self) -> string
}

Point :: plex {
    x i32
}

impl Named for Point {
    name :: fn (self: Self) => $"named {self.x}"
}

impl Labelled for Point {
    name :: fn (self: Self) => $"label {self.x}"
}

main :: fn () -> i32 {
    point := Point { x: 7 }
    prn(Named.name(point))
    prn(Labelled.name(point))
    return point.x
}
¬
7
¬
named 7
label 7

¬
hir 0
¬

=== case: 168-explicit-trait-impl-call ===
use std.io

Default :: trait {
    default :: fn () -> Self
}

Point :: plex {
    x i32
    y i32
}

impl Default for Point {
    default :: fn () -> Self {
        return Point { x: 13, y: 21 }
    }
}

main :: fn () -> i32 {
    point := Default[Point].default()
    prn($"Point({point.x}, {point.y})")
    return point.x + point.y
}
¬
34
¬
Point(13, 21)

¬
hir 0
¬

=== case: 244-default-once ===
calls: i32 = 0
Settings :: plex { value i32 }
impl Default for Settings {
    default :: fn () -> Self { calls += 1
        return { value: 42 }
    }
}
Owned :: plex { data box[i32] }
impl Default for Owned {
    default :: fn () -> Self { calls += 1
        return { data: box[i32]() }
    }
}
main :: fn () -> i32 {
    on yes => { nested: Settings
        assert nested.value == 42 && calls == 1
    }
    for _i in [0 .. 3] { scoped: Settings
        assert scoped.value == 42
    }
    assert calls == 4
    owned: Owned
    assert calls == 5 && owned.data != nil
    owned.data.free()
    calls = 0
    settings: Settings
    prn($"{settings.value}")
    return calls
}
¬
1
¬
42
¬

¬

=== case: 250-builtin-display-constraint ===
describe :: fn [T] (value: T) -> string
where T: Display { return value.show() }
main :: fn () {
    assert describe(42) == "42"
    assert describe(yes) == "yes"
    assert describe("text") == "text"
    assert describe(1.5) == $"{1.5}"
    values := [1, 2]
    assert describe(values) == $"{values}"
    view: []i32 = values
    assert describe(view) == $"{view}"
    pair := (1, "two")
    assert describe(pair) == $"{pair}"
    dynamic: [..]i32
    dynamic.push(4)
    assert describe(dynamic) == $"{dynamic}"
    dynamic.free()
    assert describe(42.as(i8)) == "42" && describe(42.as(i16)) == "42"
    assert describe(42.as(i64)) == "42" && describe(42.as(isize)) == "42"
    assert describe(42.as(u8)) == "42" && describe(42.as(u16)) == "42"
    assert describe(42.as(u32)) == "42" && describe(42.as(u64)) == "42"
    assert describe(42.as(usize)) == "42"
    assert describe(1.5.as(f32)) == $"{1.5.as(f32)}"
    prn(describe(42))
}
¬
0
¬
42
¬

¬

=== case: 294-run-named-trait-parameters ===
Callback :: fn (arg1: i32) -> i32
Render :: trait {
    render :: fn (self: ^Self, callback: fn (arg1: i32) -> i32) -> i32
}
Thing :: plex { value i32 }
impl Render for Thing {
    render :: fn (_self: ^Self, callback: Callback) -> i32 {
        return callback(7)
    }
}
impl Thing {
    idle :: fn (_self: ^Self) {}
    read_value :: fn (self: ^Self) -> i32 { return self.value }
}
identity :: fn (value: i32) -> i32 { return value }
main :: fn () {
    thing := Thing { value: 3 }
    thing.idle()
    assert thing.render(identity) == 7
    assert thing.read_value() == 3
}
¬
0
¬

¬

¬

=== case: 307-run-imported-display-interpolation ===
values :: use display_values
main :: fn () {
    value := values.Value { count: 42 }
    assert $"{value}" == "value=42"
    failure := values.failure(5)
    assert $"{failure}" == "error=5"
}
¬
0
¬

¬

¬

=== case: 164-trait-primitive-impl ===
use std.io

Describe :: trait {
    describe :: fn (self: Self) -> string
}

impl Describe for i32 {
    describe :: fn (self: Self) => $"i32:{self}"
}

main :: fn () -> i32 {
    value: i32 = 42
    prn(value.describe())
    return value
}
¬
42
¬
i32:42

¬
hir 0
¬

=== case: 165-trait-resolution ===
use std.io

Describe :: trait {
    describe :: fn (self: Self) -> string
}

Point :: plex {
    x i32
}

impl Describe for Point {
    describe :: fn (self: Self) => $"trait {self.x}"
}

impl Point {
    describe :: fn (self: Self) => $"inherent {self.x}"
}

Colour :: enum {
    Red
    Blue
}

impl Describe for Colour {
    describe :: fn (self: Self) => on self {
        Red => "red"
        Blue => "blue"
    }
}

Payload :: union {
    i i32
    f f32
}

impl Describe for Payload {
    describe :: fn (self: Self) => $"payload {self.i}"
}

main :: fn () -> i32 {
    point := Point { x: 7 }
    colour: Colour = Blue
    payload: Payload = Payload { i: 9 }

    prn(point.describe())
    prn(colour.describe())
    prn(payload.describe())

    return point.x + payload.i
}
¬
16
¬
inherent 7
blue
payload 9

¬
hir 0
¬

=== case: 166-trait-self-alias ===
use std.io

Display :: trait for Value {
    show :: fn (self: Value) -> string
}

Default :: trait for Value {
    default :: fn () -> Value
}

Point :: plex {
    x i32
    y i32
}

impl Display for Point {
    show :: fn (self: Self) => $"Point({self.x}, {self.y})"
}

impl Default for Point {
    default :: fn () -> Self {
        return Point { x: 5, y: 8 }
    }
}

main :: fn () -> i32 {
    point := Point.default()
    prn(point.show())
    return point.x + point.y
}
¬
13
¬
Point(5, 8)

¬
hir 0
¬

=== case: 172-display-interpolation ===
use std.io

Point :: plex {
    x i32
    y i32
}

impl Display for Point {
    show :: fn (self: Self) -> string {
        return $"Point({self.x}, {self.y})"
    }
}

main :: fn () -> i32 {
    point := Point { x: 3, y: 4 }
    text := $"Here: {point}"
    prn(text)
    return text.count.as(i32)
}
¬
17
¬
Here: Point(3, 4)

¬
hir 0
¬

=== case: 174-eq-operator-trait ===
Point :: plex {
    x i32
    y i32
}

impl Eq for Point {
    eq :: fn (lhs: Self, rhs: Self) -> bool {
        return lhs.x == rhs.x && lhs.y == rhs.y
    }
}

main :: fn () -> i32 {
    a := Point { x: 3, y: 4 }
    b := Point { x: 3, y: 4 }
    c := Point { x: 3, y: 5 }
    prn($"same={a == b}")
    prn($"diff={a != c}")
    return on a == b && a != c {
        yes => 0
        else => 1
    }
}
¬
0
¬
same=yes
diff=yes

¬
hir 0
¬

=== case: 175-default-typed-binding ===
Point :: plex {
    x i32
    y i32
}

impl Default for Point {
    default :: fn () -> Self {
        return Point { x: 10, y: 20 }
    }
}

main :: fn () -> i32 {
    point: Point
    prn($"point={point.x},{point.y}")
    return point.x + point.y
}
¬
30
¬
point=10,20

¬
hir 0
¬

=== case: 176-default-zero-init-fallback ===
Point :: plex {
    x i32
    y i32
}

main :: fn () -> i32 {
    point: Point
    prn($"point={point.x},{point.y}")
    return point.x + point.y
}
¬
0
¬
point=0,0

¬
hir 0
¬

=== case: 178-order-operator-trait ===
Point :: plex {
    x i32
    y i32
}

impl Order for Point {
    compare :: fn (lhs: Self, rhs: Self) -> i32 {
        lhs_score := lhs.x + lhs.y
        rhs_score := rhs.x + rhs.y
        return lhs_score - rhs_score
    }
}

main :: fn () -> i32 {
    low := Point { x: 1, y: 2 }
    same := Point { x: 2, y: 1 }
    high := Point { x: 3, y: 4 }

    prn($"lt={low < high}")
    prn($"le={low <= same}")
    prn($"gt={high > low}")
    prn($"ge={same >= low}")

    return on low < high && low <= same && high > low && same >= low {
        yes => 0
        else => 1
    }
}
¬
0
¬
lt=yes
le=yes
gt=yes
ge=yes

¬
hir 0
¬

=== case: 179-iterator-for-in ===
Counter :: plex {
    current i32
    end i32
}

impl Iterator[i32] for Counter {
    next :: fn (self: ^Self) -> ?i32 {
        result: ?i32 = nil
        on self.current < self.end {
            yes => {
                value := self.current
                self.current += 1
                result = value
            }
            else => {}
        }
        return result
    }
}

PtrIter :: plex {
    data []i32
    index usize
}

impl Iterator[^i32] for PtrIter {
    next :: fn (self: ^Self) -> ?^i32 {
        result: ?^i32 = nil
        on self.index < self.data.count {
            yes => {
                ptr := ^self.data[self.index]
                self.index += 1
                result = ptr
            }
            else => {}
        }
        return result
    }
}

main :: fn () -> i32 {
    counter := Counter { current: 0, end: 4 }
    total := 0
    for value in counter {
        total += value
    }

    values: [3]i32 = [2, 3, 5]
    for builtin in values[..] {
        total += builtin^
    }

    iter := PtrIter { data: values[..], index: 0 }
    for value in iter {
        total += value^
    }

    prn($"total={total}")
    return total
}
¬
26
¬
total=26

¬
hir 0
¬

=== case: 284-run-bitfield-enum-display ===
TokenType :: enum {
    Identifier
}

impl Display for TokenType {
    show :: fn (self: TokenType) -> string {
        return on self {
            TokenType.Identifier => "Identifier"
        }
    }
}

Token :: plex {
    u32 {
        type TokenType : 8
        offset : 24
    }
}

main :: fn () -> i32 {
    token := Token {
        type : TokenType.Identifier
        offset : 0
    }
    text := $"{token.type}"
    on text != "Identifier" => return 1
    return 0
}
¬
0
¬

¬

¬

=== case: 277-run-unused-default-once ===
default_value :: fn (calls: ^i32) -> i32 {
    calls^ += 1
    return 9
}

select :: fn (calls: ^i32, value: i32 = default_value(calls)) -> i32 {
    return value
}

main :: fn () -> i32 {
    calls : i32 = 0
    explicit := select(^calls, value = 4)
    on explicit != 4 || calls != 0 => return 1
    implicit := select(^calls)
    return on implicit == 9 && calls == 1 { yes => 0 else => 2 }
}
¬
0
¬

¬

¬
