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
import import.0 prn from module.1(std.io).decl.11: fn (string) -> void
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
  return string local.8(value)
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

declare i1 @string_eq({ ptr, i64 }, { ptr, i64 })
declare void @string_builder_reset()
declare i64 @string_builder_mark()
declare void @string_builder_append_string({ ptr, i64 })
declare void @string_builder_append_byte(i8)
declare { ptr, i64 } @string_builder_finish(i64)
declare { ptr, i64 } @to_string$string({ ptr, i64 })
declare { ptr, i64 } @to_string$bool(i1)
declare { ptr, i64 } @to_string$i8(i8)
declare { ptr, i64 } @to_string$i16(i16)
declare { ptr, i64 } @to_string$i32(i32)
declare { ptr, i64 } @to_string$i64(i64)
declare { ptr, i64 } @to_string$u8(i8)
declare { ptr, i64 } @to_string$u16(i16)
declare { ptr, i64 } @to_string$u32(i32)
declare { ptr, i64 } @to_string$u64(i64)
declare { ptr, i64 } @to_string$isize(i64)
declare { ptr, i64 } @to_string$usize(i64)
declare { ptr, i64 } @to_string$f32(float)
declare { ptr, i64 } @to_string$f64(double)

declare void @$prn({ ptr, i64 })

define void @fn.0() {
  ret void
}

define i32 @fn.1(i32 %value) {
  ret i32 0
}

define { ptr, i64 } @fn.2({ ptr, i64 } %value) {
  ret { ptr, i64 } %value
}

define { { ptr, i64 } } @fn.3({ ptr, i64 } %value) {
  %local.10 = alloca { { ptr, i64 } }
  store { { ptr, i64 } } zeroinitializer, ptr %local.10
  %t0 = getelementptr inbounds { { ptr, i64 } }, ptr %local.10, i64 0, i32 0
  store { ptr, i64 } %value, ptr %t0
  %t1 = load { { ptr, i64 } }, ptr %local.10
  ret { { ptr, i64 } } %t1
}

define { ptr, i64 } @fn.4({ ptr, i64 } %left, { ptr, i64 } %right, i1 %use_left) {
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
