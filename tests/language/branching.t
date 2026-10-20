-- branching feature scenarios. Results and stdout are checked in order.
use std.io

-- 027-on-bool
-- Chooses between two values with a short-form boolean `on` expression.
case_0_enabled: bool = yes

case_0_main :: fn () => on case_0_enabled => 42 else 7

-- 028-on-value-branches
-- Matches integer values with block-form `on` branches.
case_1_size :: 2

case_1_test_branch :: fn (case_1_size: u32) -> i32 {
    return on case_1_size {
        0 => 10
        1 => 20
        else => 30
    }
}

case_1_main :: fn () -> i32 {
    prn($"0: {case_1_test_branch(0)}")
    prn($"1: {case_1_test_branch(1)}")
    prn($"2: {case_1_test_branch(2)}")

    return case_1_test_branch(case_1_size)
}

-- 029-on-comma-patterns
-- Matches multiple integer values in one block-form `on` branch.
case_2_size :: 2

case_2_test_branch :: fn (case_2_size: u32) -> i32 {
    return on case_2_size {
        0, 1 => 10
        else => 30
    }
}

case_2_main :: fn () -> i32 {
    prn($"0: {case_2_test_branch(0)}")
    prn($"1: {case_2_test_branch(1)}")
    prn($"2: {case_2_test_branch(2)}")

    return case_2_test_branch(case_2_size)
}

-- 030-on-ranges
-- Matches exclusive and inclusive integer ranges in block-form `on` branches.
case_3_test_branch :: fn (size: u32) -> i32 {
    return on size {
        0..2 => 10
        2..=4 => 20
        else => 30
    }
}

case_3_main :: fn () -> i32 {
    prn($"0: {case_3_test_branch(0)}")
    prn($"1: {case_3_test_branch(1)}")
    prn($"2: {case_3_test_branch(2)}")
    prn($"4: {case_3_test_branch(4)}")
    prn($"5: {case_3_test_branch(5)}")

    return case_3_test_branch(5)
}

-- 035-on-pattern-binders
-- Binds the matched value inside each block-form `on` branch.
case_4_size :: 2

case_4_test_branch :: fn (case_4_size: u32) -> u32 {
    return on case_4_size {
        0, 1 as matched => matched + 10
        2..=4 as ranged => ranged + 20
        else as other => other + 30
    }
}

case_4_main :: fn () -> i32 {
    prn($"0: {case_4_test_branch(0)}")
    prn($"2: {case_4_test_branch(2)}")
    prn($"5: {case_4_test_branch(5)}")

    return case_4_test_branch(case_4_size).as(i32)
}

-- 036-on-untyped-local
-- Block-form `on` materialises untyped local integer constants to i32.
case_5_main :: fn () {
    i :: 2

    prn(on i {
        0 => "zero"
        else as x => $"non-zero: {x}"
    })
}

-- 037-on-statement-partial
-- Statement-position block-form `on` may omit `else`.
case_6_main :: fn () {
    value :: 2

    on value {
        1 => prn("one")
    }

    prn("done")
}

-- 046-on-string-patterns
-- Matches strings with block-form `on` branches.
case_7_choice :: "blue"
case_7_warm :: "warm"
case_7_cool :: "cool"
case_7_unknown :: "unknown"

case_7_classify :: fn (case_7_choice: string) -> i32 {
    return on case_7_choice {
        "red", "green" => 1
        "blue" => 2
        else => 3
    }
}

case_7_label :: fn (case_7_choice: string) -> string {
    return on case_7_choice {
        "red" => case_7_warm
        "blue" => case_7_cool
        else => case_7_unknown
    }
}

case_7_main :: fn () -> i32 {
    prn($"red: {case_7_classify("red")} {case_7_label("red")}")
    prn($"green: {case_7_classify("green")} {case_7_label("green")}")
    prn($"blue: {case_7_classify("blue")} {case_7_label("blue")}")
    prn($"other: {case_7_classify("other")} {case_7_label("other")}")

    return case_7_classify(case_7_choice)
}

-- 060-on-pattern-guards
case_8_score :: fn (size: u32) -> u32 {
    return on size {
        0..3 as matched on matched == 2 => matched + 20
        0..3 => 10
        10 as size on size == 10 => size * 10
        else as size => size + 100
    }
}

case_8_main :: fn () {
    prn($"1: {case_8_score(1)}")
    prn($"2: {case_8_score(2)}")
    prn($"10: {case_8_score(10)}")
    prn($"50: {case_8_score(50)}")
}

-- 061-on-structural-patterns
Case_9_Point :: plex {
    x i32
    y i32
    name string
}

case_9_score_pair :: fn (pair: (i32, i32)) -> i32 {
    wanted :: 9
    return on pair {
        (0, _) => 100
        (1, y) => y
        (x, for wanted) => x * 10
        else => 0
    }
}

case_9_score_point :: fn (point: Case_9_Point) -> i32 {
    fallback :: "fallback"
    return on point {
        { name: "origin", x: x } => x
        { x: 3, y: y } => y * 10
        { name: for fallback } => 50
        else => 0
    }
}

case_9_name_of :: fn (point: Case_9_Point) -> string {
    return on point {
        { name: name } => name
        else => ""
    }
}

case_9_main :: fn () -> i32 {
    prn($"pair zero {case_9_score_pair((0, 7))}")
    prn($"pair one {case_9_score_pair((1, 8))}")
    prn($"pair nine {case_9_score_pair((4, 9))}")
    prn($"pair else {case_9_score_pair((2, 3))}")
    prn($"point origin {case_9_score_point(Case_9_Point { x: 2, y: 3, name: "origin" })}")
    prn($"point x {case_9_score_point(Case_9_Point { x: 3, y: 4, name: "other" })}")
    prn($"point fallback {case_9_score_point(Case_9_Point { x: 5, y: 6, name: "fallback" })}")
    prn($"point name {case_9_name_of(Case_9_Point { x: 9, y: 10, name: "named" })}")
    return 0
}

-- 064-on-in-interpolated-string
Case_10_Colour :: enum { RED GREEN BLUE }

case_10_main :: fn () {
    colour : Case_10_Colour = RED

    prn($"Colour = {on colour { RED => "red" GREEN => "green" BLUE => "blue" }}")
}

-- 066-generalised-on
case_11_score :: fn (value: i32) -> i32 {
    return on value {
        < 0 => -1
        == 0 => 0
        1, 2 => 10
        >= 10 => 100
        else => 20
    }
}

case_11_describe :: fn (value: i32) -> string {
    return on {
        value < 0 => "negative"
        value == 0 => "zero"
        value < 10 => "small"
        else => "large"
    }
}

case_11_not_five :: fn (value: i32) -> i32 {
    return on value {
        != 5 => 1
        else => 0
    }
}

case_11_main :: fn () -> i32 {
    prn($"scores {case_11_score(-2)} {case_11_score(0)} {case_11_score(2)} {case_11_score(7)} {case_11_score(12)}")
    prn($"descriptions {case_11_describe(-2)} {case_11_describe(0)} {case_11_describe(7)} {case_11_describe(12)}")
    prn($"not-five {case_11_not_five(4)} {case_11_not_five(5)}")
    return case_11_score(12)
}

-- 074-qualified-enum-patterns
Case_12_Maybe :: enum { None Some(i32) Pair(i32, i32) }

case_12_score :: fn (value: Case_12_Maybe) -> i32 {
    return on value {
        Case_12_Maybe.None => 0
        Case_12_Maybe.Some(x) => x
        Case_12_Maybe.Pair(left, right) => left + right
    }
}

case_12_main :: fn () -> i32 {
    return case_12_score(Case_12_Maybe.Pair(10, 20))
}

-- 098-on-branch-loop-control
case_13_main :: fn () -> i32 {
    for {
        value :: 1
        on value {
            1 => break
        }
        return 1
    }
    return 0
}

-- 100-on-short-block-expr
case_14_split :: fn (s: string, sep: string) -> [..]string {
    parts: [..]string

    on sep.count == 0 => {
        parts.push(s)
        return parts
    }

    return parts
}

case_14_main :: fn () -> i32 {
    parts := case_14_split("look", "")
    on parts.count != 1 => return 1
    on parts.count == 2 => {
        return 2
    } else return 0
    return 0
}

-- 118-nested-on-branch
case_15_main :: fn () -> i32 {
    value := 2
    hit := 0

    on value == 2 => {
        on {
            value == 1 => hit = 1
            else => hit = 2
        }
    }

    prn($"hit {hit}")
    return hit
}

-- 144-pointer-equality-and-nil-on
case_16_main :: fn () -> i32 {
    item: i32
    ptr: ^i32 = ^item
    nil_ptr: ^i32
    void_ptr: ^void = ptr

    on nil_ptr {
        nil => {}
        else => return 1
    }

    on ptr == nil => return 2
    on nil != nil_ptr => return 3
    on void_ptr != ptr => return 4
    return 0
}

-- 151-typed-plex-patterns
Case_17_Event :: enum {
    KeyDown(Case_17_KeyEvent)
    KeyUp(Case_17_KeyEvent)
}

Case_17_KeyEvent :: plex {
    keycode   Case_17_Key
    modifiers i32
    char      u32
}

Case_17_Key :: enum {
    Escape = 41
    Other  = 42
}

case_17_score :: fn (event: Case_17_Event) -> i32 {
    return on event {
        KeyDown(Case_17_KeyEvent { keycode: Escape }) => 1
        KeyDown(Case_17_KeyEvent { keycode: Other }) => 2
        else => 0
    }
}

case_17_main :: fn () -> i32 {
    escape := Case_17_Event.KeyDown(Case_17_KeyEvent { keycode: Escape, modifiers: 0, char: 0 })
    other := Case_17_Event.KeyDown(Case_17_KeyEvent { keycode: Other, modifiers: 1, char: 2 })
    up := Case_17_Event.KeyUp(Case_17_KeyEvent { keycode: Escape, modifiers: 0, char: 0 })
    prn($"typed plex patterns {case_17_score(escape)} {case_17_score(other)} {case_17_score(up)}")
    return case_17_score(escape)
}

-- 152-on-break-for-expression
Case_18_Info :: plex {
    handle u64
}

Case_18_System :: plex {
    infos [..]Case_18_Info
}

case_18_find :: fn (system: ^Case_18_System, handle: u64) -> ^Case_18_Info {
    return for info in system.infos {
        on info.handle == handle => break info
    } else {
        break nil
    }
}

case_18_main :: fn () -> i32 {
    infos: [2..]Case_18_Info
    infos.push({ handle: 7 })
    infos.push({ handle: 9 })

    system := Case_18_System { infos }
    found  := case_18_find(^system, 9)
    on found == nil => return 1
    on found.handle != 9 => return 2

    missing := case_18_find(^system, 3)
    on missing != nil => return 3

    return 0
}

-- 153-break-on
Case_19_Info :: plex {
    handle u64
}

Case_19_System :: plex {
    infos [..]Case_19_Info
}

case_19_find :: fn (system: ^Case_19_System, handle: u64) -> ^Case_19_Info {
    return for info in system.infos {
        break on info.handle == handle => info
    } else {
        break nil
    }
}

case_19_main :: fn () -> i32 {
    labelled := $pick {
        break $pick on 1 == 1 => 5
        break $pick 9
    }
    on labelled != 5 => return 1

    infos: [2..]Case_19_Info
    infos.push({ handle: 7 })
    infos.push({ handle: 9 })

    system := Case_19_System { infos }
    found  := case_19_find(^system, 9)
    on found == nil => return 2
    on found.handle != 9 => return 3

    missing := case_19_find(^system, 3)
    on missing != nil => return 4

    return 0
}

-- 183-pattern-binds-and-for-values
Case_20_Maybe :: enum { None Some(i32) Pair(i32, i32) }

case_20_score :: fn (value: Case_20_Maybe, wanted: i32) -> i32 {
    return on value {
        None => 0
        Some(for wanted) => 10
        Some(found) => found
        Pair(left, for wanted) => left + wanted
        Pair(left, right) => left + right
    }
}

case_20_main :: fn () -> i32 {
    some_wanted : Case_20_Maybe = Some(7)
    some_other : Case_20_Maybe = Some(4)
    pair_wanted : Case_20_Maybe = Pair(2, 7)
    pair_other : Case_20_Maybe = Pair(2, 3)

    a := case_20_score(some_wanted, 7)
    b := case_20_score(some_other, 7)
    c := case_20_score(pair_wanted, 7)
    d := case_20_score(pair_other, 7)

    prn($"pattern syntax {a} {b} {c} {d}")
    return a + b + c + d
}

-- 184-on-postfix-deref-scrutinee
Case_21_Map :: plex {
    width u32
    height u32
}

Case_21_Size :: plex {
    width u32
    height u32
}

case_21_score :: fn (map: ^Case_21_Map, size: Case_21_Size) -> i32 {
    return on map^ {
        { width: for size.width, height: for size.height } => 1
        else => 2
    }
}

case_21_main :: fn () -> i32 {
    map := Case_21_Map { width: 80, height: 25 }
    same := Case_21_Size { width: 80, height: 25 }
    different := Case_21_Size { width: 100, height: 25 }

    a := case_21_score(^map, same)
    b := case_21_score(^map, different)
    prn($"postfix on {a} {b}")
    return a + b
}

-- 186-unqualified-enum-comma-patterns
Case_22_Key :: enum {
    Escape = 27
    Q      = 81
    Other  = 99
}

case_22_score :: fn (key: Case_22_Key) -> i32 {
    return on key {
        Escape, Q => 1
        Other => 2
    }
}

case_22_block_score :: fn (key: Case_22_Key) -> i32 {
    return on key {
        Escape, Q => {
            break 3
        }
        Other => 4
    }
}

case_22_main :: fn () -> i32 {
    return case_22_score(Q) + case_22_block_score(Escape)
}

-- 200-on-regressions
-- Runtime on regressions. Original IDs and exact golden output keep cases traceable.
-- HIR/LLVM-specific on fixtures remain separate.

-- 129-run-on-expression-return-branches
case_23_read :: fn (value: ?u8) -> u8 {
    return on value {
        byte => {
            copied : u8 = byte
            return copied
        }
        else => {
            return 0
        }
    }
}

case_23_case_129 :: fn () -> i32 {
    result := case_23_read('x')
    prn($"{result}")
    return result.as(i32)
}

-- 172-run-on-enum-payload-branch-return
Case_23_Size :: plex {
    width u16
    height u16
}

Case_23_Mode :: enum {
    Fit {
        scale u16
    }

    Fixed {
        width u16
        height u16
    }
}

case_23_dims :: fn (mode: Case_23_Mode) -> Case_23_Size {
    on mode {
        Fit { scale } => {
            return Case_23_Size { width: scale height: scale }
        }
        Fixed { width, height } => {
            return Case_23_Size { width: width height: height }
        }
    }

    return Case_23_Size { width: 1 height: 1 }
}

case_23_case_172 :: fn () {
    size := case_23_dims(Case_23_Mode.Fixed { width: 10 height: 20 })
    prn($"{size.width}x{size.height}")
}

-- 221-run-on-expression-return-option-variant
case_23_direct :: fn (value: ?u8) -> ?u8 {
    byte := on value {
        found => found
        else => return nil
    }
    return byte
}

case_23_block :: fn (value: ?u8) -> ?u8 {
    byte := on value {
        found => found
        else => {
            return nil
        }
    }
    return byte
}

case_23_case_221 :: fn () -> i32 {
    direct_result := case_23_direct(nil)
    block_result := case_23_block(nil)

    on direct_result => {
        return 1
    } else {
    }

    on block_result => {
        return 2
    } else {
    }

    return 0
}

-- 222-run-on-expression-assignment-binder-read
case_23_out : u8

case_23_maybe :: fn () -> ?u8 {
    return 7
}

case_23_case_222 :: fn () -> i32 {
    case_23_out = on case_23_maybe() {
        program => program
        else => 0
    }
    prn($"{case_23_out}")
    return case_23_out.as(i32)
}

-- 227-run-on-expression-block-break-option
case_23_make :: fn (ok: bool) -> ?i32 {
    on ok => {
        value := 42
        return value
    }
    return nil
}

case_23_case_227 :: fn () -> i32 {
    result := case_23_make(yes)
    on result {
        value => {
            on (value != 42) => return 1
            return 0
        }
        else => return 2
    }
}

-- 231-run-on-pattern-same-name-different-payload
Case_23_Payload231 :: plex {
    code i32
}

Case_23_Thing231 :: enum {
    Number(u16)
    Record(Case_23_Payload231)
}

case_23_case_231 :: fn () -> i32 {
    value : Case_23_Thing231 = Case_23_Thing231.Number(0x1234)
    on value {
        Case_23_Thing231.Number(value) => {
            on value == 0x1234 => return 0
            return 1
        }
        Case_23_Thing231.Record(value) => {
            _ := value
            return 2
        }
    }
}

-- 232-run-on-pattern-binder-read-in-condition
Case_23_Payload232 :: plex {
    width  u32
    height u32
}

Case_23_Thing232 :: enum {
    Done(Case_23_Payload232)
    Other
}

case_23_case_232 :: fn () -> i32 {
    value : Case_23_Thing232 = Case_23_Thing232.Done({ width: 1 height: 2 })

    on value {
        Case_23_Thing232.Done(header) => {
            on header.width == 0 || header.height == 0 => return 1
            return 0
        }
        Case_23_Thing232.Other => return 2
    }
}

-- 233-run-generic-on-pattern-binder-read-in-condition
Case_23_Payload233 :: plex {
    width  u32
    height u32
}

case_23_case_233 :: fn () -> i32 {
    -- An earlier enum uses a structurally equivalent payload (Payload232).
    -- Canonicalising these wrappers must preserve optional/result semantics.
    optional: ?Case_23_Payload233 = { width: 3 height: 4 }
    on optional => [present] {
        assert present.width == 3 && present.height == 4
    } else {
        assert no
    }
    value : Case_23_Payload233\string = { width: 1 height: 2 }

    on value => [header] {
        on header.width == 0 || header.height == 0 => return 1
        return 0
    } else {
        return 2
    }
}

-- 234-run-on-expression-branch-binder-result
Case_23_Payload234 :: plex {
    width  u32
    height u32
}

case_23_case_234 :: fn () -> i32 {
    value : Case_23_Payload234\string = { width: 1 height: 2 }

    header := on value => [parsed_header] {
        break parsed_header
    } else {
        return 1
    }

    return (header.width + header.height).as(i32) - 3
}

-- 252-run-nested-partial-on-statement
case_23_case_252 :: fn () -> i32 {
    result := 0

    on yes {
        yes => {
            on yes => {
                result = 42
            }
        }
    }

    return result - 42
}

-- 255-run-llvm-on-value-intermediate-expressions
Case_23_Resolver :: fn () -> bool

case_23_platform_resolve :: fn () => yes

case_23_load_commands :: fn (resolve: Case_23_Resolver = case_23_platform_resolve) -> bool {
    return resolve()
}

case_23_initialise :: fn (context: ?i32) -> bool {
    return on context {
        value => {
            on value == 0 => return no
            on !case_23_load_commands() => return no
            break yes
        }
        else => {
            break no
        }
    }
}

case_23_case_255 :: fn () -> i32 {
    on !case_23_initialise(1) => return 1
    return 0
}

-- 270-run-condition-on-in-expression-block
case_23_case_270 :: fn () -> i32 {
    value := 1
    return on value {
        1 => ${
            break on value < 2 => ${
                break 42
            }
            else ${
                break 0
            }
        }
        else => 0
    }
}

-- 278-run-implicit-on-result-binders
case_23_make_result :: fn (failed: bool) -> i32\string {
    on failed => return "fail"!
    return 21
}

case_23_read_result :: fn (value: i32\string) -> i32 {
    on value => {
        return value * 2
    } else {
        return value.count.as(i32)
    }
}

case_23_read_optional :: fn (value: ?i32) -> i32 {
    on value => {
        return value
    } else {
        assert value == nil
        return 0
    }
}

case_23_check_bool :: fn (flag: bool) {
    on flag => {
        assert flag
    } else {
        assert !flag
    }
}

case_23_case_278 :: fn () -> i32 {
    case_23_check_bool(yes)
    case_23_check_bool(no)
    return case_23_read_result(case_23_make_result(no)) +
           case_23_read_result(case_23_make_result(yes)) +
           case_23_read_optional(7) +
           case_23_read_optional(nil) - 53
}

-- 309-run-on-implicit-binder-mutation
case_23_ready: bool
case_23_step :: fn () {
    on case_23_ready => { case_23_ready = no } else { case_23_ready = yes }
}
case_23_flip :: fn [T] (initial: T) -> T {
    value := initial
    on value => { value = no } else { value = yes }
    return value
}
case_23_case_309 :: fn () {
    case_23_step()
    assert case_23_ready
    case_23_step()
    assert !case_23_ready
    local := no
    on local => { assert no } else { local = yes }
    assert local
    on local => { local = no }
    assert !local
    assert case_23_flip(no)
    assert !case_23_flip(yes)
    absent: ?i32 = nil
    on absent => { assert no } else { absent = 7 }
    on absent => { assert absent == 7 } else { assert no }
    result: i32\string = 42
    on result => { assert result == 42 } else { assert no }
}

case_23_main :: fn () {
    prn("begin 129-run-on-expression-return-branches")
    assert case_23_case_129() == 120
    prn("pass 129-run-on-expression-return-branches")

    prn("begin 172-run-on-enum-payload-branch-return")
    case_23_case_172()
    prn("pass 172-run-on-enum-payload-branch-return")

    prn("begin 221-run-on-expression-return-option-variant")
    assert case_23_case_221() == 0
    prn("pass 221-run-on-expression-return-option-variant")

    prn("begin 222-run-on-expression-assignment-binder-read")
    assert case_23_case_222() == 7
    prn("pass 222-run-on-expression-assignment-binder-read")

    prn("begin 227-run-on-expression-block-break-option")
    assert case_23_case_227() == 0
    prn("pass 227-run-on-expression-block-break-option")

    prn("begin 231-run-on-pattern-same-name-different-payload")
    assert case_23_case_231() == 0
    prn("pass 231-run-on-pattern-same-name-different-payload")

    prn("begin 232-run-on-pattern-binder-read-in-condition")
    assert case_23_case_232() == 0
    prn("pass 232-run-on-pattern-binder-read-in-condition")

    prn("begin 233-run-generic-on-pattern-binder-read-in-condition")
    assert case_23_case_233() == 0
    prn("pass 233-run-generic-on-pattern-binder-read-in-condition")

    prn("begin 234-run-on-expression-branch-binder-result")
    assert case_23_case_234() == 0
    prn("pass 234-run-on-expression-branch-binder-result")

    prn("begin 252-run-nested-partial-on-statement")
    assert case_23_case_252() == 0
    prn("pass 252-run-nested-partial-on-statement")

    prn("begin 255-run-llvm-on-value-intermediate-expressions")
    assert case_23_case_255() == 0
    prn("pass 255-run-llvm-on-value-intermediate-expressions")

    prn("begin 270-run-condition-on-in-expression-block")
    assert case_23_case_270() == 42
    prn("pass 270-run-condition-on-in-expression-block")

    prn("begin 278-run-implicit-on-result-binders")
    assert case_23_case_278() == 0
    prn("pass 278-run-implicit-on-result-binders")

    prn("begin 309-run-on-implicit-binder-mutation")
    case_23_case_309()
    prn("pass 309-run-on-implicit-binder-mutation")
    prn("completed 14 on cases")
}

-- 251-condition-truthiness
case_24_calls: i32 = 0
case_24_side :: fn () -> bool { case_24_calls += 1
    return yes }
case_24_result :: fn (success: bool) -> bool\string {
    on success => return no
    return "failure"!
}
case_24_main :: fn () {
    present: ?i32 = 42
    absent: ?i32 = nil
    false_payload: ?bool = no
    assert present && yes
    assert !(absent && case_24_side()) && case_24_calls == 0
    assert present || case_24_side()
    assert case_24_calls == 0
    assert absent || case_24_side()
    assert case_24_calls == 1
    assert false_payload && yes
    on present && false_payload => { case_24_calls += 1 }
    assert case_24_calls == 2
    success := case_24_result(yes)
    failure := case_24_result(no)
    assert success && yes && !(failure && case_24_side()) && case_24_calls == 2
    assert success || case_24_side()
    assert case_24_calls == 2
    on 7 {
        7 on false_payload => { case_24_calls += 1 }
        else => { assert no }
    }
    on { present => { case_24_calls += 1 } else => { assert no } }
    assert case_24_calls == 4
    for present && case_24_calls < 5 { case_24_calls += 1 }
    assert case_24_calls == 5
    prn("Condition truthiness passed")
}

-- 182-run-nested-on-expression-in-enum-branch
Case_25_Mode :: enum {
    A
}

case_25_min_value :: fn (mode: Case_25_Mode, width: u16, height: u16) -> u32 {
    on mode {
        A => {
            scale_x := width.as(u32)
            scale_y := height.as(u32)
            scale := on scale_x < scale_y => scale_x else scale_y
            return scale
        }
    }

    return 0
}

case_25_main :: fn () -> i32 {
    return case_25_min_value(Case_25_Mode.A, 3, 5).as(i32)
}

-- 189-run-disabled-top-on-declarations
on "missing_platform_key" {
    hidden :: fn (value: MissingType) -> AlsoMissing {
        return value
    }

    impl MissingReceiver {
        ignored :: fn (self: ^Self) -> MissingReturn {
            return self
        }
    }
}

case_26_main :: fn () -> i32 {
    return 7
}

-- 262-run-addressed-local-in-skipped-on
case_27_set_value :: fn (value: ^u64) -> bool {
    value^ = 64
    return yes
}

case_27_conditional_value :: fn (set: bool) -> u64 {
    value : u64 = 0
    on set => {
        on case_27_set_value(^value) => { }
    }
    return value
}

case_27_main :: fn () -> i32 {
    on case_27_conditional_value(yes) != 64 => return 1
    return on case_27_conditional_value(no) == 0 => 0 else 2
}

main :: fn () -> i32 {
    prn("begin 027-on-bool")
    {
        result := case_0_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 42
    }
    prn("begin 028-on-value-branches")
    {
        result := case_1_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 30
    }
    prn("begin 029-on-comma-patterns")
    {
        result := case_2_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 30
    }
    prn("begin 030-on-ranges")
    {
        result := case_3_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 30
    }
    prn("begin 035-on-pattern-binders")
    {
        result := case_4_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 22
    }
    prn("begin 036-on-untyped-local")
    {
        case_5_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 037-on-statement-partial")
    {
        case_6_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 046-on-string-patterns")
    {
        result := case_7_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 2
    }
    prn("begin 060-on-pattern-guards")
    {
        case_8_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 061-on-structural-patterns")
    {
        result := case_9_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 064-on-in-interpolated-string")
    {
        case_10_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 066-generalised-on")
    {
        result := case_11_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 100
    }
    prn("begin 074-qualified-enum-patterns")
    {
        result := case_12_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 30
    }
    prn("begin 098-on-branch-loop-control")
    {
        result := case_13_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 100-on-short-block-expr")
    {
        result := case_14_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 118-nested-on-branch")
    {
        result := case_15_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 2
    }
    prn("begin 144-pointer-equality-and-nil-on")
    {
        result := case_16_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 151-typed-plex-patterns")
    {
        result := case_17_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 1
    }
    prn("begin 152-on-break-for-expression")
    {
        result := case_18_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 153-break-on")
    {
        result := case_19_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 183-pattern-binds-and-for-values")
    {
        result := case_20_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 28
    }
    prn("begin 184-on-postfix-deref-scrutinee")
    {
        result := case_21_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 3
    }
    prn("begin 186-unqualified-enum-comma-patterns")
    {
        result := case_22_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 4
    }
    prn("begin 200-on-regressions")
    {
        case_23_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 251-condition-truthiness")
    {
        case_24_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 182-run-nested-on-expression-in-enum-branch")
    {
        result := case_25_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 3
    }
    prn("begin 189-run-disabled-top-on-declarations")
    {
        result := case_26_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 7
    }
    prn("begin 262-run-addressed-local-in-skipped-on")
    {
        result := case_27_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    return 0
}
¬
0
¬
begin 027-on-bool
result 42
begin 028-on-value-branches
0: 10
1: 20
2: 30
result 30
begin 029-on-comma-patterns
0: 10
1: 10
2: 30
result 30
begin 030-on-ranges
0: 10
1: 10
2: 20
4: 20
5: 30
result 30
begin 035-on-pattern-binders
0: 10
2: 22
5: 35
result 22
begin 036-on-untyped-local
non-zero: 2
result 0
begin 037-on-statement-partial
done
result 0
begin 046-on-string-patterns
red: 1 warm
green: 1 unknown
blue: 2 cool
other: 3 unknown
result 2
begin 060-on-pattern-guards
1: 10
2: 22
10: 100
50: 150
result 0
begin 061-on-structural-patterns
pair zero 100
pair one 8
pair nine 40
pair else 0
point origin 2
point x 40
point fallback 50
point name named
result 0
begin 064-on-in-interpolated-string
Colour = red
result 0
begin 066-generalised-on
scores -1 0 10 20 100
descriptions negative zero small large
not-five 1 0
result 100
begin 074-qualified-enum-patterns
result 30
begin 098-on-branch-loop-control
result 0
begin 100-on-short-block-expr
result 0
begin 118-nested-on-branch
hit 2
result 2
begin 144-pointer-equality-and-nil-on
result 0
begin 151-typed-plex-patterns
typed plex patterns 1 2 0
result 1
begin 152-on-break-for-expression
result 0
begin 153-break-on
result 0
begin 183-pattern-binds-and-for-values
pattern syntax 10 4 9 5
result 28
begin 184-on-postfix-deref-scrutinee
postfix on 1 2
result 3
begin 186-unqualified-enum-comma-patterns
result 4
begin 200-on-regressions
begin 129-run-on-expression-return-branches
120
pass 129-run-on-expression-return-branches
begin 172-run-on-enum-payload-branch-return
10x20
pass 172-run-on-enum-payload-branch-return
begin 221-run-on-expression-return-option-variant
pass 221-run-on-expression-return-option-variant
begin 222-run-on-expression-assignment-binder-read
7
pass 222-run-on-expression-assignment-binder-read
begin 227-run-on-expression-block-break-option
pass 227-run-on-expression-block-break-option
begin 231-run-on-pattern-same-name-different-payload
pass 231-run-on-pattern-same-name-different-payload
begin 232-run-on-pattern-binder-read-in-condition
pass 232-run-on-pattern-binder-read-in-condition
begin 233-run-generic-on-pattern-binder-read-in-condition
pass 233-run-generic-on-pattern-binder-read-in-condition
begin 234-run-on-expression-branch-binder-result
pass 234-run-on-expression-branch-binder-result
begin 252-run-nested-partial-on-statement
pass 252-run-nested-partial-on-statement
begin 255-run-llvm-on-value-intermediate-expressions
pass 255-run-llvm-on-value-intermediate-expressions
begin 270-run-condition-on-in-expression-block
pass 270-run-condition-on-in-expression-block
begin 278-run-implicit-on-result-binders
pass 278-run-implicit-on-result-binders
begin 309-run-on-implicit-binder-mutation
pass 309-run-on-implicit-binder-mutation
completed 14 on cases
result 0
begin 251-condition-truthiness
Condition truthiness passed
result 0
begin 182-run-nested-on-expression-in-enum-branch
result 3
begin 189-run-disabled-top-on-declarations
result 7
begin 262-run-addressed-local-in-skipped-on
result 0

¬

¬


=== case: 072-top-level-on ===
on "debug" {
    use std.io
    answer :: 7

    on !"release" {
        mode_answer :: 11
    }

    on "x64" {
        arch_answer :: 13
    }

    on !"feature" {
        fallback :: 9
    }
}

main :: fn () {
    prn($"answer={answer}")
    prn($"mode={mode_answer}")
    prn($"arch={arch_answer}")
    prn($"fallback={fallback}")
}
¬
0
¬
answer=7
mode=11
arch=13
fallback=9

¬
hir 0
module module.branching.input(branching.input)
import module.std.io(std.io)
import import.0 prn from module.core(core).decl.N: <unknown>
import import.1 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.2 prn_text from module.core(core).decl.N: fn (string) -> void
import import.3 prn_empty from module.core(core).decl.N: fn () -> void
bind prn = import.0
bind input = import.1
bind prn_text = import.2
bind prn_empty = import.3
bind answer = value.0
bind mode_answer = value.1
bind arch_answer = value.2
bind fallback = value.3
bind main = fn.0
const value.0: untyped integer = untyped integer 7
const value.1: untyped integer = untyped integer 11
const value.2: untyped integer = untyped integer 13
const value.3: untyped integer = untyped integer 9
func fn.0() -> void {
  expr void call bind.2(prn_text)(string interpolate(<unknown> "answer=", untyped integer bind.4(answer)))
  expr void call bind.2(prn_text)(string interpolate(<unknown> "mode=", untyped integer bind.5(mode_answer)))
  expr void call bind.2(prn_text)(string interpolate(<unknown> "arch=", untyped integer bind.6(arch_answer)))
  expr void call bind.2(prn_text)(string interpolate(<unknown> "fallback=", untyped integer bind.7(fallback)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [34 x i8] c"tests/language/branching.t\00"
@.str.m0.0 = private unnamed_addr constant [6 x i8] c"debug\00"
@.str.m0.1 = private unnamed_addr constant [8 x i8] c"release\00"
@.str.m0.2 = private unnamed_addr constant [4 x i8] c"x64\00"
@.str.m0.3 = private unnamed_addr constant [8 x i8] c"feature\00"
@.str.m0.4 = private unnamed_addr constant [8 x i8] c"answer=\00"
@.str.m0.5 = private unnamed_addr constant [6 x i8] c"mode=\00"
@.str.m0.6 = private unnamed_addr constant [6 x i8] c"arch=\00"
@.str.m0.7 = private unnamed_addr constant [10 x i8] c"fallback=\00"

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
  %t8 = alloca { ptr, i64 }
  %t14 = alloca { ptr, i64 }
  %t20 = alloca { ptr, i64 }
  %t0 = call i64 @nrt_string_builder_mark()
  %t1 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 7 }, ptr %t2
  call void @nrt_to_string_string(ptr %t1, ptr %t2)
  call void @nrt_string_builder_append_string(ptr %t1)
  %t3 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t3, i32 7)
  call void @nrt_string_builder_append_string(ptr %t3)
  %t4 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t4, i64 %t0)
  %t5 = load { ptr, i64 }, ptr %t4
  call void @$prn_text({ ptr, i64 } %t5)
  %t6 = call i64 @nrt_string_builder_mark()
  %t7 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.5, i64 5 }, ptr %t8
  call void @nrt_to_string_string(ptr %t7, ptr %t8)
  call void @nrt_string_builder_append_string(ptr %t7)
  %t9 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t9, i32 11)
  call void @nrt_string_builder_append_string(ptr %t9)
  %t10 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t10, i64 %t6)
  %t11 = load { ptr, i64 }, ptr %t10
  call void @$prn_text({ ptr, i64 } %t11)
  %t12 = call i64 @nrt_string_builder_mark()
  %t13 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.6, i64 5 }, ptr %t14
  call void @nrt_to_string_string(ptr %t13, ptr %t14)
  call void @nrt_string_builder_append_string(ptr %t13)
  %t15 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t15, i32 13)
  call void @nrt_string_builder_append_string(ptr %t15)
  %t16 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t16, i64 %t12)
  %t17 = load { ptr, i64 }, ptr %t16
  call void @$prn_text({ ptr, i64 } %t17)
  %t18 = call i64 @nrt_string_builder_mark()
  %t19 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.7, i64 9 }, ptr %t20
  call void @nrt_to_string_string(ptr %t19, ptr %t20)
  call void @nrt_string_builder_append_string(ptr %t19)
  %t21 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t21, i32 9)
  call void @nrt_string_builder_append_string(ptr %t21)
  %t22 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t22, i64 %t18)
  %t23 = load { ptr, i64 }, ptr %t22
  call void @$prn_text({ ptr, i64 } %t23)
  ret void
}

@$main = hidden alias void (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)

=== case: 026-run-llvm-partial-on ===
use std.io

main :: fn () {
    value :: 2

    on value {
        1 => prn("one")
    }

    prn("done")
}
¬
0
¬
done

¬

¬

=== case: 028-run-llvm-string-patterns ===
choice :: "blue"

classify :: fn (choice: string) -> i32 {
    return on choice {
        "red", "green" => 1
        "blue" => 2
        else => 3
    }
}

main :: fn () => classify(choice)
¬
2
¬

¬

¬

=== case: 042-run-llvm-on-pattern-binders ===
use std.io

Pair :: plex {
    x i32
    y i32
}

score :: fn (size: u32) -> u32 {
    return on size {
        0..3 as matched on matched == 2 => matched + 20
        0..3 => 10
        10 as size on size == 10 => size * 10
        else as size => size + 100
    }
}

pair_score :: fn (pair: Pair) -> i32 {
    return on pair {
        { x: 0, y: as y } => y + 100
        { x: as x, y: 1 } => x
        else => 0
    }
}

main :: fn () {
    prn($"1: {score(1)}")
    prn($"2: {score(2)}")
    prn($"10: {score(10)}")
    prn($"50: {score(50)}")
    prn($"pair: {pair_score(Pair { x: 0, y: 7 })}")
}
¬
0
¬
1: 10
2: 22
10: 100
50: 150
pair: 107

¬

¬

=== case: 043-run-llvm-enum-on-values ===
use std.io

Direction :: enum {
    NORTH
    EAST
    SOUTH
    WEST
    NO_DIRECTION
}

direction_from_word :: fn (word: string) -> Direction {
    return on word {
        "north", "n" => NORTH
        "east",  "e" => EAST
        "south", "s" => SOUTH
        "west",  "w" => WEST
        else => NO_DIRECTION
    }
}

direction_name :: fn (direction: Direction) -> string {
    return on direction {
        NORTH => "north"
        EAST => "east"
        SOUTH => "south"
        WEST => "west"
        else => "none"
    }
}

main :: fn () -> i32 {
    Choice :: enum { Left(i32) Right(i32) }
    choice: Choice = Right(5)
    extra := on choice {
        Left(n) => n
        Right(n) => n
    }

    prn(direction_name(direction_from_word("n")))
    prn($"extra = {extra}")
    return extra
}
¬
5
¬
north
extra = 5

¬

¬

=== case: 067-run-llvm-typed-plex-patterns ===
use std.io

Event :: enum {
    KeyDown(KeyEvent)
    KeyUp(KeyEvent)
}

KeyEvent :: plex {
    keycode   Key
    modifiers i32
    char      u32
}

Key :: enum {
    Escape = 41
    Other  = 42
}

score :: fn (event: Event) -> i32 {
    return on event {
        KeyDown(KeyEvent { keycode: Escape }) => 1
        KeyDown(KeyEvent { keycode: Other }) => 2
        else => 0
    }
}

main :: fn () -> i32 {
    escape := Event.KeyDown(KeyEvent { keycode: Escape, modifiers: 0, char: 0 })
    other := Event.KeyDown(KeyEvent { keycode: Other, modifiers: 1, char: 2 })
    up := Event.KeyUp(KeyEvent { keycode: Escape, modifiers: 0, char: 0 })
    result := score(escape) * 100 + score(other) * 10 + score(up)
    prn($"{result}")
    return 0
}
¬
0
¬
120

¬

¬

=== case: 099-run-llvm-runtime-pattern-values ===
use std.io

main :: fn () {
    details := (42, "matt")
    name := "matt"

    on details {
        (42, for name) => prn($"Hello {name}!")
        else => prn("Whatever!")
    }

    other := "jane"
    on details {
        (42, for other) => prn("wrong")
        (42, matched) => prn($"Matched {matched}.")
    }
}
¬
0
¬
Hello matt!
Matched matt.

¬

¬

=== case: 117-run-break-in-on-branch ===
use std.io

main :: fn () -> i32 {
    count := 0
    for {
        count += 1
        on count == 1 => {
            prn("breaking")
            break
        } else {
            prn("else")
        }
        on count > 3 => return 2
    }
    prn("done")
    return 0
}
¬
0
¬
breaking
done

¬

¬

=== case: 128-run-pattern-binder-typed-local ===
use std.io

read :: fn (value: ?u8) -> u8 {
    on value {
        byte => {
            copied : u8 = byte
            return copied
        }
        else => {}
    }
    return 0
}

main :: fn () -> i32 {
    result := read('x')
    prn($"{result}")
    return result.as(i32)
}
¬
120
¬
120

¬

¬

=== case: 195-run-pattern-binder-qualified-enum-constructor ===
Key :: enum {
    A = 65
}

Maybe :: enum {
    None
    Some(Key)
}

Event :: enum {
    None
    KeyPress(Key)
}

main :: fn () -> i32 {
    maybe := Maybe.Some(Key.A)
    event := on maybe {
        Some(code) => Event.KeyPress(code)
        None => Event.None
    }

    return on event {
        KeyPress(code) => code.as(i32) - 65
        None => 1
    }
}
¬
0
¬

¬

¬

=== case: 318-grouped-payload-patterns ===
Key :: enum { Q Escape Other }
Event :: enum { Press { key Key } None }

matches :: fn (event: Event) -> bool {
    return on event {
        Press { key: Q }, Press { key: Escape } => yes
        else => no
    }
}

main :: fn () {
    assert matches(Event.Press { key: Q })
    assert matches(Event.Press { key: Escape })
    assert !matches(Event.Press { key: Other })
    assert !matches(Event.None)
}
¬
0
¬

¬

¬

=== case: 355-run-on-extraction-shorthand ===
-- test-release
Failure :: enum { Bad Other }
Translated :: enum { Converted }
State :: plex { calls i32 handled i32 defers i32 undos i32 handler_defers i32 handler_undos i32 }
operation :: fn (state: ^State, ok: bool) -> i32\Failure {
    state.calls += 1
    on !ok => return Failure.Bad!
    return 7
}
forward :: fn (state: ^State, ok: bool) -> i32\Failure {
    defer state.defers += 1
    undo state.undos += 1
    value := operation(state, ok) else [error] {
        state.handled += 1
        defer state.handler_defers += 1
        undo state.handler_undos += 1
        assert error == Failure.Bad
    }
    return value
}
translate :: fn (state: ^State) -> i32\Translated {
    value := operation(state, no) else [error] {
        assert error == Failure.Bad
        return Translated.Converted!
    }
    return value
}
recover :: fn (state: ^State) -> i32\Failure {
    value := operation(state, no) else { return 4 }
    return value
}
optional :: fn (ok: bool) -> ?i32 {
    on !ok => return nil
    return 9
}
optional_forward :: fn (state: ^State, ok: bool) -> ?i32 {
    value := optional(ok) else { state.handled += 1 }
    return value
}
optional_plain :: fn (ok: bool) -> i32 {
    value := optional(ok) else { return 6 }
    return value
}
void_operation :: fn (ok: bool) -> void\Failure {
    on !ok => return Failure.Bad!
}
void_forward :: fn (ok: bool) -> void\Failure {
    void_operation(ok) else {}
}
Record :: plex { number i32 }
record_operation :: fn () -> Record\Failure { return Record { number: 11 } }
record_forward :: fn () -> i32\Failure {
    record := record_operation() else {}
    return record.number
}
inferred :: fn () => record_operation() else {}
cached_forward :: fn (state: ^State) -> i32\Failure {
    cached := operation(state, no)
    value := cached else [error] {
        assert error == Failure.Bad
        cached = 8
    }
    return value
}
implicit_error :: fn (state: ^State) -> i32\Failure {
    cached := operation(state, no)
    value := cached else { assert cached == Failure.Bad }
    return value
}

panic_handler :: fn (state: ^State) -> i32 {
    value := operation(state, yes) else { abort("unexpected failure") }
    return value
}
exhaustive_handler :: fn (state: ^State, choice: bool) -> i32 {
    value := operation(state, no) else {
        on choice { yes => return 13 no => return 14 }
    }
    return value
}

run :: fn () -> void\Failure {
    state : State
    assert record_forward()? == 11
    assert inferred()?.number == 11
    assert forward(^state, yes)? == 7
    assert state.calls == 1 && state.handled == 0
    on forward(^state, no) {
        error! => { assert error == Failure.Bad }
        else => { assert no }
    }
    assert state.calls == 2 && state.handled == 1
    assert state.defers == 2 && state.undos == 1
    assert state.handler_defers == 1 && state.handler_undos == 1
    on translate(^state) { error! => { assert error == Translated.Converted } else => { assert no } }
    assert recover(^state)? == 4
    assert optional_plain(yes) == 9 && optional_plain(no) == 6
    present := optional_forward(^state, yes) else { return Failure.Other! }
    assert present == 9
    assert !optional_forward(^state, no)
    void_forward(yes)?
    on void_forward(no) { error! => { assert error == Failure.Bad } else => { assert no } }
    attempts := 0
    for attempts < 3 {
        attempts += 1
        value := optional(no) else { again }
        assert value == 0
    }
    assert attempts == 3
    for {
        value := optional(no) else { break }
        assert value == 0
    }
    attempts = 0
    for attempts < 3 {
        attempts += 1
        value := optional(no) else { again }
        assert value == 0
    }
    assert attempts == 3
    for {
        value := optional(no) else { break }
        assert value == 0
    }
    ignored := ${
        value := optional(no) else { break 15 }
        break value
    }
    assert ignored == 15
    outer := ${
        value := optional(no) else { break 12 }
        break value
    }
    assert outer == 12
    _ := operation(^state, yes) else { return Failure.Other! }
    assert panic_handler(^state) == 7
    assert exhaustive_handler(^state, yes) == 13
    assert exhaustive_handler(^state, no) == 14
    on cached_forward(^state) {
        error! => { assert error == Failure.Bad }
        else => { assert no }
    }
    on implicit_error(^state) {
        error! => { assert error == Failure.Bad }
        else => { assert no }
    }
    prn("bare else handler passed")
}
main :: fn () -> i32 {
    on run() { _! => return 1 else => return 0 }
}
¬
0
¬
bare else handler passed
¬

¬
