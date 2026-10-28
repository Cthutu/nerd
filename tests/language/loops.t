-- loops feature scenarios. Results and stdout are checked in order.
use std.io

-- 038-for-infinite
case_0_main :: fn () -> i32 {
    for {
        return 7
    }
}

-- 041-for-while
case_1_main :: fn () {
    i := 0
    for i < 5 {
        prn($"While {i}")
        i += 1
    }
}

-- 042-for-c-style
case_2_main :: fn () -> i32 {
    total := 0
    for i := 0, step := 1; i < 5; total += step, i += 1 {
        prn($"C {i}")
    }
    return total
}

-- 043-for-c-style-empty-init
case_3_main :: fn () {
    i := 0
    for ; i < 3; i += 1 {
        prn($"N {i}")
    }
}

-- 044-for-break-again
case_4_main :: fn () -> i32 {
    total := 0
    for i := 0; i < 6; i += 1 {
        on i == 2 => again
        on i == 5 => break
        total += i
    }
    return total
}

-- 076-for-in-and-deref
case_7_main :: fn () -> i32 {
    values: [3]i32 = [1, 2, 3]

    for _item in values[..] {
    }

    total := 0
    for item in values[..] {
        total += item^
    }

    text := "ab"
    count := 0
    sum := 0
    for ch in text {
        count += 1
        sum += ch^.as(i32)
    }

    ptr: ^i32 = ^values[1]
    prn($"{ptr^} {total} {count} {sum}")
    return ptr^ + total + count
}

-- 116-indexed-for-in
-- Iterates with both a usize index and an element binding.
Case_8_Location :: plex {
    description string
    tag         string
}

case_8_locs: []Case_8_Location = [
    { description : "d", tag : "a" },
]

case_8_main :: fn () -> i32 {
    noun := "a"
    for i, loc in case_8_locs {
        on noun == loc.tag => return i.as(i32)
    }
    return 0
}

-- 117-interpolated-for-field
-- Interpolates a field read from an indexed for-in item.

Case_9_Location :: plex {
    description string
}

case_9_locs: []Case_9_Location = [
    { description : "field" },
]

case_9_main :: fn () {
    player_loc: usize = 1
    for i, loc in case_9_locs {
        on {
            i == player_loc => prn("same")
            else => prn($"You go to {loc.description}.")
        }
    }
}

-- 157-for-in-ranges
case_10_main :: fn () -> i32 {
    total := 0
    for value in [0..10] {
        total += value
    }

    for index, value in [2..=4] {
        total += index.as(i32) + value
    }

    return total
}

-- 182-for-in-postfix-deref-body
case_11_main :: fn () -> usize {
    groups := [["quit", "q"]]
    result: usize = 0
    for i, group in groups {
        for item in group^ {
            on item^ == "q" => result = i + 1
        }
    }
    return result
}

-- 183-for-in-fixed-array
case_12_main :: fn () {
    names := ["Alice", "Bob", "Charlie"]
    for name in names {
        prn(name^)
    }
}

-- 198-loop-dynamic-array-count
case_13_main :: fn () -> i32 {
    values : [..]u32 = nil
    defer values.free()
    values.resize_to(120000)

    for i: usize = 0; i < values.count; i += 1 {
        values[i] = i.as(u32)
    }

    return on values[119999] == 119999 => 0 else 1
}

-- 098-run-llvm-for-in-ranges
case_14_main :: fn () {
    for value in [0..3] {
        prn($"{value}")
    }
    for value in [3..=4] {
        prn($"{value}")
    }
}

-- 150-run-for-init-infers-from-condition-bound
case_15_main :: fn () -> i32 {
    width: u32 = 3
    height: u32 = 2
    num_bytes := width * height
    total := 0

    for i := 0; i < num_bytes; i += 1 {
        total += i.as(i32)
    }

    on total != 15 => return 1
    return 0
}

-- 310-run-return-after-iteration
Case_16_Failure :: enum { Missing }
case_16_lookup :: fn (values: []i32, wanted: i32) -> i32\Case_16_Failure {
    for value in values {
        on value^ == wanted => return value^
    }
    return (Case_16_Failure.Missing)!
}
case_16_main :: fn () {
    values: []i32 = [2, 4, 6]
    found := case_16_lookup(values, 4)
    on found => { assert found == 4 } else { assert no }
    missing := case_16_lookup(values, 5)
    on missing => { assert no } else { assert missing == Case_16_Failure.Missing }
    empty: []i32
    absent := case_16_lookup(empty, 4)
    on absent => { assert no } else { assert absent == Case_16_Failure.Missing }
}

main :: fn () -> i32 {
    prn("begin 038-for-infinite")
    {
        result := case_0_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 7
    }
    prn("begin 041-for-while")
    {
        case_1_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 042-for-c-style")
    {
        result := case_2_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 5
    }
    prn("begin 043-for-c-style-empty-init")
    {
        case_3_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 044-for-break-again")
    {
        result := case_4_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 8
    }
    prn("begin 076-for-in-and-deref")
    {
        result := case_7_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 10
    }
    prn("begin 116-indexed-for-in")
    {
        result := case_8_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 117-interpolated-for-field")
    {
        case_9_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 157-for-in-ranges")
    {
        result := case_10_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 57
    }
    prn("begin 182-for-in-postfix-deref-body")
    {
        result := case_11_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 1
    }
    prn("begin 183-for-in-fixed-array")
    {
        case_12_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 198-loop-dynamic-array-count")
    {
        result := case_13_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 098-run-llvm-for-in-ranges")
    {
        case_14_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 150-run-for-init-infers-from-condition-bound")
    {
        result := case_15_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 310-run-return-after-iteration")
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
begin 038-for-infinite
result 7
begin 041-for-while
While 0
While 1
While 2
While 3
While 4
result 0
begin 042-for-c-style
C 0
C 1
C 2
C 3
C 4
result 5
begin 043-for-c-style-empty-init
N 0
N 1
N 2
result 0
begin 044-for-break-again
result 8
begin 076-for-in-and-deref
2 6 2 195
result 10
begin 116-indexed-for-in
result 0
begin 117-interpolated-for-field
You go to field.
result 0
begin 157-for-in-ranges
result 57
begin 182-for-in-postfix-deref-body
result 1
begin 183-for-in-fixed-array
Alice
Bob
Charlie
result 0
begin 198-loop-dynamic-array-count
result 0
begin 098-run-llvm-for-in-ranges
0
1
2
3
4
result 0
begin 150-run-for-init-infers-from-condition-bound
result 0
begin 310-run-return-after-iteration
result 0

¬

¬


=== case: 049-labelled-loop-expressions ===
use std.io

main :: fn () {
    total := 0
    for i := 0; i < 5; i += 1 $outer {
        for j := 0; j < 5; j += 1 $inner {
            on j == 1 => again $outer
            total += 100
        }
        total += i
    }

    for $done {
        break $done
    }

    value :: for {
        break 7
    }
    labelled_value :: for $value_loop {
        break $value_loop 11
    }

    prn($"total = {total}")
    prn($"value = {value}")
    prn($"labelled = {labelled_value}")
    return total + value + labelled_value
}
¬
6
¬
total = 500
value = 7
labelled = 11

¬
hir 0
module module.loops.input(loops.input)
import module.std.io(std.io)
import import.0 prn from module.core(core).decl.N: <unknown>
import import.1 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.2 prn_text from module.core(core).decl.N: fn (string) -> void
import import.3 prn_empty from module.core(core).decl.N: fn () -> void
bind prn = import.0
bind input = import.1
bind prn_text = import.2
bind prn_empty = import.3
bind main = fn.0
func fn.0() -> i32 {
  let total: i32 = untyped integer 0
  expr void for c_style $outer {
    init {
      let i: i32 = untyped integer 0
    }
    condition bool less(i32 local.3(i), i32 5)
    body {
      expr void for c_style $inner {
    init {
      let j: i32 = untyped integer 0
    }
    condition bool less(i32 local.4(j), i32 5)
    body {
      expr void on bool equal(i32 local.4(j), i32 1) {
    value(bool yes) => {
      again $outer
    }
  }
      assign i32 local.2(total) = i32 add(i32 local.2(total), i32 100)
    }
    update {
      assign i32 local.4(j) = i32 add(i32 local.4(j), i32 1)
    }
  }
      assign i32 local.2(total) = i32 add(i32 local.2(total), i32 local.3(i))
    }
    update {
      assign i32 local.3(i) = i32 add(i32 local.3(i), i32 1)
    }
  }
  expr void for condition $done {
    body {
      break $done
    }
  }
  let value: untyped integer = untyped integer for condition {
    body {
      break untyped integer 7
    }
  }
  let labelled_value: untyped integer = untyped integer for condition $value_loop {
    body {
      break $value_loop untyped integer 11
    }
  }
  expr void call bind.2(prn_text)(string interpolate(<unknown> "total = ", i32 local.2(total)))
  expr void call bind.2(prn_text)(string interpolate(<unknown> "value = ", untyped integer local.0(value)))
  expr void call bind.2(prn_text)(string interpolate(<unknown> "labelled = ", untyped integer local.1(labelled_value)))
  return i32 add(i32 add(i32 local.2(total), i32 local.0(value)), i32 local.1(labelled_value))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [47 x i8] c"tests/language/loops.t\00"
@.str.m0.0 = private unnamed_addr constant [9 x i8] c"total = \00"
@.str.m0.1 = private unnamed_addr constant [9 x i8] c"value = \00"
@.str.m0.2 = private unnamed_addr constant [12 x i8] c"labelled = \00"

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

define internal i32 @fn.0() {
  %local.2 = alloca i32
  %local.3 = alloca i32
  %local.4 = alloca i32
  %t22 = alloca { ptr, i64 }
  %t29 = alloca { ptr, i64 }
  %t35 = alloca { ptr, i64 }
  store i32 0, ptr %local.2
  store i32 0, ptr %local.3
  br label %for.cond.0
for.cond.0:
  %t0 = load i32, ptr %local.3
  %t1 = icmp slt i32 %t0, 5
  br i1 %t1, label %for.body.1, label %for.end.3
for.body.1:
  store i32 0, ptr %local.4
  br label %for.cond.4
for.cond.4:
  %t2 = load i32, ptr %local.4
  %t3 = icmp slt i32 %t2, 5
  br i1 %t3, label %for.body.5, label %for.end.7
for.body.5:
  %t4 = load i32, ptr %local.4
  %t5 = icmp eq i32 %t4, 1
  %t6 = icmp eq i1 %t5, 1
  br i1 %t6, label %on.body.9, label %on.end.8
on.body.9:
  br label %for.update.2
on.end.8:
  %t7 = load i32, ptr %local.2
  %t8 = add i32 %t7, 100
  store i32 %t8, ptr %local.2
  br label %for.update.6
for.update.6:
  %t9 = load i32, ptr %local.4
  %t10 = add i32 %t9, 1
  store i32 %t10, ptr %local.4
  br label %for.cond.4
for.end.7:
  %t11 = load i32, ptr %local.2
  %t12 = load i32, ptr %local.3
  %t13 = add i32 %t11, %t12
  store i32 %t13, ptr %local.2
  br label %for.update.2
for.update.2:
  %t14 = load i32, ptr %local.3
  %t15 = add i32 %t14, 1
  store i32 %t15, ptr %local.3
  br label %for.cond.0
for.end.3:
  br label %for.cond.10
for.cond.10:
  br label %for.body.11
for.body.11:
  br label %for.end.13
for.end.13:
  %t16 = alloca i32, align 4
  store i32 0, ptr %t16, align 4
  br label %for.cond.14
for.cond.14:
  br label %for.body.15
for.body.15:
  store i32 7, ptr %t16, align 4
  br label %for.end.18
for.end.18:
  %t17 = load i32, ptr %t16, align 4
  %t18 = alloca i32, align 4
  store i32 0, ptr %t18, align 4
  br label %for.cond.19
for.cond.19:
  br label %for.body.20
for.body.20:
  store i32 11, ptr %t18, align 4
  br label %for.end.23
for.end.23:
  %t19 = load i32, ptr %t18, align 4
  %t20 = call i64 @nrt_string_builder_mark()
  %t21 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 8 }, ptr %t22
  call void @nrt_to_string_string(ptr %t21, ptr %t22)
  call void @nrt_string_builder_append_string(ptr %t21)
  %t23 = load i32, ptr %local.2
  %t24 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t24, i32 %t23)
  call void @nrt_string_builder_append_string(ptr %t24)
  %t25 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t25, i64 %t20)
  %t26 = load { ptr, i64 }, ptr %t25
  call void @$prn_text({ ptr, i64 } %t26)
  %t27 = call i64 @nrt_string_builder_mark()
  %t28 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 8 }, ptr %t29
  call void @nrt_to_string_string(ptr %t28, ptr %t29)
  call void @nrt_string_builder_append_string(ptr %t28)
  %t30 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t30, i32 %t17)
  call void @nrt_string_builder_append_string(ptr %t30)
  %t31 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t31, i64 %t27)
  %t32 = load { ptr, i64 }, ptr %t31
  call void @$prn_text({ ptr, i64 } %t32)
  %t33 = call i64 @nrt_string_builder_mark()
  %t34 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 11 }, ptr %t35
  call void @nrt_to_string_string(ptr %t34, ptr %t35)
  call void @nrt_string_builder_append_string(ptr %t34)
  %t36 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t36, i32 %t19)
  call void @nrt_string_builder_append_string(ptr %t36)
  %t37 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t37, i64 %t33)
  %t38 = load { ptr, i64 }, ptr %t37
  call void @$prn_text({ ptr, i64 } %t38)
  %t39 = load i32, ptr %local.2
  %t40 = add i32 %t39, %t17
  %t41 = add i32 %t40, %t19
  ret i32 %t41
}

@$main = hidden alias i32 (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)

=== case: 050-for-else ===
use std.io

main :: fn () {
    total := 0
    found :: for i := 0; i < 1; i += 1 {
        total += i
        break total + 7
    } else {
        break -1
    }
    missing :: for j := 0; j < 0; j += 1 {
        break j
    } else {
        break 42
    }

    prn($"found = {found}")
    prn($"missing = {missing}")
    return found + missing
}
¬
49
¬
found = 7
missing = 42

¬
hir 0
module module.loops.input(loops.input)
import module.std.io(std.io)
import import.0 prn from module.core(core).decl.N: <unknown>
import import.1 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.2 prn_text from module.core(core).decl.N: fn (string) -> void
import import.3 prn_empty from module.core(core).decl.N: fn () -> void
bind prn = import.0
bind input = import.1
bind prn_text = import.2
bind prn_empty = import.3
bind main = fn.0
func fn.0() -> i32 {
  let total: i32 = untyped integer 0
  let found: i32 = i32 for c_style {
    init {
      let i: i32 = untyped integer 0
    }
    condition bool less(i32 local.3(i), i32 1)
    body {
      assign i32 local.2(total) = i32 add(i32 local.2(total), i32 local.3(i))
      break i32 add(i32 local.2(total), i32 7)
    }
    update {
      assign i32 local.3(i) = i32 add(i32 local.3(i), i32 1)
    }
    else {
      break i32 negate(i32 1)
    }
  }
  let missing: i32 = i32 for c_style {
    init {
      let j: i32 = untyped integer 0
    }
    condition bool less(i32 local.4(j), i32 0)
    body {
      break i32 local.4(j)
    }
    update {
      assign i32 local.4(j) = i32 add(i32 local.4(j), i32 1)
    }
    else {
      break i32 42
    }
  }
  expr void call bind.2(prn_text)(string interpolate(<unknown> "found = ", i32 local.0(found)))
  expr void call bind.2(prn_text)(string interpolate(<unknown> "missing = ", i32 local.1(missing)))
  return i32 add(i32 local.0(found), i32 local.1(missing))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [30 x i8] c"tests/language/loops.t\00"
@.str.m0.0 = private unnamed_addr constant [9 x i8] c"found = \00"
@.str.m0.1 = private unnamed_addr constant [11 x i8] c"missing = \00"

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

define internal i32 @fn.0() {
  %local.2 = alloca i32
  %local.3 = alloca i32
  %local.4 = alloca i32
  %t21 = alloca { ptr, i64 }
  %t27 = alloca { ptr, i64 }
  store i32 0, ptr %local.2
  store i32 0, ptr %local.3
  %t0 = alloca i32, align 4
  store i32 0, ptr %t0, align 4
  br label %for.cond.0
for.cond.0:
  %t1 = load i32, ptr %local.3
  %t2 = icmp slt i32 %t1, 1
  br i1 %t2, label %for.body.1, label %for.else.3
for.body.1:
  %t3 = load i32, ptr %local.2
  %t4 = load i32, ptr %local.3
  %t5 = add i32 %t3, %t4
  store i32 %t5, ptr %local.2
  %t6 = load i32, ptr %local.2
  %t7 = add i32 %t6, 7
  store i32 %t7, ptr %t0, align 4
  br label %for.end.4
for.update.2:
  %t8 = load i32, ptr %local.3
  %t9 = add i32 %t8, 1
  store i32 %t9, ptr %local.3
  br label %for.cond.0
for.else.3:
  %t10 = sub i32 0, 1
  store i32 %t10, ptr %t0, align 4
  br label %for.end.4
for.end.4:
  %t11 = load i32, ptr %t0, align 4
  store i32 0, ptr %local.4
  %t12 = alloca i32, align 4
  store i32 0, ptr %t12, align 4
  br label %for.cond.5
for.cond.5:
  %t13 = load i32, ptr %local.4
  %t14 = icmp slt i32 %t13, 0
  br i1 %t14, label %for.body.6, label %for.else.8
for.body.6:
  %t15 = load i32, ptr %local.4
  store i32 %t15, ptr %t12, align 4
  br label %for.end.9
for.update.7:
  %t16 = load i32, ptr %local.4
  %t17 = add i32 %t16, 1
  store i32 %t17, ptr %local.4
  br label %for.cond.5
for.else.8:
  store i32 42, ptr %t12, align 4
  br label %for.end.9
for.end.9:
  %t18 = load i32, ptr %t12, align 4
  %t19 = call i64 @nrt_string_builder_mark()
  %t20 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 8 }, ptr %t21
  call void @nrt_to_string_string(ptr %t20, ptr %t21)
  call void @nrt_string_builder_append_string(ptr %t20)
  %t22 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t22, i32 %t11)
  call void @nrt_string_builder_append_string(ptr %t22)
  %t23 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t23, i64 %t19)
  %t24 = load { ptr, i64 }, ptr %t23
  call void @$prn_text({ ptr, i64 } %t24)
  %t25 = call i64 @nrt_string_builder_mark()
  %t26 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 10 }, ptr %t27
  call void @nrt_to_string_string(ptr %t26, ptr %t27)
  call void @nrt_string_builder_append_string(ptr %t26)
  %t28 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t28, i32 %t18)
  call void @nrt_string_builder_append_string(ptr %t28)
  %t29 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t29, i64 %t25)
  %t30 = load { ptr, i64 }, ptr %t29
  call void @$prn_text({ ptr, i64 } %t30)
  %t31 = add i32 %t11, %t18
  ret i32 %t31
}

@$main = hidden alias i32 (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)

=== case: 041-run-llvm-labelled-loops ===
use std.io

main :: fn () {
    total := 0
    for i := 0; i < 5; i += 1 $outer {
        for j := 0; j < 5; j += 1 $inner {
            on j == 1 => again $outer
            total += 100
        }
        total += i
    }

    value :: for {
        break 7
    }
    labelled_value :: for $value_loop {
        break $value_loop 11
    }

    prn($"total = {total}")
    prn($"value = {value}")
    prn($"labelled = {labelled_value}")
    return total + value + labelled_value
}
¬
6
¬
total = 500
value = 7
labelled = 11

¬

¬

=== case: 050-run-llvm-for-in-string ===
use std.io

main :: fn () -> i32 {
    values: [3]i32 = [1, 2, 3]

    total := 0
    for item in values[..] {
        total += item^
    }

    text := "ab"
    count := 0
    sum := 0
    for ch in text {
        count += 1
        sum += ch^.as(i32)
    }

    ptr: ^i32 = ^values[1]
    prn($"{ptr^} {total} {count} {sum}")
    return ptr^ + total + count
}
¬
10
¬
2 6 2 195

¬

¬

=== case: 075-run-llvm-dynarray-for-in ===
main :: fn () -> i32 {
    empty: [..]i32
    seen := 0
    for item in empty {
        seen += item^
    }
    on seen != 0 => return 1

    nums: [..]i32
    nums.push(4)
    nums.push(5)
    nums.push(6)

    total := 0
    for item in nums {
        total += item^
    }

    on total != 15 => return 2
    return 0
}
¬
0
¬

¬

¬

=== case: 079-run-llvm-indexed-for-in ===
Location :: plex {
    description string
    tag         string
}

locs: []Location = [
    { description : "d", tag : "a" },
]

main :: fn () -> i32 {
    noun := "a"
    for i, loc in locs {
        on noun == loc.tag => return i.as(i32)
    }
    return 1
}
¬
0
¬

¬

¬

=== case: 118-run-llvm-effect-for-else ===
main :: fn () -> i32 {
    ran_else := 0
    for i := 0; i < 0; i += 1 {
        ran_else = 1
        break
    } else {
        ran_else = 2
    }
    on ran_else != 2 => return 1

    break_else := 0
    for i := 0; i < 0; i += 1 {
        break_else = 1
        break
    } else {
        break_else = 2
        break
    }
    on break_else != 2 => return 2

    skipped_else := 0
    for i := 0; i < 2; i += 1 {
        skipped_else = 3
        break
    } else {
        skipped_else = 4
    }
    on skipped_else != 3 => return 3

    for_in_else := 0
    for item in [1, 2] {
        on item^ == 3 => break
    } else {
        for_in_else = 5
    }
    on for_in_else != 5 => return 4

    for_in_skipped_else := 0
    for item in [1, 2] {
        on item^ == 1 => {
            for_in_skipped_else = 6
            break
        }
    } else {
        for_in_skipped_else = 7
    }
    on for_in_skipped_else != 6 => return 5

    return 0
}
¬
0
¬

¬

¬

=== case: 154-run-range-infers-untyped-bound-from-u32 ===
main :: fn () -> i32 {
    count: u32 = 4
    total := 0

    for value in [0 .. count] {
        total += value.as(i32)
    }

    on total != 6 => return 1
    return 0
}
¬
0
¬

¬

¬

=== case: 185-run-llvm-for-in-again-advances ===
main :: fn () -> i32 {
    nums : [..]i32
    nums.push(1)
    nums.push(2)
    nums.push(3)
    defer nums.free()

    visits := 0
    total := 0
    for item in nums {
        visits += 1
        on item^ < 10 => again
        total += item^
    }

    on visits != 3 => return 1
    on total != 0 => return 2
    return 0
}
¬
0
¬

¬

¬

=== case: 353-run-loop-temporary-storage ===
-- Each nested range and short-circuit expression has a short lifetime. Repeating
-- them must reuse fixed stack storage, including when no optimiser runs.
main :: fn () -> i32 {
    count: i32 = 0
    for iteration in [0 .. 1_000_000] {
        on iteration >= 0 && iteration < 1_000_000 => {
            for _ in [0 .. 2] {
                count += 1
            }
        }
    }
    assert count == 2_000_000
    return 0
}
¬
0
¬

¬

¬

=== case: 333-indexed-loop-slot-growth ===
-- test-release
-- A large aggregate receiver plus these locals puts the named loop index
-- at a slot-table growth boundary when the item slot is added.
QueueInfo :: plex {
 queueFlags u32
 queueCount u32
 reserved [4]u32
}
impl Display for QueueInfo {

    show :: fn (self: ^Self) -> string {
        flags := [1, 2, 4, 8, 16, 32, 64, 256, 1024]
        names := ["graphics", "compute", "transfer", "sparse", "protected",
                  "decode", "encode", "flow", "data graph"]
        capabilities := ""
        remaining    := self.queueFlags

        for i, flag in flags {
            bit := flag^.as(u32)
            on (remaining & bit) != 0 => {
                separator := on capabilities.count == 0 => "" else ", "

                capabilities = temp_arena.pr($"{capabilities}{separator}{names[i]}")
                remaining    = remaining ~ bit
            }
        }
        on remaining != 0 => {
            separator := on capabilities.count == 0 => "" else ", "

            capabilities = temp_arena.pr($"{capabilities}{separator}unknown flags ({remaining})")
        }
        on capabilities.count == 0 => capabilities = "no capabilities"
        plural := on self.queueCount == 1 => "" else "s"
        return temp_arena.pr($"{self.queueCount} queue{plural}: {capabilities}")
    }

}

main :: fn () {
    family: QueueInfo = {
        queueCount: 1
        queueFlags: 7
        ...
    }
    assert family.show() == "1 queue: graphics, compute, transfer"
    family.queueFlags = 1024
    assert family.show() == "1 queue: data graph"
    prn("Indexed loop slots remain valid")
}

¬
0
¬
Indexed loop slots remain valid

¬

¬


=== case: 282-run-range-membership ===
probe :: fn (calls: ^i32, value: i32) -> i32 {
    calls^ += 1
    return value
}

is_digit :: fn (value: u8) -> bool {
    return on value {
        in ['0' ..= '9'] => yes
        else => no
    }
}

main :: fn () -> i32 {
    calls := 0
    on !(probe(^calls, 5) in [probe(^calls, 0)..probe(^calls, 10)]) => return 1
    on calls != 3 => return 2

    calls = 0
    on probe(^calls, -1) in [probe(^calls, 0)..probe(^calls, 10)] => return 3
    on calls != 2 => return 4

    on !(9 in [0..=9]) => return 5
    on 10 in [0..=9] => return 6
    on !(is_digit('0') && is_digit('9')) => return 7
    on is_digit('/') || is_digit(':') => return 8

    value := 0
    for (value in [0..3]) {
        value += 1
    }
    on value != 3 => return 9

    iterations := 0
    for item in [0..3] {
        iterations += item + 1
    }
    on iterations != 6 => return 10
    return 0
}
¬
0
¬

¬

¬

=== case: 368-truthy-for-conditions ===
Error :: enum { Stop }

next_optional :: fn (calls: ^i32) -> ?bool {
    calls^ += 1
    on calls^ > 3 => return nil
    return no
}
next_result :: fn (calls: ^i32) -> i32\Error {
    calls^ += 1
    on calls^ > 3 => return (Error.Stop)!
    return 0
}
next_optional_void :: fn (calls: ^i32) -> ?void {
    calls^ += 1
    on calls^ > 3 => return nil
}
next_result_void :: fn (calls: ^i32) -> void\Error {
    calls^ += 1
    on calls^ > 3 => return (Error.Stop)!
}

main :: fn () {
    calls := 0
    bodies := 0
    for next_optional(^calls) {
        bodies += 1
        again
    }
    assert calls == 4
    assert bodies == 3

    calls = 0
    bodies = 0
    for next_result(^calls) { bodies += 1 }
    assert calls == 4
    assert bodies == 3

    calls = 0
    bodies = 0
    for next_optional_void(^calls) { bodies += 1 }
    assert calls == 4
    assert bodies == 3

    calls = 0
    bodies = 0
    for next_result_void(^calls) { bodies += 1 }
    assert calls == 4
    assert bodies == 3

    pending: ?bool = no
    for pending {
        pending = nil
        bodies += 1
    }
    assert bodies == 4
    successful: i32\Error = 0
    for successful {
        successful = (Error.Stop)!
        bodies += 1
    }
    assert bodies == 5

    calls = 0
    bodies = 0
    for i := 0; next_result(^calls); i += 1 {
        assert i == bodies
        bodies += 1
        again
    }
    assert calls == 4
    assert bodies == 3

    calls = 0
    bodies = 0
    for i := 0; next_optional(^calls); i += 1 {
        assert i == bodies
        bodies += 1
    }
    assert calls == 4
    assert bodies == 3

    missing: ?i32
    exhausted := for missing { break 10 } else { break 20 }
    assert exhausted == 20
    present: ?i32 = 0
    broken := for present { break 30 } else { break 40 }
    assert broken == 30
    failed: bool\Error = (Error.Stop)!
    error_end := for failed { break 50 } else { break 60 }
    assert error_end == 60
    false_success: bool\Error = no
    success_break := for false_success { break 70 } else { break 80 }
    assert success_break == 70

    calls = 0
    c_style := for i := 0; next_optional(^calls); i += 1 {
        on i == 1 => break 90
    } else { break 100 }
    assert c_style == 90
    assert calls == 2

    full_box := box[i32](1)
    for full_box { break }
    empty_box: box[i32]
    for empty_box { assert no }
    absent_storage: ?arena
    for absent_storage { assert no }
    failed_storage: arena\Error = (Error.Stop)!
    for failed_storage { assert no }
    for no { assert no }
}
¬
0
¬

¬

¬

=== case: 369-nested-labelled-loop-values ===
main :: fn () {
    direct := for $outer {
        for { break $outer 42 }
    }
    assert direct == 42

    deep := for $outer {
        for {
            for { break $outer 43 }
        }
    }
    assert deep == 43

    log := 0
    cleaned := for $outer {
        defer log = log * 10 + 1
        for {
            defer log = log * 10 + 2
            break $outer 44
        }
    }
    assert cleaned == 44
    assert log == 21

    own_values := for $outer {
        inner := for { break "inner" }
        assert inner == "inner"
        for { break $outer 45 }
    }
    assert own_values == 45

    shadowed := for $same {
        inner := for $same { break $same "inner" }
        assert inner == "inner"
        break $same 46
    }
    assert shadowed == 46

    block_shadow := for $same {
        inner := $same { break $same "block" }
        assert inner == "block"
        break $same 47
    }
    assert block_shadow == 47

    through_block := for $outer {
        $inner {
            for { break $outer 48 }
        }
    }
    assert through_block == 48

    scoped := for $outer {
        {
            for { break $outer 49 }
        }
    }
    assert scoped == 49

    finite := for i in [0 .. 3] $outer {
        for {
            on i == 2 => break $outer 50
            break
        }
    } else { break -1 }
    assert finite == 50

    exhausted := for i in [0 .. 0] $outer {
        for { break $outer i }
    } else { break 51 }
    assert exhausted == 51

    c_style := for i := 0; i < 3; i += 1 $outer {
        for {
            on i == 2 => break $outer 52
            break
        }
    } else { break -1 }
    assert c_style == 52

    inner_else := for $outer {
        for no {} else { break $outer 53 }
    }
    assert inner_else == 53

    wide: i64 = for $outer {
        for { break $outer 5_000_000_000 }
    }
    assert wide == 5_000_000_000

    function_boundary := for $same {
        local :: fn () -> string {
            return for $same { break $same "function" }
        }
        assert local() == "function"
        break $same 54
    }
    assert function_boundary == 54

    sibling := for $outer {
        for {
            on yes => break $outer 55
            break
        }
        break $outer 56
    }
    assert sibling == 55
}
¬
0
¬

¬

¬
