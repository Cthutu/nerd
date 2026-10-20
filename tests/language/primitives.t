-- primitives feature scenarios. Results and stdout are checked in order.
use std.io

-- 001-number
-- Returns a single integer literal from main.
case_0_main :: fn () => 42

-- 002-add-2-numbers
-- Adds two integer literals in an expression-bodied main.
case_1_main :: fn () => 123 + 44

-- 003-precedence
-- Checks operator precedence between addition and multiplication.
case_2_main :: fn () => 1+2*3

-- 004-unary-and-grouping
-- Checks unary minus combined with grouped subtraction.
case_3_main :: fn () => -(2-5)

-- 005-muldivmod-assoc
-- Checks left-associative division and modulo evaluation.
case_4_main :: fn () => 20/3%4

-- 031-primitive-operators
-- Checks float literals plus comparison, logical, bitwise, and modulo operators.
case_5_half: f32 = 0.5
case_5_limit: f64 = 2.0

case_5_main :: fn () -> i32 {
    mask: u32 = 14
    ordered := 3 < 4 && 4 <= 4 && 5 > 4 && 5 >= 5
    prn(on (1.5 < case_5_limit && !no) => "float" else "bad")
    prn(on ordered => "cmp" else "bad")
    prn(on (5 % 2 == 1) => "mod" else "bad")
    prn(on (((mask & 11) ^ 3) == 9 || no) => "bits" else "bad")
    return on (case_5_half <= 0.5 && 3 != 4) => 1 else 0
}

-- 032-unannotated-float-literal
-- Compiles an unannotated top-level float literal as f64 storage.
case_6_f :: 3.14

case_6_main :: fn () => 0

-- 075-packed-integer-literals
case_7_a :: 'a'
case_7_ab :: 'ab'
case_7_abc :: 'abc'
case_7_abcde :: 'abcde'

case_7_main :: fn () -> i32 {
    ok := case_7_a == 97 && case_7_ab == 24930 && case_7_abc == 6382179 && case_7_abcde == 418262508645
    upper := on 'Q' {
        'A' ..= 'Z' => yes
        else => no
    }
    lower := on 'q' {
        'A' ..= 'Z' => yes
        else => no
    }

    return on ok && upper && !lower => 0 else 1
}

-- 105-float-integer-casts
-- Casts between i32 and f32, including float-to-integer truncation toward zero.
case_8_main :: fn () -> i32 {
    up: f32 = 42.as(f32)
    down: f32 = 3.9
    below: f32 = -3.9

    whole := down.as(i32)
    negative := below.as(i32)
    return up.as(i32) + whole + negative
}

-- 106-prefixed-integer-literals
-- Decodes hexadecimal, binary, and octal integer literal prefixes.
case_9_main :: fn () => 0x10 + 0b11 + 0o7

-- 108-size-operator
case_10_helper :: fn () -> i32 {
    return 0
}

case_10_main :: fn () -> i32 {
    nums: [3]i32 = [1, 2, 3]
    view: []i32 = nums[..]
    text :: "hi"
    ptr: ^i32 = nil
    i32_size := i32.size
    literal_size := 128.size
    array_size := nums.size
    slice_size := view.size
    string_size := text.size
    ptr_size := ptr.size
    nil_size := nil.size
    fn_size := case_10_helper.size
    void_size := void.size
    prn($"i32={i32_size} literal={literal_size} array={array_size} slice={slice_size} string={string_size} ptr={ptr_size} nil={nil_size} fn={fn_size} void={void_size}")
    total := i32_size + literal_size + array_size + slice_size + string_size +
        ptr_size + nil_size + fn_size + void_size
    return total.as(i32)
}

-- 112-integer-separators
-- Decodes underscores in integer literals as digit separators.
case_11_main :: fn () => 1_00 + 0b10_10 + 0o7_5 + 0x1_0

-- 114-shift-operators
-- Supports shift operators and compound shift assignment.
case_12_main :: fn () -> i32 {
    x := 1
    y := 64
    x <<= 3
    y >>= 2
    return x + y + (2 << 4) + (128 >> 5)
}

-- 115-float-separators
-- Decodes underscores in float literals as digit separators.
case_13_main :: fn () -> i32 {
    value := 1_0.5 + 3_1.5
    return value.as(i32)
}

-- 139-size-field-name
Case_14_Thing :: plex {
    size (i32, i32)
}

case_14_main :: fn () -> i32 {
    thing: Case_14_Thing
    thing.size = (7, 5)
    return thing.size.0 + thing.size.1
}

-- 153-unsigned-byte-comparisons
case_15_inner :: fn (a: i32, b: i32, c: i32) -> i32 {
    return a + b + c
}

case_15_outer :: fn (a: i32, b: i32) -> i32 {
    return a * b
}

case_15_main :: fn () -> i32 {
    value := 65.as(u8)
    high := 0x80.as(u8)

    prn(on value < high => "lt" else "bad")
    prn(on high > value => "gt" else "bad")
    prn($"nested={case_15_outer(case_15_inner(1, 2, 3), 4)}")
    return on value <= value && high >= value => 0 else 1
}

-- 241-untyped-integer-float-context
-- Untyped integer bindings retain their type and adopt float contexts per use.
case_16_imported :: use tests.mods.untyped_float_context
case_16_answer :: 42
case_16_alias :: case_16_answer
case_16_float_answer : f32 : case_16_alias
case_16_double_answer : f64 : case_16_answer
case_16_negative_answer : f32 : -case_16_answer
case_16_literal_answer : f64 : 42
case_16_sum_answer : f32 : case_16_answer + 1
case_16_large_answer : f64 : 4_294_967_296
case_16_global_answer : f32 = case_16_answer

case_16_identity :: fn (value: f32) -> f32 { return value }
case_16_from_binding :: fn () -> f64 { return case_16_alias }

case_16_main :: fn () {
    local : f32 = case_16_answer
    assert local == 42.0
    assert case_16_float_answer == 42.0
    assert case_16_double_answer == 42.0
    assert case_16_negative_answer == -42.0
    assert case_16_literal_answer == 42.0
    assert case_16_sum_answer == 43.0
    assert case_16_large_answer == 4_294_967_296.0
    assert case_16_global_answer == 42.0
    assert case_16_identity(case_16_imported.answer) == 42.0
    assert case_16_identity(case_16_answer) == 42.0
    assert case_16_identity(42) == 42.0
    assert case_16_from_binding() == 42.0
    assert case_16_float_answer + case_16_answer == 84.0
    assert case_16_answer + case_16_double_answer == 84.0
    assert case_16_answer == case_16_float_answer
    assert case_16_double_answer == case_16_answer
    assert case_16_answer < case_16_double_answer + 1.0
    assert case_16_double_answer + 1.0 > case_16_answer
    assert case_16_answer + 1 == 43
    prn("untyped integer float contexts passed")
}

-- 252-scientific-floats
case_17_identity :: fn [T] (value: T) -> T { return value }
case_17_main :: fn () {
    value: f64 = 1e3
    small: f64 = 1e-3
    single: f32 = 1E+3
    assert value == 1000.0 && single == 1000.0
    assert small > 0.0009 && small < 0.0011
    assert 1.0e3 == value
    assert case_17_identity(2e2) == 200.0
    assert 1_000e-1 == 100.0 && 1e1_0 == 10000000000.0
    assert 123456789012345678901234567890e-29 > 1.0
    assert 0x1e3 == 483
    values := [1, 2, 3]
    assert values[1] == 2
    prn("Scientific floats passed")
}

main :: fn () -> i32 {
    prn("begin 001-number")
    {
        result := case_0_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 42
    }
    prn("begin 002-add-2-numbers")
    {
        result := case_1_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 167
    }
    prn("begin 003-precedence")
    {
        result := case_2_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 7
    }
    prn("begin 004-unary-and-grouping")
    {
        result := case_3_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 3
    }
    prn("begin 005-muldivmod-assoc")
    {
        result := case_4_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 2
    }
    prn("begin 031-primitive-operators")
    {
        result := case_5_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 1
    }
    prn("begin 032-unannotated-float-literal")
    {
        result := case_6_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 075-packed-integer-literals")
    {
        result := case_7_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 105-float-integer-casts")
    {
        result := case_8_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 42
    }
    prn("begin 106-prefixed-integer-literals")
    {
        result := case_9_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 26
    }
    prn("begin 108-size-operator")
    {
        result := case_10_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 68
    }
    prn("begin 112-integer-separators")
    {
        result := case_11_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 187
    }
    prn("begin 114-shift-operators")
    {
        result := case_12_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 60
    }
    prn("begin 115-float-separators")
    {
        result := case_13_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 42
    }
    prn("begin 139-size-field-name")
    {
        result := case_14_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 12
    }
    prn("begin 153-unsigned-byte-comparisons")
    {
        result := case_15_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 241-untyped-integer-float-context")
    {
        case_16_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 252-scientific-floats")
    {
        case_17_main()
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
begin 001-number
result 42
begin 002-add-2-numbers
result 167
begin 003-precedence
result 7
begin 004-unary-and-grouping
result 3
begin 005-muldivmod-assoc
result 2
begin 031-primitive-operators
float
cmp
mod
bits
result 1
begin 032-unannotated-float-literal
result 0
begin 075-packed-integer-literals
result 0
begin 105-float-integer-casts
result 42
begin 106-prefixed-integer-literals
result 26
begin 108-size-operator
i32=4 literal=4 array=12 slice=16 string=16 ptr=8 nil=0 fn=8 void=0
result 68
begin 112-integer-separators
result 187
begin 114-shift-operators
result 60
begin 115-float-separators
result 42
begin 139-size-field-name
result 12
begin 153-unsigned-byte-comparisons
lt
gt
nested=24
result 0
begin 241-untyped-integer-float-context
untyped integer float contexts passed
result 0
begin 252-scientific-floats
Scientific floats passed
result 0

¬

¬


=== case: 024-run-llvm-primitive-operators ===
use std.io

half: f32 = 0.5
limit: f64 = 2.0

main :: fn () {
    mask: u32 = 14
    ordered := 3 < 4 && 4 <= 4 && 5 > 4 && 5 >= 5
    prn(on (1.5 < limit && !no) => "float" else "bad")
    prn(on ordered => "cmp" else "bad")
    prn(on (5 % 2 == 1) => "mod" else "bad")
    prn(on (((mask & 11) ^ 3) == 9 || no) => "bits" else "bad")
    return on (half <= 0.5 && 3 != 4) => 1 else 0
}
¬
1
¬
float
cmp
mod
bits

¬

¬

=== case: 030-run-llvm-bool-cast ===
enabled: bool = yes

main :: fn () => enabled.as(i32)
¬
1
¬

¬

¬

=== case: 057-run-llvm-float-casts ===
main :: fn () -> i32 {
    up: f32 = 42.as(f32)
    down: f32 = 3.9
    below: f32 = -3.9

    whole := down.as(i32)
    negative := below.as(i32)
    return up.as(i32) + whole + negative
}
¬
42
¬

¬

¬

=== case: 077-run-llvm-size-operator ===
use std.io

helper :: fn () -> i32 {
    return 0
}

main :: fn () -> i32 {
    nums: [3]i32 = [1, 2, 3]
    view: []i32 = nums[..]
    text :: "hi"
    ptr: ^i32 = nil
    i32_size := i32.size
    literal_size := 128.size
    array_size := nums.size
    slice_size := view.size
    array_bytes := nums.bytes
    slice_bytes := view.bytes
    string_size := text.size
    ptr_size := ptr.size
    nil_size := nil.size
    fn_size := helper.size
    void_size := void.size
    prn($"i32={i32_size} literal={literal_size} array={array_size} slice={slice_size} array_bytes={array_bytes} slice_bytes={slice_bytes} string={string_size} ptr={ptr_size} nil={nil_size} fn={fn_size} void={void_size}")
    return (i32_size + literal_size + array_size + slice_size + array_bytes +
        slice_bytes + string_size + ptr_size + nil_size + fn_size +
        void_size).as(i32)
}
¬
92
¬
i32=4 literal=4 array=12 slice=16 array_bytes=12 slice_bytes=12 string=16 ptr=8 nil=0 fn=8 void=0

¬

¬

=== case: 126-run-bitwise-not ===
main :: fn () -> i32 {
    value: u32 = 0xfffffff0
    return (~value).as(i32)
}
¬
15
¬

¬

¬

=== case: 211-run-implicit-integer-comparison-width ===
main :: fn () -> i32 {
    count: usize = 4
    limit :: 65536 - 16384

    on count <= limit => return 42
    return 1
}
¬
42
¬

¬

¬

=== case: 294-run-content-equality ===
-- Content equality must honour each element's Eq behaviour and borrow owners.
Point :: plex {
    x i32
    ignored i32
}
comparisons := 0
impl [T] []T
where T: Eq {
    contains :: fn (s: ^Self, value: T) -> bool {
        for i in [0 .. s.count] {
            on s^[i] == value => return yes
        }
        return no
    }
}
same :: fn [T] (lhs: T, rhs: T) -> bool
where T: Eq { return lhs == rhs }
is_absent :: fn [T] (value: box[T]) => value == nil
main :: fn () {
    a := ["one", "two"]
    b := ["one", "two"]
    assert a == b && a[..] == b[..]
    assert same(a[..], b[..])
    assert a.contains("two")
    assert a[..1] != b[..]
    b[1] = "three"
    assert a != b && a[..] != b[..]
    empty: []string
    assert empty == a[..0]
    assert empty == nil
    assert a[..0] != nil

    f: [2]f64 = [0.0, 2.0]
    g: [2]f64 = [-0.0, 2.0]
    assert f == g && f[..] == g[..]
    single: [1]f32 = [0.0]
    negative: [1]f32 = [-0.0]
    assert single[..] == negative[..]
    zero: f64 = 0.0
    nan := zero / zero
    n := [nan]
    assert n[..] != n[..]
    assert n != n

    p := [Point {x: 1, ignored: 2}, Point {x: 2, ignored: 3}]
    q := [Point {x: 1, ignored: 8}, Point {x: 2, ignored: 9}]
    comparisons = 0
    assert p[..] == q[..]
    assert comparisons == 2
    assert same(p[..], q[..])
    assert p == q
    comparisons = 0
    assert p[..1] != q[..]
    assert comparisons == 0
    q[0].x = 10
    assert p[..] != q[..]
    assert comparisons == 1

    x := box[i32](2)
    y := box[i32](2)
    x.data[0] = 5
    x.data[1] = 0
    y.data[0] = 5
    y.data[1] = 0
    assert x.count == 2 && y.count == 2
    assert x.data != y.data
    assert x == y && !(x != y)
    assert x != nil && y != nil
    y.data[1] = 6
    assert x != y
    smaller := box[i32]()
    smaller.data[0] = 5
    assert x != smaller
    nil_box: box[i32]
    other_nil: box[i32]
    assert nil_box == other_nil
    assert nil_box != x && x != nil_box
    assert nil_box.count == 0

    z := box[Point]()
    w := box[Point]()
    z.x = 8
    w.x = 8
    z.ignored = 1
    w.ignored = 2
    assert z == w
    assert z.count == 1 && w.count == 1
    assert z.x == 8 && w.x == 8
    w.x = 9
    assert z != w

    nested_a := [a[..], b[..]]
    nested_b := [a[..], b[..]]
    assert nested_a[..] == nested_b[..]
    assert nested_a == nested_b
    nested_b[1] = a[..]
    assert nested_a[..] != nested_b[..]

    strings := box[string](2)
    other_strings := box[string](2)
    strings.data[0] = "hello"
    strings.data[1] = "world"
    other_strings.data[0] = "hello"
    other_strings.data[1] = "world"
    assert strings == other_strings
    assert strings.count == 2

    floats := box[f64]()
    other_floats := box[f64]()
    floats.data[0] = nan
    other_floats.data[0] = nan
    assert floats != other_floats && floats != floats
    floats.data[0] = 0.0
    other_floats.data[0] = -0.0
    assert floats == other_floats

    Strings :: []string
    boxed_slices := box[Strings]()
    other_boxed_slices := box[Strings]()
    boxed_slices.data[0] = a[..]
    other_boxed_slices.data[0] = a[..]
    assert boxed_slices == other_boxed_slices

    inner := box[i32]()
    other_inner := box[i32]()
    inner.data[0] = 42
    other_inner.data[0] = 42
    -- The slices borrow box storage; their comparisons must not consume it.
    box_view := (^inner).as([]box[i32], 1)
    other_box_view := (^other_inner).as([]box[i32], 1)
    assert box_view == other_box_view
    assert inner != nil && other_inner != nil
    assert inner.data[0] == 42
    arrays_a := [[1, 2], [3, 4]]
    arrays_b := [[1, 2], [3, 4]]
    assert arrays_a[..] == arrays_b[..]
    assert arrays_a == arrays_b
    assert is_absent[arena](nil)
    missing_arena: box[arena]
    assert missing_arena == nil
    Callback :: fn () -> i32
    missing_callback: box[Callback]
    assert missing_callback == nil
    prn("content equality passed")
}
impl Eq for Point {
    eq :: fn (lhs: Self, rhs: Self) -> bool {
        comparisons += 1
        return lhs.x == rhs.x
    }
}
¬
0
¬
content equality passed
¬

¬

=== case: 138-run-llvm-unsigned-arithmetic ===
main :: fn () -> i32 {
    high := 0xffff_ffff_ffff_fffe.as(u64)

    on high % 4 != 2 => return 1
    on high / 2 != high >> 1 => return 2
    on high >> 1 <= 0x7fff_ffff.as(u64) => return 3

    return 0
}
¬
0
¬

¬

¬


=== case: 152-run-character-literal-contextual-u32 ===
put :: fn (_ch: u32, _ink: u32, _paper: u32) {}

main :: fn () -> i32 {
    tile: u8 = 1
    (char, ink, paper) := on tile {
        0x00 => (' ', 0x000000, 0x000000)
        0x01 => (' ', 0xffffff, 0xffffff)
        else => ('?', 0xff00ff, 0x000000)
    }
    put(char, ink, paper)
    on '╭' != 9581 => return 1
    return 0
}
¬
0
¬

¬

¬


=== case: 194-run-llvm-logical-short-circuit ===
use std.io

Node :: plex {
    value i32
}

main :: fn () -> i32 {
    missing: ^Node = nil
    value := Node {
        value: 7
    }
    present := ^value

    on missing == nil || missing.value == 0 => {
        prn("or short")
    }
    on missing != nil && missing.value == 0 => {
        prn("bad and")
        return 1
    }
    on present != nil && present.value == 7 => {
        prn("and rhs")
    }
    return 0
}
¬
0
¬
or short
and rhs

¬

¬


=== case: 335-structural-equality ===
-- test-release
use std.slice
use structural_eq
Point :: plex { x i32 name string }
Event :: enum { None Value(Point) Pair(i32, string) }
Custom :: plex { key i32 ignored i32 }
comparisons := 0
impl Eq for Custom {
 eq :: fn (lhs:Self, rhs:Self) -> bool {
  comparisons += 1
  return lhs.key == rhs.key
 }
}
Wrapped :: plex { first i32 custom Custom }
CustomEnum :: enum { A(i32) B }
impl Eq for CustomEnum {
 eq :: fn (_lhs:Self, _rhs:Self) -> bool { return yes }
}
Packed :: plex #packed { number u8 value i64 }
Empty :: plex {}
Kind :: enum { First Second }
Bits :: plex { u32 { kind Kind : 8 offset : 24 } }
same :: fn [T] (a:T, b:T) -> bool where T: Eq { return a == b }
main :: fn () {
 a := Point { x:1, name:"a" }
 b := Point { x:1, name:"a" }
 assert same(a,b)
 b.x = 2
 assert a != b
 x: Event = Value(a)
 y: Event = Value(b)
 assert x != y
 y = Value(a)
 assert same(x,y)
 y = None
 assert x != y
 p: Event = Pair(1,"x")
 q: Event = Pair(1,"y")
 assert p != q
 items := [a,b]
 assert items.contains(a)
 c := Custom { key:1, ignored:2 }
 d := Custom { key:1, ignored:9 }
 w := Wrapped { first:1, custom:c }
 v := Wrapped { first:1, custom:d }
 comparisons = 0
 assert same(w,v)
 assert comparisons == 1
 v.first = 2
 comparisons = 0
 assert w != v
 assert comparisons == 0
 ce: CustomEnum = A(1)
 de: CustomEnum = B
 assert same(ce,de)
 assert [ce] == [de]
 left := Packed { number:1, value:42 }
 right := Packed { number:1, value:43 }
 assert left != right
 assert same(Empty {}, Empty {})
 optional: ?Point = a
 other: ?Point = b
 assert optional != other
 assert optional != nil
 absent: ?arena
 assert absent == nil
 imported_a: Holder = { item: { value:1, ignored:2 }, choice: First(1) }
 imported_b: Holder = { item: { value:1, ignored:3 }, choice: Second }
 assert imported_a == imported_b
 bits_a := Bits { kind: Kind.First, offset: 3 }
 bits_b := Bits { kind: Kind.First, offset: 3 }
 assert same(bits_a,bits_b)
 bits_b.kind = Kind.Second
 assert bits_a != bits_b
 prn("Structural equality passed")
}

¬
0
¬
Structural equality passed

¬

¬
