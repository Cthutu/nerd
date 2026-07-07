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
    prn($"typed plex patterns {score(escape)} {score(other)} {score(up)}")
    return score(escape)
}
¬
1
¬
typed plex patterns 1 2 0

¬
hir 0
module module.0(151-typed-plex-patterns.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind Event = type.0
bind KeyEvent = type.1
bind Key = type.2
bind score = fn.0
bind main = fn.1
type type.0 = Event
type type.1 = KeyEvent
type type.2 = Key
func fn.0(event: Event) -> i32 {
  return i32 on Event local.0(event) {
    enum_variant(KeyDown, value(Key Escape)) => {
      expr i32 1
    }
    enum_variant(KeyDown, value(Key Other)) => {
      expr i32 2
    }
    else => {
      expr i32 0
    }
  }
}
func fn.1() -> i32 {
  let escape: Event = Event call Event field(Event bind.2(Event), KeyDown)(KeyEvent plex(keycode: Key Escape, modifiers: i32 0, char: u32 0))
  let other: Event = Event call Event field(Event bind.2(Event), KeyDown)(KeyEvent plex(keycode: Key Other, modifiers: i32 1, char: u32 2))
  let up: Event = Event call Event field(Event bind.2(Event), KeyUp)(KeyEvent plex(keycode: Key Escape, modifiers: i32 0, char: u32 0))
  expr void call bind.0(prn)(string interpolate(<unknown> "typed plex patterns ", i32 call bind.5(score)(Event local.1(escape)), <unknown> " ", i32 call bind.5(score)(Event local.2(other)), <unknown> " ", i32 call bind.5(score)(Event local.3(up))))
  return i32 call bind.5(score)(Event local.1(escape))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [41 x i8] c"tests/language/151-typed-plex-patterns.t\00"
@.str.m0.0 = private unnamed_addr constant [21 x i8] c"typed plex patterns \00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.2 = private unnamed_addr constant [2 x i8] c" \00"

declare i1 @string_eq(ptr, ptr)
declare void @string_builder_reset()
declare i64 @string_builder_mark()
declare void @string_builder_append_string(ptr)
declare void @string_builder_append_byte(i8)
declare void @string_builder_finish(ptr, i64)
declare void @to_string$string(ptr, ptr)
declare void @to_string$bool(ptr, i1)
declare void @to_string$i8(ptr, i8)
declare void @to_string$i16(ptr, i16)
declare void @to_string$i32(ptr, i32)
declare void @to_string$i64(ptr, i64)
declare void @to_string$u8(ptr, i8)
declare void @to_string$u16(ptr, i16)
declare void @to_string$u32(ptr, i32)
declare void @to_string$u64(ptr, i64)
declare void @to_string$isize(ptr, i64)
declare void @to_string$usize(ptr, i64)
declare void @to_string$f32(ptr, float)
declare void @to_string$f64(ptr, double)

declare void @$prn({ ptr, i64 })
declare ptr @$input({ ptr, i64 })

define internal i32 @fn.0({ i64, i256 } %event) {
  %t4 = alloca i256
  %t16 = alloca i256
  %t0 = extractvalue { i64, i256 } %event, 0
  %t1 = icmp eq i64 %t0, 0
  %t2 = extractvalue { i64, i256 } %event, 1
  store i256 %t2, ptr %t4
  %t3 = load { { i64, i8 }, i32, i32 }, ptr %t4
  %t5 = extractvalue { { i64, i8 }, i32, i32 } %t3, 0
  %t6 = insertvalue { i64, i8 } poison, i64 41, 0
  %t7 = insertvalue { i64, i8 } %t6, i8 0, 1
  %t8 = extractvalue { i64, i8 } %t5, 0
  %t9 = extractvalue { i64, i8 } %t7, 0
  %t10 = icmp eq i64 %t8, %t9
  %t11 = and i1 %t1, %t10
  br i1 %t11, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t12 = extractvalue { i64, i256 } %event, 0
  %t13 = icmp eq i64 %t12, 0
  %t14 = extractvalue { i64, i256 } %event, 1
  store i256 %t14, ptr %t16
  %t15 = load { { i64, i8 }, i32, i32 }, ptr %t16
  %t17 = extractvalue { { i64, i8 }, i32, i32 } %t15, 0
  %t18 = insertvalue { i64, i8 } poison, i64 42, 0
  %t19 = insertvalue { i64, i8 } %t18, i8 0, 1
  %t20 = extractvalue { i64, i8 } %t17, 0
  %t21 = extractvalue { i64, i8 } %t19, 0
  %t22 = icmp eq i64 %t20, %t21
  %t23 = and i1 %t13, %t22
  br i1 %t23, label %on.body.4, label %on.next.5
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  br label %on.body.7
on.body.7:
  br label %on.value.9
on.value.9:
  br label %on.end.0
on.end.0:
  %t24 = phi i32 [1, %on.value.3], [2, %on.value.6], [0, %on.value.9]
  ret i32 %t24
}

define internal i32 @fn.1() {
  %t6 = alloca i256
  %local.1 = alloca { i64, i256 }
  %t15 = alloca i256
  %local.2 = alloca { i64, i256 }
  %t24 = alloca i256
  %local.3 = alloca { i64, i256 }
  %t29 = alloca { ptr, i64 }
  %t34 = alloca { ptr, i64 }
  %t39 = alloca { ptr, i64 }
  %t0 = insertvalue { i64, i8 } poison, i64 41, 0
  %t1 = insertvalue { i64, i8 } %t0, i8 0, 1
  %t2 = insertvalue { { i64, i8 }, i32, i32 } poison, { i64, i8 } %t1, 0
  %t3 = insertvalue { { i64, i8 }, i32, i32 } %t2, i32 0, 1
  %t4 = insertvalue { { i64, i8 }, i32, i32 } %t3, i32 0, 2
  store i256 0, ptr %t6
  store { { i64, i8 }, i32, i32 } %t4, ptr %t6
  %t5 = load i256, ptr %t6
  %t7 = insertvalue { i64, i256 } poison, i64 0, 0
  %t8 = insertvalue { i64, i256 } %t7, i256 %t5, 1
  store { i64, i256 } %t8, ptr %local.1
  %t9 = insertvalue { i64, i8 } poison, i64 42, 0
  %t10 = insertvalue { i64, i8 } %t9, i8 0, 1
  %t11 = insertvalue { { i64, i8 }, i32, i32 } poison, { i64, i8 } %t10, 0
  %t12 = insertvalue { { i64, i8 }, i32, i32 } %t11, i32 1, 1
  %t13 = insertvalue { { i64, i8 }, i32, i32 } %t12, i32 2, 2
  store i256 0, ptr %t15
  store { { i64, i8 }, i32, i32 } %t13, ptr %t15
  %t14 = load i256, ptr %t15
  %t16 = insertvalue { i64, i256 } poison, i64 0, 0
  %t17 = insertvalue { i64, i256 } %t16, i256 %t14, 1
  store { i64, i256 } %t17, ptr %local.2
  %t18 = insertvalue { i64, i8 } poison, i64 41, 0
  %t19 = insertvalue { i64, i8 } %t18, i8 0, 1
  %t20 = insertvalue { { i64, i8 }, i32, i32 } poison, { i64, i8 } %t19, 0
  %t21 = insertvalue { { i64, i8 }, i32, i32 } %t20, i32 0, 1
  %t22 = insertvalue { { i64, i8 }, i32, i32 } %t21, i32 0, 2
  store i256 0, ptr %t24
  store { { i64, i8 }, i32, i32 } %t22, ptr %t24
  %t23 = load i256, ptr %t24
  %t25 = insertvalue { i64, i256 } poison, i64 1, 0
  %t26 = insertvalue { i64, i256 } %t25, i256 %t23, 1
  store { i64, i256 } %t26, ptr %local.3
  %t27 = call i64 @string_builder_mark()
  %t28 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 20 }, ptr %t29
  call void @to_string$string(ptr %t28, ptr %t29)
  call void @string_builder_append_string(ptr %t28)
  %t30 = load { i64, i256 }, ptr %local.1
  %t31 = call i32 @fn.0({ i64, i256 } %t30)
  %t32 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t32, i32 %t31)
  call void @string_builder_append_string(ptr %t32)
  %t33 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 1 }, ptr %t34
  call void @to_string$string(ptr %t33, ptr %t34)
  call void @string_builder_append_string(ptr %t33)
  %t35 = load { i64, i256 }, ptr %local.2
  %t36 = call i32 @fn.0({ i64, i256 } %t35)
  %t37 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t37, i32 %t36)
  call void @string_builder_append_string(ptr %t37)
  %t38 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 1 }, ptr %t39
  call void @to_string$string(ptr %t38, ptr %t39)
  call void @string_builder_append_string(ptr %t38)
  %t40 = load { i64, i256 }, ptr %local.3
  %t41 = call i32 @fn.0({ i64, i256 } %t40)
  %t42 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t42, i32 %t41)
  call void @string_builder_append_string(ptr %t42)
  %t43 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t43, i64 %t27)
  %t44 = load { ptr, i64 }, ptr %t43
  call void @$prn({ ptr, i64 } %t44)
  %t45 = load { i64, i256 }, ptr %local.1
  %t46 = call i32 @fn.0({ i64, i256 } %t45)
  ret i32 %t46
}

@$score = internal alias i32 ({ i64, i256 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)
