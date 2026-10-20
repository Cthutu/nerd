=== case: 087-grouped-use ===
use test { grouped { alpha beta } }

main :: fn() -> i32 {
    return alpha() + beta()
}
¬
42
¬

¬
hir 0
module module.0(modules.input)
import module.1(test.grouped.alpha)
import module.2(test.grouped.beta)
import import.0 alpha from module.1(test.grouped.alpha).decl.0: fn () -> i32
import import.1 beta from module.2(test.grouped.beta).decl.0: fn () -> i32
bind alpha = import.0
bind beta = import.1
bind main = fn.0
func fn.0() -> i32 {
  return i32 add(i32 call bind.0(alpha)(), i32 call bind.1(beta)())
}
¬
; nerd llvm-ir 0
; generated from HIR

declare i32 @$alpha()
declare i32 @$beta()

define internal i32 @fn.0() {
  %t0 = call i32 @$alpha()
  %t1 = call i32 @$beta()
  %t2 = add i32 %t0, %t1
  ret i32 %t2
}

@$main = hidden alias i32 (), ptr @fn.0

=== case: 091-imported-plex-field-interpolation ===
boxmod :: use test.imported_plex

consume :: fn (text: string) {
    on text.count == 0 => return
    return
}

main :: fn () {
    box := boxmod.make_box(7)
    consume($"Value: {box.value}")
}
¬
0
¬

¬
hir 0
module module.0(modules.input)
import module.1(test.imported_plex)
import import.0 __impl_Box_bump from module.1(test.imported_plex).decl.1: fn (^Box, usize) -> void
import import.1 make_box from module.1(test.imported_plex).decl.2: fn (usize) -> Box
bind __impl_Box_bump = import.0
bind make_box = import.1
bind boxmod = module.1
bind consume = fn.0
bind main = fn.1
func fn.0(text: string) -> void {
  expr void on bool equal(usize field(string local.0(text), count), usize 0) {
    value(bool yes) => {
      return <none>
    }
  }
  return <none>
}
func fn.1() -> void {
  let box: Box = Box call fn (usize) -> Box field(module bind.2(boxmod), make_box)(usize 7)
  expr void call bind.3(consume)(string interpolate(<unknown> "Value: ", usize field(Box local.1(box), value)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [55 x i8] c"tests/language/modules.t\00"
@.str.m0.0 = private unnamed_addr constant [8 x i8] c"Value: \00"

declare i1 @nrt_string_eq(ptr, ptr)
declare void @nrt_string_builder_reset()
declare i64 @nrt_string_builder_mark()
declare void @nrt_string_builder_append_string(ptr)
declare void @nrt_string_builder_append_byte(i8)
declare void @nrt_string_builder_finish(ptr, i64)
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

declare void @$__impl_Box_bump(ptr, i64)
declare { i64 } @$make_box(i64)

define internal void @fn.0({ ptr, i64 } %text) {
  %t0 = extractvalue { ptr, i64 } %text, 1
  %t1 = icmp eq i64 %t0, 0
  %t2 = icmp eq i1 %t1, 1
  br i1 %t2, label %on.body.1, label %on.end.0
on.body.1:
  ret void
on.end.0:
  ret void
}

define internal void @fn.1() {
  %t3 = alloca { ptr, i64 }
  %t0 = call { i64 } @$make_box(i64 7)
  %t1 = call i64 @nrt_string_builder_mark()
  %t2 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 7 }, ptr %t3
  call void @nrt_to_string_string(ptr %t2, ptr %t3)
  call void @nrt_string_builder_append_string(ptr %t2)
  %t4 = extractvalue { i64 } %t0, 0
  %t5 = alloca { ptr, i64 }
  call void @nrt_to_string_usize(ptr %t5, i64 %t4)
  call void @nrt_string_builder_append_string(ptr %t5)
  %t6 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t6, i64 %t1)
  %t7 = load { ptr, i64 }, ptr %t6
  call void @fn.0({ ptr, i64 } %t7)
  ret void
}

@$consume = internal alias void ({ ptr, i64 }), ptr @fn.0
@$main = hidden alias void (), ptr @fn.1

=== case: 145-public-mutable-globals ===
pub counter := 3

main :: fn () -> i32 {
    counter += 4
    return counter
}
¬
7
¬

¬
hir 0
module module.0(modules.input)
bind counter = value.0
bind main = fn.0
global value.0: i32 = untyped integer 3
func fn.0() -> i32 {
  assign i32 bind.0(counter) = i32 add(i32 bind.0(counter), i32 4)
  return i32 bind.0(counter)
}
export bind.0(counter)
¬
; nerd llvm-ir 0
; generated from HIR

@$counter = global i32 0

define void @m0.init() {
  store i32 3, ptr @$counter
  ret void
}

define internal i32 @fn.0() {
  %t0 = load i32, ptr @$counter
  %t1 = add i32 %t0, 4
  store i32 %t1, ptr @$counter
  %t2 = load i32, ptr @$counter
  ret i32 %t2
}

@$main = hidden alias i32 (), ptr @fn.0

=== case: 181-bare-use-imported-types ===
use test.lsp_types

Window :: Point

Panel :: plex {
    window Window
}

main :: fn () -> i32 {
    return 0
}
¬
0
¬

¬
hir 0
module module.0(modules.input)
import module.1(test.lsp_types)
import import.0 Point from module.1(test.lsp_types).decl.0: Window
bind Point = import.0
bind Window = type.0
bind Panel = type.1
bind main = fn.0
type type.0 = Window
type type.1 = Panel
func fn.0() -> i32 {
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  ret i32 0
}

@$main = hidden alias i32 (), ptr @fn.0

=== case: 240-imported-optional-nil-defaults ===
use std.io
use test.optional_defaults
local :: fn (value: ?u64 = nil) -> u64 {
    return on value { present => present else => 42 }
}
main :: fn () -> i32 {
    reader : Reader
    assert local() == 42
    assert local(value = 0) == 0
    assert stamp() == 99
    assert stamp(value = nil) == 99
    assert stamp(value = 0) == 0
    assert stamp(value = 7) == 7
    assert decimal() == 1.5
    assert decimal(value = 2.5) == 2.5
    assert reader.read() == 99
    assert reader.read(value = 8) == 8
    prn("Imported optional nil defaults passed")
    return 0
}
¬
0
¬
Imported optional nil defaults passed
¬
hir 0
¬
; nerd llvm-ir 0

=== case: 102-run-imported-const-plex-field ===
use test.imported_const_plex

main :: fn () -> i32 {
    flags :: Flags {
        value: A | B
        ...
    }
    return flags.value.as(i32)
}
¬
3
¬

¬

¬

=== case: 068-std-print-module ===
memory :: use std.memory

main :: fn() {
    local_memory :: use std.memory

    _ := local_memory.kb(1)
    _ := memory.mb(1)
    prn("Local")
    prn("Hello")
    pr("same")
    prn(" line")
}
¬
0
¬
Local
Hello
same line

¬
hir 0
bind memory = module.1
bind main = fn.0
func fn.0() -> void {
¬
; nerd llvm-ir 0
define internal void @fn.0() {
  ret void
}

@$main = hidden alias void (), ptr @fn.0

=== case: 071-use-modules ===
use std.io

main :: fn() {
    prn("top")

    p :: use std.io
    use p

    prn("local")
    pr("same")
    prn(" line")
}
¬
0
¬
top
local
same line

¬
hir 0
module module.modules.input(modules.input)
import module.std.io(std.io)
import import.0 pr from module.core(core).decl.N: fn (string) -> void
import import.1 prn from module.core(core).decl.N: <unknown>
import import.2 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.3 prn_text from module.core(core).decl.N: fn (string) -> void
import import.4 prn_empty from module.core(core).decl.N: fn () -> void
bind pr = import.0
bind prn = import.1
bind input = import.2
bind prn_text = import.3
bind prn_empty = import.4
bind main = fn.0
func fn.0() -> void {
  expr void call bind.3(prn_text)(string "top")
  expr module <unsupported>
  let p: module = module <unsupported>
  expr void <unsupported>
  expr void call bind.3(prn_text)(string "local")
  expr void call bind.0(pr)(string "same")
  expr void call bind.3(prn_text)(string " line")
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [33 x i8] c"tests/language/modules.t\00"
@.str.m0.0 = private unnamed_addr constant [4 x i8] c"top\00"
@.str.m0.1 = private unnamed_addr constant [6 x i8] c"local\00"
@.str.m0.2 = private unnamed_addr constant [5 x i8] c"same\00"
@.str.m0.3 = private unnamed_addr constant [6 x i8] c" line\00"

declare void @$pr({ ptr, i64 })
declare ptr @$input({ ptr, i64 })
declare void @$prn_text({ ptr, i64 })
declare void @$prn_empty()

define internal void @fn.0() {
  call void @$prn_text({ ptr, i64 } { ptr @.str.m0.0, i64 3 })
  call void @$prn_text({ ptr, i64 } { ptr @.str.m0.1, i64 5 })
  call void @$pr({ ptr, i64 } { ptr @.str.m0.2, i64 4 })
  call void @$prn_text({ ptr, i64 } { ptr @.str.m0.3, i64 5 })
  ret void
}

@$main = hidden alias void (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)

=== case: 131-module-folder-imports ===
use test.folder_mod
priority :: use test.folder_priority

main :: fn () -> i32 {
    return answer() + priority.priority_answer()
}
¬
49
¬

¬
hir 0
module module.0(modules.input)
import module.1(test.folder_mod)
import module.2(test.folder_priority)
import import.0 answer from module.1(test.folder_mod).decl.0: fn () -> i32
import import.1 label from module.1(test.folder_mod).decl.1: string
import import.2 priority_answer from module.2(test.folder_priority).decl.0: fn () -> i32
bind answer = import.0
bind label = import.1
bind priority_answer = import.2
bind priority = module.3
bind main = fn.0
bind label = value.0
const value.0: string
func fn.0() -> i32 {
  return i32 add(i32 call bind.0(answer)(), i32 call fn () -> i32 field(module bind.3(priority), priority_answer)())
}
¬
; nerd llvm-ir 0
; generated from HIR

declare i32 @$answer()
declare i32 @$priority_answer()

define internal i32 @fn.0() {
  %t0 = call i32 @$answer()
  %t1 = call i32 @$priority_answer()
  %t2 = add i32 %t0, %t1
  ret i32 %t2
}

@$main = hidden alias i32 (), ptr @fn.0

=== case: 134-platform-on-use ===
on "linux" {
    use test.platform_ffi
}

on "windows" {
    use test.platform_ffi
}

main :: fn () -> i32 {
    return abs(-7) + PLATFORM_VALUE
}
¬
19
¬

¬
hir 0
module module.0(modules.input)
import module.1(test.platform_ffi)
import import.0 abs from module.1(test.platform_ffi).decl.0: fn (i32) -> i32
import import.1 PLATFORM_VALUE from module.1(test.platform_ffi).decl.1: untyped integer
bind abs = import.0
bind PLATFORM_VALUE = import.1
bind PLATFORM_VALUE = value.0
bind main = fn.0
bind abs = fn.1
const value.0: untyped integer
func fn.0() -> i32 {
  return i32 add(i32 call bind.0(abs)(i32 negate(i32 7)), i32 bind.2(PLATFORM_VALUE))
}
extern func fn.1(i32) -> i32
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %t0 = sub i32 0, 7
  %t1 = call i32 @abs(i32 %t0)
  %t2 = add i32 %t1, 12
  ret i32 %t2
}

declare i32 @abs(i32)

@$main = hidden alias i32 (), ptr @fn.0

=== case: 135-module-parts ===
use test.parts

main :: fn () -> i32 {
    return part_answer()
}
¬
42
¬

¬
hir 0
module module.0(modules.input)
import module.1(test.parts)
import import.0 Thing from module.1(test.parts).decl.0: Thing
import import.1 make_thing from module.1(test.parts).decl.1: fn (i32) -> Thing
import import.2 part_answer from module.1(test.parts).decl.2: fn () -> i32
bind Thing = import.0
bind make_thing = import.1
bind part_answer = import.2
bind main = fn.0
bind Thing = type.0
type type.0 = Thing
func fn.0() -> i32 {
  return i32 call bind.2(part_answer)()
}
¬
; nerd llvm-ir 0
; generated from HIR

declare { i32 } @$make_thing(i32)
declare i32 @$part_answer()

define internal i32 @fn.0() {
  %t0 = call i32 @$part_answer()
  ret i32 %t0
}

@$main = hidden alias i32 (), ptr @fn.0

=== case: 145-module-symbol-namespaces ===
a :: use test.namespaced_a
b :: use test.namespaced_b

main :: fn () -> i32 {
    return a.test() + b.test()
}
¬
42
¬

¬
hir 0
module module.0(modules.input)
import module.1(test.namespaced_a)
import module.2(test.namespaced_b)
import import.0 test from module.1(test.namespaced_a).decl.0: fn () -> i32
import import.1 test from module.2(test.namespaced_b).decl.0: fn () -> i32
bind test = import.0
bind test = import.1
bind a = module.1
bind b = module.3
bind main = fn.0
func fn.0() -> i32 {
  return i32 add(i32 call fn () -> i32 field(module bind.2(a), test)(), i32 call fn () -> i32 field(module bind.3(b), test)())
}
¬
; nerd llvm-ir 0
; generated from HIR

declare i32 @m1.fn.0()
declare i32 @m2.fn.0()

define internal i32 @fn.0() {
  %t0 = call i32 @m1.fn.0()
  %t1 = call i32 @m2.fn.0()
  %t2 = add i32 %t0, %t1
  ret i32 %t2
}

@$main = hidden alias i32 (), ptr @fn.0

=== case: 147-qualified-module-types ===
boxmod :: use test.imported_plex

Holder :: plex {
    box boxmod.Box
    ptr ^boxmod.Box
}

main :: fn () -> i32 {
    box : boxmod.Box
    box = boxmod.make_box(7)
    box.bump(1)
    holder := Holder { box, ptr: ^box }
    return (holder.box.value + holder.ptr^.value).as(i32)
}
¬
16
¬

¬
hir 0
module module.0(modules.input)
import module.1(test.imported_plex)
import import.0 Box from module.1(test.imported_plex).decl.0: Box
import import.1 __impl_Box_bump from module.1(test.imported_plex).decl.1: fn (^Box, usize) -> void
import import.2 make_box from module.1(test.imported_plex).decl.2: fn (usize) -> Box
bind Box = import.0
bind __impl_Box_bump = import.1
bind make_box = import.2
bind boxmod = module.1
bind Holder = type.0
bind main = fn.0
bind Box = type.1
type type.0 = Holder
type type.1 = Box
func fn.0() -> i32 {
  let box: Box = <unknown> default
  assign Box local.0(box) = Box call fn (usize) -> Box field(module bind.3(boxmod), make_box)(usize 7)
  expr void call bind.1(__impl_Box_bump)(^Box address_of(Box local.0(box)), usize 1)
  let holder: Holder = Holder plex(box: Box local.0(box), ptr: ^Box address_of(Box local.0(box)))
  return i32 cast(usize add(usize field(Box field(Holder local.1(holder), box), value), usize field(Box deref(^Box field(Holder local.1(holder), ptr)), value)) as i32)
}
¬
; nerd llvm-ir 0
; generated from HIR

declare void @$__impl_Box_bump(ptr, i64)
declare { i64 } @$make_box(i64)

define internal i32 @fn.0() {
  %local.0 = alloca { i64 }
  store { i64 } zeroinitializer, ptr %local.0
  %t0 = call { i64 } @$make_box(i64 7)
  store { i64 } %t0, ptr %local.0
  call void @$__impl_Box_bump(ptr %local.0, i64 1)
  %t1 = load { i64 }, ptr %local.0
  %t2 = insertvalue { { i64 }, ptr } poison, { i64 } %t1, 0
  %t3 = insertvalue { { i64 }, ptr } %t2, ptr %local.0, 1
  %t4 = extractvalue { { i64 }, ptr } %t3, 0
  %t5 = extractvalue { i64 } %t4, 0
  %t6 = extractvalue { { i64 }, ptr } %t3, 1
  %t7 = load { i64 }, ptr %t6
  %t8 = extractvalue { i64 } %t7, 0
  %t9 = add i64 %t5, %t8
  %t10 = trunc i64 %t9 to i32
  ret i32 %t10
}

@$main = hidden alias i32 (), ptr @fn.0

=== case: 021-run-llvm-module ===
pub use test.folder_pub_use

main :: fn () => child_answer() + local_answer()
¬
42
¬

¬

¬

=== case: 094-run-llvm-imported-default-parameters ===
use std.io

main :: fn () {
    pr("before")
    prn()
    prn("after")
}
¬
0
¬
before
after

¬

¬

=== case: 126-run-imported-constant-codegen ===
use test.imported_const_codegen

sink :: fn (a: u32, b: u32, c: u32) -> u32 {
    return a + b + c
}

main :: fn () {
    x := IMPORTED_A.as(u32)
    y := sink(IMPORTED_A, IMPORTED_B | IMPORTED_C, IMPORTED_A)
    prn($"x={x} y={y}")
    on x != 4 => return 1
    on y != 11 => return 2
    return 0
}
¬
0
¬
x=4 y=11
¬

¬

=== case: 206-run-imported-function-callback ===
use test.imported_callback

main :: fn () -> i32 {
    return call_callback(imported_callback) - 41
}
¬
0
¬

¬

¬

=== case: 295-run-imported-content-equality ===
use equality_values
main :: fn () {
    lhs := [Value {key: 1, cache: 2}]
    rhs := [Value {key: 1, cache: 3}]
    assert lhs[..] == rhs[..]
    a := box[Value]()
    b := box[Value]()
    a.key = 2
    b.key = 2
    a.cache = 5
    b.cache = 6
    assert a == b
    assert a.key == 2 && b.key == 2
}
¬
0
¬

¬

¬

=== case: 339-imported-mutable-globals ===
use mutable_globals
use std.atomics

other :: use mutable_globals_export

separate :: use mutable_globals_other

main :: fn () {
    assert number == 7
    assert other.number == 7
    number = 37
    assert other.number == 37
    number += 2
    pointer := ^number
    pointer^ += 4
    assert other.number == 43
    assert other.values[0] == 11
    values[1] = 47
    assert other.values[1] == 47
    record.second = 53
    assert record.first == 17 && record.second == 53
    callback = replacement
    assert other.callback() == 31
    signal = 59
    assert other.signal.load() == 59
    assert separate.number == 101
    different := separate.number_address()
    assert different != pointer
    different^ = 103
    assert separate.number == 103 && number == 43
    verify()
    prn("imported globals share storage")
}
¬
0
¬
imported globals share storage
¬

¬

=== case: 352-run-imported-recursive-record ===
use test.recursive_message_owner

main :: fn () -> i32 {
    owner := Owner.create()
    owner.set(42)
    assert owner.message.read() == 42
    return owner.get() - 42
}
¬
0
¬

¬

¬

=== case: 112-run-sys-linux-import ===
-- test-platform: linux
use std.io
use os.linux

main :: fn () -> i32 {
    message := "page=yes\n"
    written := sys_write(STDOUT_FILENO, message.data.as(^void), message.count)
    return on written == message.count.as(isize) {
        yes  => 0
        else => 1
    }
}
¬
0
¬
page=yes

¬

¬


=== case: 336-imported-enum-equality ===
-- test-release
use structural_eq
main :: fn () {
 a: Choice = First(1)
 b: Choice = Second
 assert a == b
 assert [a] == [b]
 prn("Imported enum equality passed")
}
¬
0
¬
Imported enum equality passed
¬

¬
