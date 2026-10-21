-- functions feature scenarios. Results and stdout are checked in order.
use std.io

-- 014-annotated-function-type
-- Binds main with an explicit function type annotation.
case_0_main: fn () -> i32: fn () => 1

-- 017-function-type-alias
-- Reuses a function type alias for the main binding.
Case_1_MainFn :: fn () -> i32
case_1_main: Case_1_MainFn: fn () => 7

-- 021-function-parameters
-- Calls a function with typed parameters and integer arguments.
case_2_add :: fn (a: i32, b: i32) => a + b
case_2_main :: fn () => case_2_add(20, 22)

-- 022-void-main
-- Allows a block-bodied main with no explicit return value.
case_3_main :: fn () {
    prn("Hello from void main")
}

-- 023-function-values
-- Stores an anonymous function value in a binding and calls it.
case_4_adder := fn (a: i32, b: i32) => a + b
case_4_main :: fn () => case_4_adder(20, 22)

-- 024-nested-functions
-- Declares and calls a nested non-closure function.
case_5_main :: fn () -> i32 {
    add :: fn (a: i32, b: i32) => a + b
    return add(20, 22)
}

-- 025-block-forward-function
-- Defines a block-bodied function with an explicit return type.
case_6_add :: fn (a: i32, b: i32) -> i32 {
    return a + b
}

case_6_main :: fn () => case_6_add(20, 22)

-- 026-local-function-forward-ref
-- Resolves a local function binding through a forward reference.
case_7_main :: fn () -> i32 {
    result := add(20, 22)
    add :: fn (a: i32, b: i32) => a + b
    return result
}

-- 084-nested-local-fn-in-block-body
case_8_lowercase_first :: fn (text: string) -> u8 {
    tolower :: fn (char: u8) -> u8 {
        return on char {
            'A' .. 'Z' => char + 32
            else => char
        }
    }

    chars := text
    for c in chars {
        return tolower(c^)
    }

    return 0
}

case_8_main :: fn () {
    prn($"{case_8_lowercase_first("ABC")}")
}

-- 126-default-parameters
case_9_add :: fn (a: i32, b: i32 = 20, c: i32 = a + b) => a + b + c
case_9_scale :: fn (value: i32) => value * 10
case_9_with_call_default :: fn (value: i32 = case_9_scale(3)) => value + 1

case_9_local_call :: fn (a: i32) {
    inner :: fn (x: i32, y: i32 = x + 3) => x * y
    prn($"inner {inner(a)}")
}

case_9_main :: fn () {
    prn($"one {case_9_add(2)}")
    prn($"two {case_9_add(2, b = 3)}")
    prn($"three {case_9_add(2, b = 3, c = 4)}")
    prn($"call {case_9_with_call_default()}")
    case_9_local_call(4)
}

-- 180-void-function-type-shorthand
-- Function types without an explicit return type default to void.
Case_11_Sink :: fn (value: i32)

case_11_sink :: fn (_value: i32) {
}

case_11_main :: fn () {
    f: Case_11_Sink = nil
    f = case_11_sink
    f(42)
}

-- 191-compound-functions
case_12_imported :: use test.compound

case_12_numeric :: fn {
    case_12_increment
}

case_12_convert :: fn {
    case_12_numeric
    case_12_string_count
}

case_12_with_default :: fn {
    case_12_add
    case_12_prefix_count
}

case_12_increment :: fn (value: i32) -> i32 {
    return value + 1
}

case_12_string_count :: fn (value: string) -> i32 {
    return value.count.as(i32)
}

case_12_add :: fn (lhs: i32, rhs: i32 = 2) -> i32 {
    return lhs + rhs
}

case_12_prefix_count :: fn (prefix: string, value: string = "x") -> i32 {
    return (prefix.count + value.count).as(i32)
}

case_12_main :: fn () -> i32 {
    concrete: fn(arg1: i32) -> i32 = case_12_convert
    _address: ^fn(arg1: i32) -> i32 = ^case_12_convert
    return case_12_convert(4) + case_12_convert("abc") + concrete(5) + case_12_with_default(8) +
           case_12_with_default("ab") + case_12_imported.convert(1) +
           case_12_imported.convert("ab") - 57
}

-- 119-run-llvm-parameter-indirect-mutation
Case_13_State :: plex {
    player_loc i32
}

case_13_move :: fn (state: ^Case_13_State) {
    state.player_loc = 7
}

case_13_write_ptr :: fn (value: ^i32) {
    value^ = 11
}

case_13_write_slice :: fn (values: []i32) {
    values[0] = 13
}

case_13_main :: fn () -> i32 {
    state := Case_13_State { player_loc: 0 }
    case_13_move(^state)

    value := 0
    case_13_write_ptr(^value)

    values: [1]i32 = [0]
    case_13_write_slice(values[..])

    return state.player_loc + value + values[0]
}

-- 244-run-compound-functions
case_14_convert :: fn {
    case_14_convert_i32
    case_14_convert_string
}

case_14_convert_i32 :: fn (value: i32) -> i32 { return value + 1 }
case_14_convert_string :: fn (value: string) -> i32 { return value.count.as(i32) }

case_14_main :: fn () -> i32 {
    selected: fn(arg1: i32) -> i32 = case_14_convert
    return case_14_convert(4) + case_14_convert("abc") + selected(5) - 14
}

-- 260-run-direct-recursive-function
case_15_factorial :: fn (value: u64) -> u64 {
    on value <= 1 => return 1
    return value * case_15_factorial(value - 1)
}

case_15_main :: fn () -> i32 {
    return on case_15_factorial(5) == 120 => 0 else 1
}

-- 302-run-public-main
pub case_16_main :: fn () => 42

main :: fn () -> i32 {
    prn("begin 014-annotated-function-type")
    {
        result := case_0_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 1
    }
    prn("begin 017-function-type-alias")
    {
        result := case_1_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 7
    }
    prn("begin 021-function-parameters")
    {
        result := case_2_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 42
    }
    prn("begin 022-void-main")
    {
        case_3_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 023-function-values")
    {
        result := case_4_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 42
    }
    prn("begin 024-nested-functions")
    {
        result := case_5_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 42
    }
    prn("begin 025-block-forward-function")
    {
        result := case_6_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 42
    }
    prn("begin 026-local-function-forward-ref")
    {
        result := case_7_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 42
    }
    prn("begin 084-nested-local-fn-in-block-body")
    {
        case_8_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 126-default-parameters")
    {
        case_9_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 180-void-function-type-shorthand")
    {
        case_11_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 191-compound-functions")
    {
        result := case_12_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 119-run-llvm-parameter-indirect-mutation")
    {
        result := case_13_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 31
    }
    prn("begin 244-run-compound-functions")
    {
        result := case_14_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 260-run-direct-recursive-function")
    {
        result := case_15_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 302-run-public-main")
    {
        result := case_16_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 42
    }
    return 0
}
¬
0
¬
begin 014-annotated-function-type
result 1
begin 017-function-type-alias
result 7
begin 021-function-parameters
result 42
begin 022-void-main
Hello from void main
result 0
begin 023-function-values
result 42
begin 024-nested-functions
result 42
begin 025-block-forward-function
result 42
begin 026-local-function-forward-ref
result 42
begin 084-nested-local-fn-in-block-body
97
result 0
begin 126-default-parameters
one 44
two 10
three 9
call 31
inner 28
result 0
begin 180-void-function-type-shorthand
result 0
begin 191-compound-functions
result 0
begin 119-run-llvm-parameter-indirect-mutation
result 31
begin 244-run-compound-functions
result 0
begin 260-run-direct-recursive-function
result 0
begin 302-run-public-main
result 42

¬

¬


=== case: 138-named-call-arguments ===
Counter :: plex {
    value i32
}

impl Counter {
    set :: fn (self: ^Self, amount: i32 = 1, scale: i32 = 2) {
        self.value = amount * scale
    }

    add :: fn (self: ^Self, amount: i32 = 1) {
        self.value += amount
    }

    get :: fn (self: Self) -> i32 {
        return self.value
    }
}

add :: fn (a: i32, b: i32 = 10, c: i32 = a + b) => a + b + c

main :: fn () {
    counter: Counter
    counter.set(amount = 7, scale = 3)
    counter.add(amount = 5)
    return add(a = 1, b = 2, c = 3) + counter.get()
}
¬
32
¬

¬
hir 0
bind Counter = type.0
bind __impl_Counter_set = fn.0
bind __impl_Counter_add = fn.1
bind __impl_Counter_get = fn.2
bind add = fn.3
bind main = fn.4
type type.0 = Counter
func fn.0(self: ^Counter, amount: i32, scale: i32) -> void {
  assign i32 field(^Counter local.0(self), value) = i32 multiply(i32 local.1(amount), i32 local.2(scale))
}
func fn.1(self: ^Counter, amount: i32) -> void {
  assign i32 field(^Counter local.3(self), value) = i32 add(i32 field(^Counter local.3(self), value), i32 local.4(amount))
}
func fn.2(self: Counter) -> i32 {
  return i32 field(Counter local.5(self), value)
}
func fn.3(a: i32, b: i32, c: i32) -> i32 {
  return i32 add(i32 add(i32 local.6(a), i32 local.7(b)), i32 local.8(c))
}
func fn.4() -> i32 {
  let counter: Counter = <unknown> default
  expr void call bind.1(__impl_Counter_set)(^Counter address_of(Counter local.9(counter)), i32 7, i32 3)
  expr void call bind.2(__impl_Counter_add)(^Counter address_of(Counter local.9(counter)), i32 5)
  return i32 add(i32 call bind.4(add)(i32 1, i32 2, i32 3), i32 call bind.3(__impl_Counter_get)(Counter local.9(counter)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [42 x i8] c"tests/language/functions.t\00"

define internal void @fn.0(ptr %self, i32 %amount, i32 %scale) {
  %t0 = mul i32 %amount, %scale
  %t1 = getelementptr inbounds { i32 }, ptr %self, i64 0, i32 0
  store i32 %t0, ptr %t1
  ret void
}

define internal void @fn.1(ptr %self, i32 %amount) {
  %t0 = load { i32 }, ptr %self
  %t1 = extractvalue { i32 } %t0, 0
  %t2 = add i32 %t1, %amount
  %t3 = getelementptr inbounds { i32 }, ptr %self, i64 0, i32 0
  store i32 %t2, ptr %t3
  ret void
}

define internal i32 @fn.2({ i32 } %self) {
  %t0 = extractvalue { i32 } %self, 0
  ret i32 %t0
}

define internal i32 @fn.3(i32 %a, i32 %b, i32 %c) {
  %t0 = add i32 %a, %b
  %t1 = add i32 %t0, %c
  ret i32 %t1
}

define internal i32 @fn.4() {
  %local.9 = alloca { i32 }
  store { i32 } zeroinitializer, ptr %local.9
  call void @fn.0(ptr %local.9, i32 7, i32 3)
  call void @fn.1(ptr %local.9, i32 5)
  %t0 = call i32 @fn.3(i32 1, i32 2, i32 3)
  %t1 = load { i32 }, ptr %local.9
  %t2 = call i32 @fn.2({ i32 } %t1)
  %t3 = add i32 %t0, %t2
  ret i32 %t3
}

@$__impl_Counter_set = internal alias void (ptr, i32, i32), ptr @fn.0
@$__impl_Counter_add = internal alias void (ptr, i32), ptr @fn.1
@$__impl_Counter_get = internal alias i32 ({ i32 }), ptr @fn.2
@$add = internal alias i32 (i32, i32, i32), ptr @fn.3
@$main = hidden alias i32 (), ptr @fn.4

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)

=== case: 038-run-llvm-function-values ===
use std.io

adder := fn (a: i32, b: i32) => a + b

main :: fn () -> i32 {
    local_result := add(20, 22)
    add :: fn (a: i32, b: i32) => a + b

    nested :: fn (a: i32, b: i32) => a + b

    prn($"global={adder(20, 22)}")
    prn($"local={local_result}")
    prn($"nested={nested(5, 6)}")
    return adder(20, 22)
}
¬
42
¬
global=42
local=42
nested=11

¬

¬

=== case: 063-run-llvm-default-parameters ===
use std.io

add :: fn (a: i32, b: i32 = 20, c: i32 = a + b) => a + b + c
scale :: fn (value: i32) => value * 10
with_call_default :: fn (value: i32 = scale(3)) => value + 1

local_call :: fn (a: i32) {
    inner :: fn (x: i32, y: i32 = x + 3) => x * y
    prn($"inner {inner(a)}")
}

function_value_call :: fn (a: i32) {
    f := add
    prn($"value {f(a)}")
}

main :: fn () {
    prn($"one {add(2)}")
    prn($"two {add(2, b = 3)}")
    prn($"three {add(2, b = 3, c = 4)}")
    prn($"call {with_call_default()}")
    local_call(4)
    function_value_call(2)
}
¬
0
¬
one 44
two 10
three 9
call 31
inner 28
value 44

¬

¬

=== case: 213-run-llvm-function-field-call ===
MeasureFn :: fn (arg1: i32) -> i32

double :: fn (value: i32) -> i32 {
    return value * 2
}

Box :: plex {
    measure MeasureFn
}

main :: fn () -> i32 {
    box := Box { measure: double }
    return box.measure(21)
}
¬
42
¬

¬

¬

=== case: 304-run-named-function-table ===
Functions :: plex #c {
    log fn (channel: ^u8, format: ^u8)
    apply fn (callback: fn (value: i32) -> i32, value: i32) -> i32
}
log :: fn (_channel: ^u8, _format: ^u8) {}
apply :: fn (operation: fn (input: i32) -> i32, number: i32) -> i32 {
    return operation(number)
}
double :: fn (n: i32) => n * 2
main :: fn () {
    funcs := Functions { log: log, apply: apply }
    callback: fn (different_name: i32) -> i32 = double
    funcs.log(nil, nil)
    assert funcs.apply(callback, 21) == 42
}
¬
0
¬

¬

¬

=== case: 320-named-default-parameters ===
use test.named_defaults

add :: fn (a: i32, b: i32 = 2, c: i32 = a + b) => a + b + c
identity :: fn [T] (value: T, enabled: bool = yes) => value
specialised :: fn (value: i32, enabled :: bool = yes) => on enabled => value else 0
Counter :: plex { value i32 }
impl Counter {
    add :: fn (self: Self, amount: i32 = 1) => self.value + amount
}
select_i32 :: fn (value: i32, increment: i32 = 1) => value + increment
select_bool :: fn (value: bool) => on value => 1 else 0
select :: fn {
    select_i32
    select_bool
}
invoke :: fn (callback: fn (a: i32, b: i32, c: i32) -> i32) => callback(1, 2, 3)
main :: fn () -> i32 {
    on add(3) != 10 => return 1
    on add(3, b = 4) != 14 => return 2
    on add(3, b = 4, c = 8) != 15 => return 3
    alias := add
    on alias(3, b = 4) != 14 => return 4
    on identity(7, enabled = no) != 7 => return 5
    on specialised(8, enabled = no) != 0 => return 6
    counter := Counter { value: 10 }
    on counter.add(amount = 3) != 13 => return 7
    on select(10, increment = 4) != 14 => return 8
    on imported(10, increment = 5) != 15 => return 9
    on imported_generic(6, enabled = no) != 6 => return 10
    on invoke(add) != 6 => return 11
    return 0
}
¬
0
¬

¬

¬

=== case: explicit-void-fallthrough ===
empty :: fn () -> void {}
show :: fn (early: bool) -> void {
    defer prn("cleanup")
    on early => return
    prn("body")
}
main :: fn () -> void {
    empty()
    callback: fn () -> void = empty
    callback()
    nested :: fn () -> void {}
    nested()
    show(yes)
    show(no)
    prn("explicit void main")
}
¬
0
¬
cleanup
body
cleanup
explicit void main
¬

¬
