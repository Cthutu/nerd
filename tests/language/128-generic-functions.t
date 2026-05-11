io :: use std.io

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

    io.prn($"{number} {text} {explicit} {from_value} {chosen}")
}
¬
0
¬
7 ok 12 42 ok

¬
hir 0
module module.0(128-generic-functions.input)
import module.1(std.io)
import import.0 prn from module.1(std.io).decl.8: fn (string) -> void
bind prn = import.0
bind io = module.1
bind Box = type.0
bind main = fn.0
generic type type.0 = <unknown>
func fn.0() -> void {
  let number: i32 = i32 call decl.1(id)(untyped integer 7)
  let text: string = string call decl.1(id)(string "ok")
  let explicit: i32 = i32 call fn (i32) -> i32 index(<unknown> decl.1(id), i32 i32)(i32 add(i32 local.0(number), i32 5))
  let int_id: fn (i32) -> i32 = fn (i32) -> i32 index(<unknown> decl.1(id), i32 i32)
  let from_value: i32 = i32 call local.3(int_id)(i32 add(i32 local.2(explicit), i32 30))
  let box: plex { string value } = plex { string value } call fn (string) -> plex { string value } index(<unknown> decl.4(wrap), string string)(string local.1(text))
  let chosen: string = string call fn (string, string, bool) -> string index(<unknown> decl.2(choose), string string)(string field(plex { string value } local.5(box), value), string "bad", bool yes)
  expr void call fn (string) -> void field(module bind.1(io), prn)(string interpolate(i32 local.0(number), <unknown> " ", string local.1(text), <unknown> " ", i32 local.2(explicit), <unknown> " ", i32 local.4(from_value), <unknown> " ", string local.6(chosen)))
}
inst func fn.1(value: i32) -> i32 {
  return i32 local.7(value)
}
inst func fn.2(value: string) -> string {
  return string local.8(value)
}
inst func fn.3(value: string) -> plex { string value } {
  expr <unknown> <unsupported>
  let box: plex { string value } = <unknown> <unsupported>
  assign string field(plex { string value } local.10(box), value) = string local.9(value)
  return plex { string value } local.10(box)
}
inst func fn.4(left: string, right: string, use_left: bool) -> string {
  return string on bool local.13(use_left) {
    value(bool yes) => {
      expr string local.11(left)
    }
    else => {
      expr string local.12(right)
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [3 x i8] c"ok\00"
@.str.m0.1 = private unnamed_addr constant [4 x i8] c"bad\00"
@.str.m0.2 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.3 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.4 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.5 = private unnamed_addr constant [2 x i8] c" \00"

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

define internal void @fn.0() {
  %t0 = call i32 @fn.1(i32 7)
  %t1 = call { ptr, i64 } @fn.2({ ptr, i64 } { ptr @.str.m0.0, i64 2 })
  %t2 = add i32 %t0, 5
  %t3 = call i32 @fn.1(i32 %t2)
  %t4 = add i32 %t3, 30
  %t5 = call i32 @fn.1(i32 %t4)
  %t6 = call { { ptr, i64 } } @fn.3({ ptr, i64 } %t1)
  %t7 = extractvalue { { ptr, i64 } } %t6, 0
  %t8 = call { ptr, i64 } @fn.4({ ptr, i64 } %t7, { ptr, i64 } { ptr @.str.m0.1, i64 3 }, i1 1)
  %t9 = call i64 @string_builder_mark()
  %t10 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t10, i32 %t0)
  call void @string_builder_append_string(ptr %t10)
  %t11 = alloca { ptr, i64 }
  %t12 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 1 }, ptr %t12
  call void @to_string$string(ptr %t11, ptr %t12)
  call void @string_builder_append_string(ptr %t11)
  %t13 = alloca { ptr, i64 }
  %t14 = alloca { ptr, i64 }
  store { ptr, i64 } %t1, ptr %t14
  call void @to_string$string(ptr %t13, ptr %t14)
  call void @string_builder_append_string(ptr %t13)
  %t15 = alloca { ptr, i64 }
  %t16 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 1 }, ptr %t16
  call void @to_string$string(ptr %t15, ptr %t16)
  call void @string_builder_append_string(ptr %t15)
  %t17 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t17, i32 %t3)
  call void @string_builder_append_string(ptr %t17)
  %t18 = alloca { ptr, i64 }
  %t19 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 1 }, ptr %t19
  call void @to_string$string(ptr %t18, ptr %t19)
  call void @string_builder_append_string(ptr %t18)
  %t20 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t20, i32 %t5)
  call void @string_builder_append_string(ptr %t20)
  %t21 = alloca { ptr, i64 }
  %t22 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.5, i64 1 }, ptr %t22
  call void @to_string$string(ptr %t21, ptr %t22)
  call void @string_builder_append_string(ptr %t21)
  %t23 = alloca { ptr, i64 }
  %t24 = alloca { ptr, i64 }
  store { ptr, i64 } %t8, ptr %t24
  call void @to_string$string(ptr %t23, ptr %t24)
  call void @string_builder_append_string(ptr %t23)
  %t25 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t25, i64 %t9)
  %t26 = load { ptr, i64 }, ptr %t25
  call void @$prn({ ptr, i64 } %t26)
  ret void
}

define internal i32 @fn.1(i32 %value) {
  ret i32 %value
}

define internal { ptr, i64 } @fn.2({ ptr, i64 } %value) {
  ret { ptr, i64 } %value
}

define internal { { ptr, i64 } } @fn.3({ ptr, i64 } %value) {
  %local.10 = alloca { { ptr, i64 } }
  store { { ptr, i64 } } zeroinitializer, ptr %local.10
  %t0 = getelementptr inbounds { { ptr, i64 } }, ptr %local.10, i64 0, i32 0
  store { ptr, i64 } %value, ptr %t0
  %t1 = load { { ptr, i64 } }, ptr %local.10
  ret { { ptr, i64 } } %t1
}

define internal { ptr, i64 } @fn.4({ ptr, i64 } %left, { ptr, i64 } %right, i1 %use_left) {
  %t0 = icmp eq i1 %use_left, 1
  br i1 %t0, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  br label %on.body.4
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.end.0:
  %t1 = phi { ptr, i64 } [%left, %on.value.3], [%right, %on.value.6]
  ret { ptr, i64 } %t1
}

@$main = alias void (), ptr @fn.0
