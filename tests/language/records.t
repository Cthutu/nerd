-- records feature scenarios. Results and stdout are checked in order.
use std.io

-- 056-plexes
Case_0_Point :: plex {
    x i32
    y i32
    name string
}

case_0_main :: fn () -> i32 {
    p: Case_0_Point = Case_0_Point { x: 3, y: 4, name: "origin" }
    q := Case_0_Point { name: "other", x: 5, y: 6 }
    prn($"p {p.name}: {p.x}, {p.y}")
    prn($"q {q.name}: {q.x}, {q.y}")
    return p.x + p.y + q.x + q.y
}

-- 058-plex-ergonomics
Case_2_Point :: plex #c {
    x    i32
    y    i32
    name string
}

Case_2_Packed :: plex #packed {
    a u8
    b i32
}

case_2_main :: fn () -> i32 {
    p := Case_2_Point { x: 1, y: 2, name: "first" }
    q := p with { y: 7, name: "second" }
    pp := ^q
    prn($"{p.name} {p.x} {p.y}")
    prn($"{pp.name} {pp.x} {pp.y}")
    packed := Case_2_Packed { a: 1, b: 2 }
    return q.x + q.y + packed.b
}

-- 062-raw-unions
Case_3_Value :: union {
    i i32
    f f32
}

case_3_main :: fn () -> i32 {
    a: Case_3_Value = Case_3_Value { i: 42 }
    b: Case_3_Value = Case_3_Value { f: 3.5 }

    prn($"i {a.i}")
    prn($"f {b.f}")

    return a.i
}

-- 079-contextual-plex-and-slice
Case_4_Point :: plex { x i32 y i32 }

case_4_origin : Case_4_Point : { x: 9, y: 8 }
case_4_points : []Case_4_Point : [
    { x: 1, y: 2 },
    { x: 3, y: 4 },
]

case_4_show_last :: fn (items: []Case_4_Point) {
    last :: items[items.count - 1]
    prn($"{last.x} {last.y}")
}

case_4_main :: fn () {
    p : Case_4_Point = { x: 5, y: 6 }
    prn($"{case_4_origin.x} {case_4_origin.y}")
    prn($"{case_4_points[0].x} {case_4_points[1].y}")
    prn($"{p.x} {p.y}")
    case_4_show_last([
        { x: 13, y: 14 },
        { x: 15, y: 16 },
    ])
}

-- 120-plex-literal-default-missing
Case_5_Point :: plex {
    x i32
    y i32
    z i32
}

case_5_main :: fn () -> i32 {
    p := Case_5_Point { x: 7 ... }
    return p.x + p.y + p.z
}

-- 137-plex-field-shorthand
-- Uses shorthand plex fields where `name` means `name: name`.
Case_7_Point :: plex {
    x i32
    y i32
}

case_7_main :: fn () -> i32 {
    x := 7
    y := 11
    point := Case_7_Point { x, y }
    other := point with { x }
    return point.x + point.y + other.x
}

-- 142-lvalue-and-plex-regressions
Case_8_Cell :: plex {
    values [4]i32
}

Case_8_Box :: plex {
    cell Case_8_Cell
}

case_8_take_cell :: fn (cell: Case_8_Cell) -> i32 {
    return cell.values[1] + cell.values[2]
}

case_8_main :: fn () -> i32 {
    box: Case_8_Box
    box.cell.values[1] = 7
    (^box.cell.values[2])^ = 9

    direct := case_8_take_cell(Case_8_Cell { values: [1, 2, 3, 4] })
    return box.cell.values[1] + box.cell.values[2] + direct
}

-- 150-qualified-plex-literal
case_9_types :: use test.lsp_types

case_9_main :: fn () -> i32 {
    point := case_9_types.Point {
        x: 3
        y: 4
    }
    return point.x + point.y
}

-- 155-plex-literal-default-missing
Case_10_State :: plex {
    visible bool
    name    string
    next    ^Case_10_State
    count   i32
}

case_10_main :: fn () -> i32 {
    state := Case_10_State { count: 7 ... }

    on state.visible => return 1
    on state.name != "" => return 2
    on state.next != nil => return 3
    return state.count
}

-- 158-local-const-typed-plex-ellipsis
Case_11_Point :: plex {
    x i32
    y i32
}

case_11_main :: fn () -> i32 {
    p :: Case_11_Point { ... }
    return p.x + p.y
}

-- 243-plex-storage-order
Case_12_Compact :: plex { first u8 middle u32 last u8 }
Case_12_COrder :: plex #c { first u8 middle u32 last u8 }
Case_12_Generic :: plex [T] { first u8 middle T last u8 }
Case_12_Embedded :: plex { head u8 use Case_12_Compact tail u8 }
Case_12_CEmbedded :: plex #c { head u8 use Case_12_Compact tail u8 }
Case_12_Kind :: enum { First Second }
Case_12_TaggedBits :: plex { before u8 u32 { kind Case_12_Kind : 8 payload : 24 } after u8 }
Case_12_UseTagged :: plex { tag u64 use Case_12_TaggedBits }
Case_12_Nested :: plex { before u8 use Case_12_Embedded after u64 }
Case_12_Blocks :: plex { first u8 u32 { low : 8 high : 24 } last u8 }
case_12_constant :: Case_12_Compact { first: 1 middle: 200 last: 3 }
case_12_global := Case_12_Compact { first: 4 middle: 500 last: 6 }
case_12_calls := 0
case_12_next :: fn () -> u32 { case_12_calls += 1 return case_12_calls.as(u32) }
case_12_copy :: fn (value: Case_12_Compact) -> Case_12_Compact { return value }
case_12_main :: fn () {
    assert Case_12_Compact.size == 8
    assert Case_12_COrder.size == 12
    assert Case_12_Embedded.size == 12
    assert Case_12_Nested.size == 24
    assert Case_12_Blocks.size == 8
    assert Case_12_TaggedBits.size == 8 && Case_12_UseTagged.size == 16
    assert Case_12_CEmbedded.size == 16
    value := Case_12_Compact { first: case_12_next().as(u8) middle: case_12_next() last: case_12_next().as(u8) }
    assert case_12_calls == 3 && value.first == 1 && value.middle == 2 && value.last == 3
    assert (^value.middle).as(usize) - (^value).as(usize) == 0
    assert (^value.first).as(usize) - (^value).as(usize) == 4
    assert (^value.last).as(usize) - (^value).as(usize) == 5
    c := Case_12_COrder { first: 7 middle: 800 last: 9 }
    assert (^c.first).as(usize) - (^c).as(usize) == 0
    assert (^c.middle).as(usize) - (^c).as(usize) == 4
    assert (^c.last).as(usize) - (^c).as(usize) == 8
    assert case_12_constant.first == 1 && case_12_constant.middle == 200 && case_12_constant.last == 3
    assert case_12_global.first == 4 && case_12_global.middle == 500 && case_12_global.last == 6
    case_12_global.middle = 501
    assert case_12_copy(case_12_global).middle == 501
    changed := value with { middle: 42 }
    assert changed.first == 1 && changed.middle == 42 && changed.last == 3
    values: [2]Case_12_Compact = [value, changed]
    assert values.size == 16 && values[1].middle == 42
    assert (^values[1]).as(usize) - (^values[0]).as(usize) == 8
    dynamic: [..]Case_12_Compact
    dynamic.push(value)
    dynamic.push(changed)
    assert dynamic[0].first == 1 && dynamic[1].middle == 42
    dynamic.free()
    nested := Case_12_Nested { before: 1 head: 2 first: 3 middle: 400 last: 5 tail: 6 after: 7 }
    base: ^Case_12_Compact = ^nested
    assert base.middle == 400 && base.first == 3 && base.last == 5
    base.middle = 401
    assert nested.middle == 401 && nested.head == 2 && nested.tail == 6 && nested.after == 7
    cembedded := Case_12_CEmbedded { head: 1 first: 2 middle: 300 last: 4 tail: 5 }
    cbase: ^Case_12_Compact = ^cembedded
    assert cbase.middle == 300 && cbase.first == 2 && cbase.last == 4
    assert cbase.as(usize) - (^cembedded).as(usize) == 4
    tagged := Case_12_UseTagged { tag: 1 before: 2 kind: Case_12_Kind.Second payload: 3 after: 4 }
    assert tagged.kind == Case_12_Kind.Second && tagged.payload == 3 && tagged.before == 2 && tagged.after == 4
    bits := Case_12_Blocks { first: 1 low: 2 high: 3 last: 4 }
    assert bits.first == 1 && bits.low == 2 && bits.high == 3 && bits.last == 4
    bits.high = 10
    assert bits.low == 2 && bits.high == 10 && bits.first == 1 && bits.last == 4
    generic: Case_12_Generic[u32] = { first: 10 middle: 20 last: 30 }
    assert generic.size == 8
    assert generic.first == 10 && generic.middle == 20 && generic.last == 30
    tuple := (1.as(u8), 2.as(u32), 3.as(u8))
    assert tuple.size == 12
    prn($"compact={Case_12_Compact.size} c={Case_12_COrder.size}")
}

-- 245-plex-destructuring
case_13_calls: i32 = 0
Case_13_Point :: plex { x i32
    y i32 }
case_13_make_point :: fn () -> Case_13_Point { case_13_calls += 1
    return { x: 20 y: 22 } }
case_13_fetch :: fn (owner: ^Case_13_Owned) -> ^Case_13_Owned { case_13_calls += 1
    return owner }
Case_13_Nested :: plex { point Case_13_Point
    pair (i32, i32) }
Case_13_Owned :: plex { data box[i32] }
case_13_main :: fn () -> i32 {
    point := case_13_make_point()
    { x, y } := point
    assert x + y == 42 && case_13_calls == 1
    { y: second, x: first } :: case_13_make_point()
    assert first + second == 42 && case_13_calls == 2
    on yes => { { x, y }: Case_13_Point = point
        assert x + y == 42
    }
    { x, y } = Case_13_Point { x: 21 y: 23 }
    assert x == 21 && y == 23
    nested := Case_13_Nested { point: point pair: (1, 2) }
    { point: { x: xx }, pair: (a, b) } := nested
    assert xx == 20 && a == 1 && b == 2
    owner := Case_13_Owned { data: box[i32]() }
    owner.data.data^ = 42
    { data } := owner
    assert owner.data == nil && data.data^ == 42
    data.free()
    owner.data = box[i32]()
    owner.data.data^ = 7
    { data: fetched } := case_13_fetch(^owner)^
    assert case_13_calls == 3 && fetched.data^ == 7 && owner.data == nil
    fetched.free()
    first_box := box[i32]()
    second_box := box[i32]()
    first_box.data^ = 1
    second_box.data^ = 2
    (first_box, second_box) = (second_box, first_box)
    assert first_box.data^ == 2 && second_box.data^ == 1
    (moved, _) := (first_box, 0)
    assert first_box == nil && moved.data^ == 2
    moved.free()
    second_box.free()
    return first + second
}

-- 173-run-nested-braced-enum-plex-field
Case_14_Mode :: enum {
    Fixed {
        width u16
        height u16
    }
}

Case_14_Layer :: plex {
    mode Case_14_Mode
    width u16
    height u16
}

case_14_main :: fn () {
    layer := Case_14_Layer {
        mode   : Case_14_Mode.Fixed { width: 1 height: 2 }
        width  : 3
        height : 4
    }

    prn($"{layer.width}x{layer.height}")
}

-- 261-run-typed-plex-shorthand-before-ellipsis
Case_15_Item :: plex {
    output i32
    extra  i32
}

case_15_main :: fn () -> i32 {
    output := 42
    item := Case_15_Item { output ... }
    return on item.output == 42 && item.extra == 0 => 0 else 1
}

-- 288-run-usage-context-inference
case_16_single :: fn (value: f32) -> f32 { return value }
case_16_double :: fn (value: f64) -> f64 { return value }
case_16_pair :: fn (value: (f32, f64)) -> f32 { return value.0 }
case_16_pointed :: fn (value: ^f32) -> f32 { return value^ }

case_16_returned :: fn () -> f32 {
    values := (0.25, 0.5)
    return values.0
}

case_16_with_parameter :: fn (amount: f32) -> f32 {
    initial := 0.25
    alias := initial
    output := alias + amount
    return output
}

case_16_main :: fn () {
    scalar := 0.25
    assert case_16_single(scalar) == 0.25
    assert scalar.size == f32.size

    colour := (0.25, 0.5, 0.75, 1.0)
    alias := colour.0
    assert case_16_single(alias) == 0.25
    assert case_16_double(colour.1) == 0.5
    assert case_16_single(colour.2 + 0.25) == 1.0
    assert colour.0.size == f32.size
    assert colour.1.size == f64.size
    assert colour.2.size == f32.size
    assert colour.3.size == f64.size
    colour = (0.5, 0.75, 0.25, 1.0)
    assert case_16_single(colour.0) == 0.5

    nested := ((0.25, 0.5), 0.75)
    assert case_16_single((nested.0).1) == 0.5
    assert (nested.0).1.size == f32.size
    assert (nested.0).0.size == f64.size

    whole := (0.25, 0.5)
    assert case_16_pair(whole) == 0.25
    assert whole.0.size == f32.size
    assert whole.1.size == f64.size

    assigned := 0.25
    destination : f32
    destination = assigned
    assert destination == 0.25
    assert assigned.size == f32.size
    assert case_16_returned() == 0.25
    assert case_16_with_parameter(0.5) == 0.75

    mixed := (yes, 0.25, "colour")
    assert case_16_single(mixed.1) == 0.25
    assert mixed.0
    assert mixed.2 == "colour"

    integer := 42
    wide: u64 = integer
    assert wide == 42
    assert integer.size == u64.size

    addressed := 0.5
    assert case_16_pointed(^addressed) == 0.5
    assert addressed.size == f32.size

    casted := 0.25
    assert case_16_single(casted.as(f32)) == 0.25
    assert casted.size == f64.size

    compared := 0.5
    reference: f32 = 0.5
    assert compared == reference
    assert compared.size == f32.size
}

main :: fn () -> i32 {
    prn("begin 056-plexes")
    {
        result := case_0_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 18
    }
    prn("begin 058-plex-ergonomics")
    {
        result := case_2_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 10
    }
    prn("begin 062-raw-unions")
    {
        result := case_3_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 42
    }
    prn("begin 079-contextual-plex-and-slice")
    {
        case_4_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 120-plex-literal-default-missing")
    {
        result := case_5_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 7
    }
    prn("begin 137-plex-field-shorthand")
    {
        result := case_7_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 25
    }
    prn("begin 142-lvalue-and-plex-regressions")
    {
        result := case_8_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 21
    }
    prn("begin 150-qualified-plex-literal")
    {
        result := case_9_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 7
    }
    prn("begin 155-plex-literal-default-missing")
    {
        result := case_10_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 7
    }
    prn("begin 158-local-const-typed-plex-ellipsis")
    {
        result := case_11_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 243-plex-storage-order")
    {
        case_12_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 245-plex-destructuring")
    {
        result := case_13_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 42
    }
    prn("begin 173-run-nested-braced-enum-plex-field")
    {
        case_14_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 261-run-typed-plex-shorthand-before-ellipsis")
    {
        result := case_15_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 288-run-usage-context-inference")
    {
        case_16_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    return 0
}
¬
0
¬
begin 056-plexes
p origin: 3, 4
q other: 5, 6
result 18
begin 058-plex-ergonomics
first 1 2
second 1 7
result 10
begin 062-raw-unions
i 42
f 3.5
result 42
begin 079-contextual-plex-and-slice
9 8
1 4
5 6
15 16
result 0
begin 120-plex-literal-default-missing
result 7
begin 137-plex-field-shorthand
result 25
begin 142-lvalue-and-plex-regressions
result 21
begin 150-qualified-plex-literal
result 7
begin 155-plex-literal-default-missing
result 7
begin 158-local-const-typed-plex-ellipsis
result 0
begin 243-plex-storage-order
compact=8 c=12
result 0
begin 245-plex-destructuring
result 42
begin 173-run-nested-braced-enum-plex-field
3x4
result 0
begin 261-run-typed-plex-shorthand-before-ellipsis
result 0
begin 288-run-usage-context-inference
result 0

¬

¬


=== case: 057-plex-newline-fields ===
use std.io

Person :: plex { name string age u8 }

matt :: Person {
name: "Matt"
          age: 53
}

main :: fn () {
    prn($"His name is {matt.name} and he is {matt.age} years old")
}
¬
0
¬
His name is Matt and he is 53 years old

¬
hir 0
module module.records.input(records.input)
import module.std.io(std.io)
import import.0 prn from module.core(core).decl.N: <unknown>
import import.1 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.2 prn_text from module.core(core).decl.N: fn (string) -> void
import import.3 prn_empty from module.core(core).decl.N: fn () -> void
bind prn = import.0
bind input = import.1
bind prn_text = import.2
bind prn_empty = import.3
bind Person = type.0
bind matt = value.0
bind main = fn.0
type type.0 = Person
const value.0: Person = Person plex(name: string "Matt", age: u8 53)
func fn.0() -> void {
  expr void call bind.2(prn_text)(string interpolate(<unknown> "His name is ", string field(Person bind.5(matt), name), <unknown> " and he is ", u8 field(Person bind.5(matt), age), <unknown> " years old"))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [41 x i8] c"tests/language/records.t\00"
@.str.m0.0 = private unnamed_addr constant [5 x i8] c"Matt\00"
@.str.m0.1 = private unnamed_addr constant [13 x i8] c"His name is \00"
@.str.m0.2 = private unnamed_addr constant [12 x i8] c" and he is \00"
@.str.m0.3 = private unnamed_addr constant [11 x i8] c" years old\00"

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

define internal void @fn.0() {
  %t2 = alloca { ptr, i64 }
  %t7 = alloca { ptr, i64 }
  %t9 = alloca { ptr, i64 }
  %t15 = alloca { ptr, i64 }
  %t0 = call i64 @nrt_string_builder_mark()
  %t1 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 12 }, ptr %t2
  call void @nrt_to_string_string(ptr %t1, ptr %t2)
  call void @nrt_string_builder_append_string(ptr %t1)
  %t3 = insertvalue { { ptr, i64 }, i8 } poison, { ptr, i64 } { ptr @.str.m0.0, i64 4 }, 0
  %t4 = insertvalue { { ptr, i64 }, i8 } %t3, i8 53, 1
  %t5 = extractvalue { { ptr, i64 }, i8 } %t4, 0
  %t6 = alloca { ptr, i64 }
  store { ptr, i64 } %t5, ptr %t7
  call void @nrt_to_string_string(ptr %t6, ptr %t7)
  call void @nrt_string_builder_append_string(ptr %t6)
  %t8 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 11 }, ptr %t9
  call void @nrt_to_string_string(ptr %t8, ptr %t9)
  call void @nrt_string_builder_append_string(ptr %t8)
  %t10 = insertvalue { { ptr, i64 }, i8 } poison, { ptr, i64 } { ptr @.str.m0.0, i64 4 }, 0
  %t11 = insertvalue { { ptr, i64 }, i8 } %t10, i8 53, 1
  %t12 = extractvalue { { ptr, i64 }, i8 } %t11, 1
  %t13 = alloca { ptr, i64 }
  call void @nrt_to_string_u8(ptr %t13, i8 zeroext %t12)
  call void @nrt_string_builder_append_string(ptr %t13)
  %t14 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 10 }, ptr %t15
  call void @nrt_to_string_string(ptr %t14, ptr %t15)
  call void @nrt_string_builder_append_string(ptr %t14)
  %t16 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t16, i64 %t0)
  %t17 = load { ptr, i64 }, ptr %t16
  call void @$prn_text({ ptr, i64 } %t17)
  ret void
}

@$main = hidden alias void (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)

=== case: 123-mutual-pointer-plex-types ===
Left :: plex {
    value i32
    right ^Right
}

Right :: plex {
    value i32
    left  ^Left
}

left: Left = { value: 1, ... }
right: Right = { value: 2, left: ^left }

main :: fn () -> i32 {
    left.right = ^right
    return left.right.value + right.left.value
}
¬
3
¬

¬
hir 0
bind Left = type.0
bind Right = type.1
bind left = value.0
bind right = value.1
bind main = fn.0
type type.0 = Left
type type.1 = Right
global value.0: Left = Left plex(value: i32 1, ...)
global value.1: Right = Right plex(value: i32 2, left: ^Left address_of(Left bind.2(left)))
func fn.0() -> i32 {
  assign ^Right field(Left bind.2(left), right) = ^Right address_of(Right bind.3(right))
  return i32 add(i32 field(^Right field(Left bind.2(left), right), value), i32 field(^Left field(Right bind.3(right), left), value))
}
¬
; nerd llvm-ir 0
; generated from HIR

@$left = internal global { ptr, i32 } zeroinitializer
@$right = internal global { ptr, i32 } zeroinitializer

define void @m0.init() {
  %t0 = insertvalue { ptr, i32 } poison, i32 1, 1
  %t1 = insertvalue { ptr, i32 } %t0, ptr null, 0
  store { ptr, i32 } %t1, ptr @$left
  %t2 = insertvalue { ptr, i32 } poison, i32 2, 1
  %t3 = insertvalue { ptr, i32 } %t2, ptr @$left, 0
  store { ptr, i32 } %t3, ptr @$right
  ret void
}

define internal i32 @fn.0() {
  %t0 = getelementptr inbounds { ptr, i32 }, ptr @$left, i64 0, i32 0
  store ptr @$right, ptr %t0
  %t1 = load { ptr, i32 }, ptr @$left
  %t2 = extractvalue { ptr, i32 } %t1, 0
  %t3 = load { ptr, i32 }, ptr %t2
  %t4 = extractvalue { ptr, i32 } %t3, 1
  %t5 = load { ptr, i32 }, ptr @$right
  %t6 = extractvalue { ptr, i32 } %t5, 0
  %t7 = load { ptr, i32 }, ptr %t6
  %t8 = extractvalue { ptr, i32 } %t7, 1
  %t9 = add i32 %t4, %t8
  ret i32 %t9
}

@$main = hidden alias i32 (), ptr @fn.0

=== case: 068-run-llvm-dynarray-plex-default ===
Info :: plex {
    handle u64
}

System :: plex {
    infos [..]Info
}

main :: fn () -> i32 {
    infos: [..]Info
    system := System { infos }
    _ := system
    return 0
}
¬
0
¬

¬

¬

=== case: 096-run-llvm-returned-plex-slice-literals ===
State :: plex {
    exits [][](string, usize)
    player_loc usize
}

init :: fn () -> State {
    return {
        exits      : [
            [
                ("E", 1),
                ("W", 4),
            ],
            [("S", 2)],
        ]
        player_loc : 0
    }
}

main :: fn () -> i32 {
    state := init()

    on state.exits.count != 2 => return 1
    on state.exits[state.player_loc].count != 2 => return 2
    on state.exits[state.player_loc][0].0 != "E" => return 3
    on state.exits[state.player_loc][1].1 != 4 => return 4

    total := 0
    for _, exit in state.exits[state.player_loc] {
        total += exit.1.as(i32)
    }

    on total != 5 => return 5
    return 0
}
¬
0
¬

¬

¬

=== case: 139-run-llvm-dynarray-padded-plex-resize ===
Tile :: enum { Wall Floor }

Cell :: plex {
    tile   Tile
    region i32
}

cells : [..]Cell

main :: fn () -> i32 {
    cells.resize_to(697)
    on cells.count != 697 => return 1

    cells[696] = Cell { tile: Tile.Floor region: 42 }
    on cells[696].tile != Tile.Floor => return 2
    on cells[696].region != 42 => return 3

    cells.free()
    return 0
}
¬
0
¬

¬

¬

=== case: 169-run-enum-braced-plex-payload ===
use std.io

Event :: enum {
    Closed
    Resized {
        width u16
        height u16
    }
}

score :: fn (event: Event) -> i32 {
    return on event {
        Resized { width } => width.as(i32)
        Closed => 0
    }
}

main :: fn () -> i32 {
    event := Event.Resized { width: 80, height: 25 }
    prn($"{score(event)}")
    return 0
}
¬
0
¬
80

¬

¬

=== case: 292-run-plex-usage-context ===
State :: plex {
    background_colour (f32, f32, f32)
}
Outer :: plex { state State }
Value :: union { number f32 bits u32 }
returned :: fn () -> State {
    value := { background_colour: (0.25, 0.5, 0.75) }
    return value
}
main :: fn () {
    draw_state := { background_colour: (0.25, 0.5, 0.75) }
    assert draw(^draw_state) == 0.25
    assert draw_state.background_colour.0.size == f32.size

    red := 0.5
    initial := { background_colour: (red, 0.25, 0.75) }
    alias := initial
    pointer := ^alias
    assert draw(pointer) == 0.5
    assert red.size == f32.size

    nested := { state: { background_colour: (0.75, 0.5, 0.25) } }
    assert outer(nested) == 0.75

    assigned := { background_colour: (0.25, 0.5, 0.75) }
    destination: State
    destination = assigned
    assert draw(^destination) == 0.25
    result := returned()
    assert draw(^result) == 0.25

    union_value := { number: 0.5 }
    assert read_union(^union_value) == 0.5
}
draw :: fn (state: ^State) -> f32 { return state.background_colour.0 }
outer :: fn (value: Outer) -> f32 { return value.state.background_colour.0 }
read_union :: fn (value: ^Value) -> f32 { return value.number }
¬
0
¬

¬

¬

=== case: 319-address-of-plex-literal ===
Info :: plex { value i32 other i32 }
CreateInfo :: plex { info ^Info }

read :: fn (info: ^Info) -> i32 { return info.value }

main :: fn () {
    pointer := ^Info { value: 7, ... }
    assert pointer.value == 7
    assert pointer.other == 0
    pointer.value = 9
    assert read(pointer) == 9

    create := CreateInfo { info: ^Info { value: 11, ... } }
    assert create.info.value == 11
    create.info.other = 3
    assert create.info.other == 3

    contextual := CreateInfo { info: ^{ value: 13, ... } }
    assert contextual.info.value == 13
    assert read(^{ value: 17, ... }) == 17
    grouped := ^(Info { value: 19, ... })
    assert grouped.value == 19

    calls := 0
    first := ^Info { value: calls += 1, ... }
    second := ^Info { value: calls += 1, ... }
    assert calls == 2
    assert first.value == 1
    assert second.value == 2
    assert first != second
}
¬
0
¬

¬

¬

=== case: 135-run-llvm-addressed-local-plex-literal ===
use std.io

Buffer :: plex {
    bytes []u8
    count usize
    ok bool
}

make :: fn (capacity: usize) -> Buffer {
    return Buffer { bytes: temp_arena.alloc_bytes(capacity), count: 0, ok: yes }
}

push :: fn (buffer: ^Buffer, text: string) {
    buffer^.count += text.count
}

main :: fn () -> i32 {
    output := make(16)

    on output.ok => {
        push(^output, "hello")
    }

    prn($"count={output.count}")
    return output.count.as(i32)
}
¬
5
¬
count=5

¬

¬


=== case: 196-plex-use ===
Point :: plex {
    x i32
    y i32
}

Rect :: plex {
    colour u8
    use Point
    width  i32
    height i32
}

Pair :: plex [T] {
    first  T
    second T
}

IntPair :: plex {
    use Pair[i32]
    label u8
}

Shape :: plex {
    tag u8
    use Rect
}

Aligned :: plex {
    small u8
    wide  u64
}

Container :: plex {
    prefix u8
    use Aligned
    suffix u8
}

initial_container :: Container { prefix: 1 small: 2 wide: 3 suffix: 4 }

move_point :: fn (point: ^Point) {
    point.x += 1
    point.y += 2
}

set_wide :: fn (aligned: ^Aligned) {
    aligned.wide = 10
}

read_wide :: fn (container: Container) -> i32 {
    return on container {
        { wide: wide } => wide.as(i32)
        else => 0
    }
}

main :: fn () -> i32 {
    rect := Rect {
        colour: 1
        x     : 10
        y     : 20
        width : 30
        height: 40
    }
    pair := IntPair { first: 2 second: 3 label: 4 }
    shape := Shape {
        tag   : 0
        colour: 1
        x     : 10
        y     : 20
        width : 30
        height: 40
    }
    move_point((^shape).as(^Point))
    container := Container { prefix: 1 small: 2 wide: 3 suffix: 4 }
    set_wide((^container).as(^Aligned))
    updated := container with { wide: 11 }
    return rect.x + rect.y + rect.width + rect.height + rect.colour.as(i32) +
           pair.first + pair.second + pair.label.as(i32) + shape.x + shape.y +
           container.wide.as(i32) + read_wide(updated) +
           initial_container.suffix.as(i32)
}
¬
168
¬

¬

¬


=== case: 204-plex-use-conversions ===
imported :: use test.imported_plex
Point :: plex { x f32 y f32 }
Rect :: plex { prefix u8 use Point w f32 h f32 }
Layer :: plex { before u64 use Rect }
Outer :: plex { before_outer u8 use Layer }
Top :: plex { tag u64 use Outer }
Pair :: plex [T] { first T second T }
IntPair :: plex { prefix u8 use Pair[i32] }
Bits :: plex { u8 { small: 3 large: 5 } }
BitContainer :: plex { prefix u64 use Bits tail u8 }
External :: plex { prefix u8 use imported.Box }
initial_rect :: Rect { prefix: 1 x: 2.0 y: 3.0 w: 4.0 h: 5.0 }
initial_point: Point = initial_rect
calls : i32
make_rect :: fn () -> Rect {
    calls += 1
    return Rect { prefix: 7 x: 10.0 y: 20.0 w: 30.0 h: 40.0 }
}
point_value :: fn (r: Rect) -> Point { return r }
point_pointer :: fn (r: ^Rect) -> ^Point { return r }
accept_point :: fn (p: Point) -> f32 { return p.x + p.y }
move_point :: fn (p: ^Point) { p.x += 1.0 p.y += 2.0 }
base_choice :: fn (_p: ^Point) -> i32 { return 1 }
rect_choice :: fn (_r: ^Rect) -> i32 { return 2 }
choice :: fn { base_choice rect_choice }
choice_reverse :: fn { rect_choice base_choice }
wrapped_choice :: fn (_p: ?Point) -> i32 { return 1 }
value_choice :: fn (_r: Rect) -> i32 { return 2 }
choice_wrapped :: fn { wrapped_choice value_choice }
optional_value :: fn (r: Rect) -> ?Point { return r }
result_value :: fn (r: Rect) -> Point\i32 { return r }
main :: fn () -> i32 {
    assert initial_point.x == 2.0 && initial_point.y == 3.0
    r := make_rect()
    p: Point = r
    p.x = 99.0
    assert r.x == 10.0
    assert accept_point(r) == 30.0
    p = r
    assert p.x == 10.0 && p.y == 20.0
    pp: ^Point = ^r
    move_point(pp)
    move_point(^r)
    pp = ^r
    assert r.x == 12.0 && r.y == 24.0 && r.prefix == 7 && r.w == 30.0
    assert point_value(r).x == 12.0
    assert point_pointer(^r) == pp
    assert r.as(Point).y == 24.0
    assert (^r).as(^Point) == pp
    assert choice(^r) == 2 && choice_reverse(^r) == 2
    assert choice(pp) == 1
    assert choice_wrapped(r) == 2
    optional_copy := optional_value(r) else { return 8 }
    result_copy := result_value(r) else { return 9 }
    assert optional_copy.x == 12.0 && result_copy.y == 24.0
    copy: Point = make_rect()
    assert calls == 2 && copy.x == 10.0
    top := Top { tag: 1 before_outer: 2 before: 3 prefix: 4
                 x: 5.0 y: 6.0 w: 7.0 h: 8.0 }
    move_point(^top)
    from_top: Point = top
    assert from_top.x == 6.0 && from_top.y == 8.0
    assert (^top).as(^Point).x == 6.0
    null_rect: ^Rect = nil
    null_point: ^Point = null_rect
    assert null_point == nil && null_rect.as(^Point) == nil
    pair := IntPair { prefix: 3 first: 4 second: 5 }
    base_pair: Pair[i32] = pair
    base_ptr: ^Pair[i32] = ^pair
    base_ptr.first = 6
    assert base_pair.first == 4 && pair.first == 6
    bit_container := BitContainer { prefix: 7 small: 3 large: 12 tail: 9 }
    bits: Bits = bit_container
    bit_ptr: ^Bits = ^bit_container
    bit_ptr.small = 4
    assert bits.small == 3 && bit_container.small == 4 && bit_container.tail == 9
    ext := External { prefix: 1 value: 23 }
    box: imported.Box = ext
    box_ptr: ^imported.Box = ^ext
    box_ptr.value = 24
    assert box.value == 23 && ext.value == 24
    indirect: fn (p: ^Point) = move_point
    indirect(^r)
    assert r.x == 13.0
    return 0
}
¬
0
¬

¬

¬


=== case: 281-run-plex-bit-fields ===
VERSION_BITS :: 4
Octet        :: u8

Header :: plex {
    Octet {
        version : VERSION_BITS
        kind TokenType : 4
    }

    u16 {
        status TokenType : 3
        active           : 1
        code             : 4
        _                : 8
    }

    length u16
}

TokenType :: enum {
    Eof
    Integer
    Float
    String
    Symbol
}

get_header :: fn (header: ^Header, calls: ^i32) -> ^Header {
    calls^ += 1
    return header
}

main :: fn () -> i32 {
    header := Header {
        version : 2
        kind    : TokenType.String
        status  : TokenType.Integer
        active  : 1
        code    : 10
        length  : 513
    }

    on header.version != 2 => return 1
    on header.kind != TokenType.String => return 2
    on header.status != TokenType.Integer => return 23
    on header.active != 1 => return 3
    on header.code != 10 => return 4
    on header.length != 513 => return 5
    on Header.size != 6 => return 6

    header.version = 15
    header.kind = TokenType.Symbol
    on header.version != 15 => return 7
    on header.kind != TokenType.Symbol => return 8
    on header.active != 1 => return 9

    pointer := ^header
    pointer.code = 3
    on pointer.code != 3 => return 10
    on pointer.active != 1 => return 11
    pointer.status = TokenType.Symbol
    on header.status != TokenType.Symbol => return 24

    calls: i32 = 0
    get_header(^header, ^calls).code = 5
    on calls != 1 => return 26
    on header.code != 5 => return 27
    on header.status != TokenType.Symbol => return 28
    on header.active != 1 => return 29
    pointer.code = 3
    on header.code != 3 => return 30

    header.kind = TokenType.Float
    on header.kind != TokenType.Float => return 12
    on header.version != 15 => return 13

    updated := header with { version: 6 }
    on updated.version != 6 => return 14
    on updated.kind != TokenType.Float => return 15
    on updated.code != 3 => return 16
    on updated.length != 513 => return 17

    defaulted := Header { length: 7 ... }
    on defaulted.version != 0 => return 18
    on defaulted.kind != TokenType.Eof => return 19
    on defaulted.status != TokenType.Eof => return 25
    on defaulted.active != 0 => return 20
    on defaulted.code != 0 => return 21
    on defaulted.length != 7 => return 22
    prn("bit-field writes complete")
    return 0
}
¬
0
¬
bit-field writes complete

¬

¬


=== case: 354-run-conditional-aggregate-reads ===
-- test-release
-- Field/index reads materialise aggregate storage even without explicit ^.
-- A skipped first use must not leave later branches reading an empty slot.
Record :: plex { value u64 padding [128]u64 }
Failure :: enum { Invalid }

record_value :: fn (record: Record, first: bool) -> u64\Failure {
    on first && record.value == 99 => return Failure.Invalid!
    return record.value
}

tuple_value :: fn (tuple: ([128]u64, u64), first: bool) -> u64 {
    on first => return tuple.1
    return tuple.1
}

array_value :: fn (array: [128]u64, first: bool) -> u64 {
    on first => return array[0]
    return array[1]
}

local_value :: fn (first: bool) -> u64 {
    record := Record { value: 37 ... }
    on first && record.value == 99 => return 0
    return record.value
}

slice_value :: fn (first: bool) -> u8 {
    bytes: [4]u8 = [5, 6, 7, 8]
    on first => { assert bytes[..][0] == 5 }
    return bytes[..][1]
}

increment :: fn (bytes: []u8) { bytes[0] = bytes[0] + 1 }

loop_slice :: fn () -> u8 {
    bytes: [4]u8
    for _ in [0 .. 2] { increment(bytes[..]) }
    return bytes[0]
}

run :: fn () -> void\Failure {
    record := Record { value: 42 ... }
    assert record_value(record, no)? == 42
    assert record_value(record, yes)? == 42
    padding: [128]u64
    tuple: ([128]u64, u64) = (padding, 11)
    assert tuple_value(tuple, no) == 11
    assert tuple_value(tuple, yes) == 11
    array: [128]u64
    array[0] = 13
    array[1] = 17
    assert array_value(array, no) == 17
    assert array_value(array, yes) == 13
    assert local_value(no) == 37
    assert local_value(yes) == 37
    assert slice_value(no) == 6
    assert slice_value(yes) == 6
    assert loop_slice() == 2
}

main :: fn () -> i32 {
    on run() { _! => return 1 else => return 0 }
}
¬
0
¬

¬

¬

=== case: anonymous-record-members ===
use test.anonymous_records

Overlay :: union { scalar u64 plex #c { low u32 high u32 } }
OtherViews :: union { plex { first u32 second u32 } plex { short_a u16 short_b u16 } }
Nested :: plex { plex { union { x u32 y u32 } } tail u8 }
GroupedC :: plex { plex #c { a u8 b u32 } c u8 }
GroupedN :: plex { plex { a u8 b u32 } c u8 }
Generic :: plex [T] { prefix u8 plex { value T } }
Named :: plex { inner plex { first u8 second u32 } tail u8 }
Bits :: plex { union { raw u8 plex { u8 { low : 4 high : 4 } } } }
saved :: Overlay { low: 16843009 high: 33686018 }
read_copy :: fn (hash: Hash) -> u32 { return hash.e }
change :: fn (hash: ^Hash) { hash.c = 50529027 }
optional :: fn (hash: Hash) -> ?Hash { return hash }
main :: fn () {
    hash: Hash
    hash.a = 16843009
    hash.e = 33686018
    assert Hash.size == 20
    assert hash.bytes[0] == 1 && hash.bytes[16] == 2
    change(^hash)
    assert hash.bytes[8] == 3
    constructed := make()
    assert constructed.a == 16843009 && constructed.e == 33686018
    clone := copy(hash)
    assert read_copy(clone) == 33686018
    on optional(hash) => [some] { assert some.c == 50529027 }
    other := Hash { bytes: [1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5] }
    assert other.e == 84215045
    envelope := Envelope { prefix: 7 a: 16843009 b: 33686018 suffix: 9 ... }
    assert Envelope.size == 28
    assert envelope.bytes[0] == 1 && envelope.bytes[4] == 2
    assert envelope.prefix == 7 && envelope.suffix == 9
    assert copy(envelope).a == 16843009
    overlay := Overlay { low: 16843009 high: 33686018 }
    assert saved.low == overlay.low && saved.high == overlay.high
    overlay.low = 67372036
    assert overlay.high == 33686018
    views := OtherViews { first: 16843009 second: 33686018 }
    assert views.short_a == 257 && views.short_b == 257
    views.short_b = 514
    assert views.second == 33686018
    nested := Nested { x: 42 tail: 3 }
    assert nested.y == 42 && nested.tail == 3
    g1: GroupedC
    g2: GroupedN
    assert (^g1.b).as(usize) - (^g1.a).as(usize) == 4
    assert (^g2.a).as(usize) - (^g2.b).as(usize) == 4
    generic: Generic[i32] = { prefix: 1 value: 42 }
    assert generic.value == 42 && generic.prefix == 1
    named := Named { inner: { first: 7 second: 42 } tail: 3 }
    assert named.inner.second == 42 && named.tail == 3
    array: [2]Hash
    array[1] = hash
    assert array[1].e == 33686018 && array[0].a == 0
    bits: Bits
    bits.low = 3
    bits.high = 12
    assert bits.raw == 195 && bits.low == 3 && bits.high == 12
    prn($"anonymous records {Hash.size} {Envelope.size} {clone.a} {overlay.high} {nested.y}")
}
¬
0
¬
anonymous records 20 28 16843009 33686018 42
¬

¬
