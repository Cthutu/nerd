-- bindings feature scenarios. Results and stdout are checked in order.
use std.io

-- 006-global-vars
-- Uses top-level constant bindings from main.
case_0_answer :: 42
case_0_magic_number :: 7

case_0_main :: fn () => case_0_answer / case_0_magic_number

-- 007-forward-refs
-- Resolves a top-level constant through a forward reference.
case_1_magic_number :: case_1_answer / 6
case_1_answer :: 42

case_1_main :: fn () => case_1_magic_number

-- 008-folded-bind-chain
-- Folds a chain of dependent top-level constant bindings.
case_2_base :: 4
case_2_factor :: 5
case_2_answer :: case_2_base * case_2_factor + 2

case_2_main :: fn () => case_2_answer

-- 009-folded-const-tree
-- Folds a small constant-expression tree across bindings.
case_3_left :: 2 + 3
case_3_right :: case_3_left * (10 - 4)

case_3_main :: fn () => case_3_right % 7

-- 010-hello-world
-- Prints a simple string from a block-bodied main.
case_4_main :: fn () {
    prn("Hello, world!")
}

-- 012-block-return
-- Returns a value explicitly from a block-bodied main.
case_5_main :: fn () -> i32 {
    return 42
}

-- 013-typed-binding
-- Declares a top-level constant with an explicit type annotation.
case_6_value: i32: 120

case_6_main :: fn () => case_6_value

-- 015-variables
-- Mutates variables and zero-initialises an explicit local variable.
case_7_base := 40

case_7_main :: fn () -> i32 {
    case_7_base = case_7_base + 1
    extra: i32
    extra = 1
    return case_7_base + extra
}

-- 016-type-alias-and-cast
-- Uses a type alias and narrows a value with an explicit cast.
Case_8_Price :: u16

case_8_main :: fn () -> u8 {
    amount: Case_8_Price = 1000
    narrowed := amount.as(u8)
    return narrowed
}

-- 018-primitive-variables
-- Declares primitive variables with explicit types and casts.
case_9_greeting: string = "Hello"
case_9_enabled: bool = yes
case_9_disabled: bool = no
case_9_truthy: bool = yes
case_9_ratio: f32 = 42.as(f32)
case_9_weight: f64

case_9_main :: fn () => case_9_enabled.as(i32) + case_9_disabled.as(i32) + case_9_truthy.as(i32)

-- 020-nested-blocks
-- Shows that nested block state is dropped after leaving the block.
case_10_main :: fn () -> i32 {
    value := 10
    {
        value := 32
        value = value + 1
        prn($"inner={value}")
    }
    prn($"outer={value}")
    return value
}

-- 033-binding-expression-starts
-- Accepts every expression-start token handled by binding classification.
case_11_floaty :: 3.14
case_11_negative :: -1
case_11_truth :: yes
case_11_inverted :: !no

case_11_main :: fn () => 0

-- 040-compound-assignments
case_12_main :: fn () {
    n := 1
    n += 2
    n *= 4
    n -= 5
    n /= 2
    n %= 2

    bits := 6
    bits &= 3
    bits ~= 7
    bits |= 8

    flag := yes
    flag &&= no
    flag ||= yes

    prn($"{n} {bits} {flag}")
}

-- 045-expression-block
case_13_main :: fn () -> i32 {
    first :: ${
        break 7
    }
    second :: ${
        break first + 5
    }
    word :: ${
        break "done"
    }
    flag :: ${
        break yes
    }

    pr("first = ")
    prn($"{first}")
    prn($"second = {second}")
    prn($"word = {word}")
    prn($"flag = {flag}")
    return second
}

-- 047-expression-block-bindings
-- Uses expression blocks as top-level constant and variable initialisers.
case_14_constant_value :: ${
    break 4
}

case_14_variable_value := ${
    break case_14_constant_value + 6
}

case_14_main :: fn () -> i32 {
    prn($"constant = {case_14_constant_value}")
    prn($"variable = {case_14_variable_value}")

    case_14_variable_value += 2
    prn($"updated = {case_14_variable_value}")

    return case_14_variable_value
}

-- 048-labelled-expression-block
case_15_main :: fn () -> i32 {
    answer :: $answer {
        break $answer 41 + 1
    }
    word :: $word {
        break $word "labelled"
    }
    outer :: $outer {
        $inner {
            break $outer 99
        }
        break $outer 0
    }
    hits := 0
    $void {
        hits += 1
        break $void
    }

    prn($"answer = {answer}")
    prn($"word = {word}")
    prn($"outer = {outer}")
    prn($"hits = {hits}")
    return outer
}

-- 082-assignment-expressions
case_16_main :: fn () -> i32 {
    value: i32 = 1

    on yes => value = 7
    prn($"{value}")

    copy: i32 = value = 9
    prn($"{value} {copy}")

    return copy
}

-- 089-field-lvalues
Case_17_Node :: plex {
    value i32
}

case_17_main :: fn () -> i32 {
    node: Case_17_Node = Case_17_Node { value: 1 }
    node.value = 3
    node.value += 2
    node.value *= 2
    node.value /= 2
    node.value -= 1

    ptr: ^Case_17_Node = ^node
    ptr.value = 5
    ptr.value += 3
    ptr^.value = 7
    ptr^.value += 4
    ptr^.value %= 6

    return node.value + ptr^.value - 10
}

-- 104-public-type-alias-binding
-- Binds public declarations directly to types and uses them as aliases.
pub Case_18_Score :: i32
pub Case_18_FinalScore :: Case_18_Score

case_18_add :: fn (lhs: Case_18_Score, rhs: Case_18_FinalScore) -> Case_18_Score {
    return lhs + rhs
}

case_18_main :: fn () -> i32 {
    lhs: Case_18_Score = 20
    rhs: Case_18_FinalScore = 22
    return case_18_add(lhs, rhs)
}

-- 109-definite-assignment
case_19_choose_with_else :: fn () -> i32 {
    value: i32 = undefined
    on yes => value = 21 else value = 3
    return value
}

case_19_choose_bool :: fn (flag: bool) -> i32 {
    value: i32 = undefined
    on flag {
        yes => value = 2
        else => value = 4
    }
    return value
}

case_19_main :: fn () -> i32 {
    return case_19_choose_with_else() + case_19_choose_bool(yes) + case_19_choose_bool(no)
}

-- 110-unused-underscore
case_20_helper :: fn (_: i32, _: i32, _unused: i32) -> i32 {
    _ := 1
    _ := 2
    _scratch := 10
    return 7
}

case_20_value :: fn () -> i32 {
    return 5
}

case_20_main :: fn () -> i32 {
    _ := case_20_value()
    _ := case_20_value()
    _future := 3
    return case_20_helper(1, 2, 3)
}

-- 144-local-type-declarations
case_21_main :: fn () -> i32 {
    Point :: plex {
        x i32
        y i32
    }

    Value :: union {
        i i32
        f f32
    }

    Choice :: enum { Left(i32) Right(i32) }

    point: Point = Point { x: 10, y: 20 }
    value: Value = Value { i: 7 }
    choice: Choice = Right(5)

    extra := on choice {
        Left(n) => n
        Right(n) => n
    }

    return point.x + point.y + value.i + extra
}

-- 159-return-context-local-inference
Case_22_LRESULT :: isize

case_22_callback :: fn () -> Case_22_LRESULT {
    result := 0
    return result
}

case_22_main :: fn () -> i32 {
    return case_22_callback().as(i32)
}

-- 160-underscore-unused-assignment
case_23_main :: fn () -> i32 {
    _value := 1
    _value = 2
    return 0
}

-- 242-mutable-aggregate-storage
-- Mutation still works for variables and storage reached through pointers/views.
case_24_support :: use tests.mods.immutable_aggregate_bindings
Case_24_Point :: plex { x i32 }
Case_24_Holder :: plex { point Case_24_Point }
Case_24_Value :: union { integer i32 real f32 }

case_24_change :: fn (point: ^Case_24_Point) { point.x += 1 }
case_24_borrow :: fn (point: ^Case_24_Point) -> ^Case_24_Point { return point }
case_24_change_first :: fn (points: []Case_24_Point) { points[0].x += 1 }

case_24_main :: fn () {
    point := Case_24_Point { x: 20 }
    point.x += 1
    pointer :: case_24_borrow(^point)
    pointer.x += 1
    case_24_change(^point)
    assert point.x == 23

    holder := Case_24_Holder { point: Case_24_Point { x: 40 } }
    holder.point.x += 2
    assert holder.point.x == 42

    points := [Case_24_Point { x: 20 }, Case_24_Point { x: 22 }]
    points[0].x += 1
    view := points[..]
    borrowed :: view
    borrowed[0].x += 1
    case_24_change_first(view)
    assert points[0].x == 23

    value := Case_24_Value { integer: 40 }
    value.integer += 2
    assert value.integer == 42

    original :: Case_24_Point { x: 20 }
    copy := original
    copy.x = 42
    assert original.x == 20 && copy.x == 42

    case_24_support.variable.x = 42
    assert case_24_support.variable.x == 42
    prn("mutable aggregate storage passed")
}

-- 275-run-range-item-used-in-nested-local
impl string {
    pub lines :: fn (text: Self) => Case_26_StringLinesIterator {
        text   : text
        offset : 0
    }
}

pub Case_26_StringLinesIterator :: plex {
    text   string
    offset usize
}

impl Iterator[string] for Case_26_StringLinesIterator {
    pub next :: fn (iterator: ^Self) -> ?string {
        start := iterator.offset
        for i in [iterator.offset .. iterator.text.bytes] {
            on i == iterator.offset => {
                line := iterator.text[start..i]
                iterator.offset = i
                return line
            }
        }
        return nil
    }
}

case_26_main :: fn () -> i32 {
    iterator := "hello".lines()
    return iterator.offset.as(i32) + 7
}

main :: fn () -> i32 {
    prn("begin 006-global-vars")
    {
        result := case_0_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 6
    }
    prn("begin 007-forward-refs")
    {
        result := case_1_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 7
    }
    prn("begin 008-folded-bind-chain")
    {
        result := case_2_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 22
    }
    prn("begin 009-folded-const-tree")
    {
        result := case_3_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 2
    }
    prn("begin 010-hello-world")
    {
        case_4_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 012-block-return")
    {
        result := case_5_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 42
    }
    prn("begin 013-typed-binding")
    {
        result := case_6_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 120
    }
    prn("begin 015-variables")
    {
        result := case_7_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 42
    }
    prn("begin 016-type-alias-and-cast")
    {
        result := case_8_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 232
    }
    prn("begin 018-primitive-variables")
    {
        result := case_9_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 2
    }
    prn("begin 020-nested-blocks")
    {
        result := case_10_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 10
    }
    prn("begin 033-binding-expression-starts")
    {
        result := case_11_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 040-compound-assignments")
    {
        case_12_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 045-expression-block")
    {
        result := case_13_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 12
    }
    prn("begin 047-expression-block-bindings")
    {
        result := case_14_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 12
    }
    prn("begin 048-labelled-expression-block")
    {
        result := case_15_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 99
    }
    prn("begin 082-assignment-expressions")
    {
        result := case_16_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 9
    }
    prn("begin 089-field-lvalues")
    {
        result := case_17_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 104-public-type-alias-binding")
    {
        result := case_18_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 42
    }
    prn("begin 109-definite-assignment")
    {
        result := case_19_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 27
    }
    prn("begin 110-unused-underscore")
    {
        result := case_20_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 7
    }
    prn("begin 144-local-type-declarations")
    {
        result := case_21_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 42
    }
    prn("begin 159-return-context-local-inference")
    {
        result := case_22_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 160-underscore-unused-assignment")
    {
        result := case_23_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 242-mutable-aggregate-storage")
    {
        case_24_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 275-run-range-item-used-in-nested-local")
    {
        result := case_26_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 7
    }
    return 0
}
¬
0
¬
begin 006-global-vars
result 6
begin 007-forward-refs
result 7
begin 008-folded-bind-chain
result 22
begin 009-folded-const-tree
result 2
begin 010-hello-world
Hello, world!
result 0
begin 012-block-return
result 42
begin 013-typed-binding
result 120
begin 015-variables
result 42
begin 016-type-alias-and-cast
result 232
begin 018-primitive-variables
result 2
begin 020-nested-blocks
inner=33
outer=10
result 10
begin 033-binding-expression-starts
result 0
begin 040-compound-assignments
1 13 yes
result 0
begin 045-expression-block
first = 7
second = 12
word = done
flag = yes
result 12
begin 047-expression-block-bindings
constant = 4
variable = 10
updated = 12
result 12
begin 048-labelled-expression-block
answer = 42
word = labelled
outer = 99
hits = 1
result 99
begin 082-assignment-expressions
7
9 9
result 9
begin 089-field-lvalues
result 0
begin 104-public-type-alias-binding
result 42
begin 109-definite-assignment
result 27
begin 110-unused-underscore
result 7
begin 144-local-type-declarations
result 42
begin 159-return-context-local-inference
result 0
begin 160-underscore-unused-assignment
result 0
begin 242-mutable-aggregate-storage
mutable aggregate storage passed
result 0
begin 275-run-range-item-used-in-nested-local
result 7

¬

¬


=== case: 037-run-llvm-nested-blocks ===
use std.io

main :: fn () {
    value := 10
    {
        value := 32
        value = value + 1
        prn($"inner={value}")
    }
    prn($"outer={value}")
    return value
}
¬
10
¬
inner=33
outer=10

¬

¬

=== case: 060-run-llvm-field-assignment ===
Thing :: plex {
    size (i32, i32)
}

main :: fn () -> i32 {
    thing: Thing
    thing.size = (7, 5)
    return thing.size.0 + thing.size.1
}
¬
12
¬

¬

¬

=== case: 274-run-assignment-context-local-inference ===
main :: fn () -> i32 {
    count : usize = 4
    start := 0

    for i in [0 ..= count] {
        start = i + count
    }

    assert start == 8
    return 0
}
¬
0
¬

¬

¬

=== case: 085-undefined ===
main :: fn () {
    value: i32 = undefined
    value = 42
    return value
}
¬
42
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  let value: i32 = <unknown> default
  assign i32 local.0(value) = i32 42
  return i32 local.0(value)
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %local.0 = alloca i32
  store i32 0, ptr %local.0
  store i32 42, ptr %local.0
  %t0 = load i32, ptr %local.0
  ret i32 %t0
}

@$main = hidden alias i32 (), ptr @fn.0


=== case: 177-default-undefined-opt-out ===
Point :: plex {
    x i32
    y i32
}

impl Default for Point {
    default :: fn () -> Self {
        prn("default")
        return Point { x: 10, y: 20 }
    }
}

main :: fn () -> i32 {
    point: Point = undefined
    point = Point { x: 1, y: 2 }
    prn($"point={point.x},{point.y}")
    return point.x + point.y
}
¬
3
¬
point=1,2

¬
hir 0
¬


=== case: 283-run-compound-assignment-inference ===
main :: fn () -> i32 {
    total := 0
    total *= 10
    total += 7.as(u64) - 1
    return total.as(i32) - 6
}
¬
0
¬

¬

¬
