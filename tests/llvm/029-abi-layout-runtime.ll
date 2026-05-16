ffi "c" strlen (^i8) -> usize
ffi "c" snprintf (^void, usize, ^i8, ...) -> i32

Pair :: plex {
    left i32
    right i64
}

Choice :: enum {
    None
    Pair(Pair)
    Text(string)
}

consume :: fn(_value: string) {
}

score :: fn(choice: Choice) -> i32 {
    return on choice {
        None => 0
        Pair(as pair) => pair.left + pair.right.as(i32)
        Text(as text) => text.count.as(i32)
    }
}

use_dyn :: fn() -> i32 {
    values: [..]Pair
    values.push(Pair { left: 3, right: 4 })
    values.reserve_to(4)
    first := values[0]
    values.free()
    return first.left + first.right.as(i32)
}

main :: fn() -> i32 {
    bytes: [4]u8 = ['a', 'b', 'c', 'd']
    ptr := bytes[..].data
    view := ptr.as([]u8, 2)
    ctext := c"hello"
    length := strlen(ctext)
    buffer: [16]u8
    written := snprintf(buffer[..].data, buffer[..].count, c"%d", 7)
    consume($"abi {length} {written}")
    same := "same" == "same"
    choice := Choice.Pair(Pair { left: view[0].as(i32), right: 8 })
    return score(choice) + use_dyn() + same.as(i32)
}

¬
; nerd llvm-ir 0
; generated from HIR

declare i1 @string_eq(ptr, ptr)
declare void @string_builder_reset()
declare i64 @string_builder_mark()
declare void @string_builder_append_string(ptr)
declare void @string_builder_append_byte(i8)
declare void @string_builder_finish(ptr, i64)
declare void @to_string$i32(ptr, i32)
declare void @to_string$usize(ptr, i64)
declare ptr @malloc(i64)
declare ptr @realloc(ptr, i64)
declare void @free(ptr)

declare i64 @strlen(ptr)

declare i32 @snprintf(ptr, i64, ptr, ...)

define internal void @fn.2({ ptr, i64 } %_value) {
define internal i32 @fn.3({ i64, i128 } %choice) {
  %t8 = load { i32, i64 }, ptr %t9
  %t18 = load { ptr, i64 }, ptr %t19
define internal i32 @fn.4() {
  %t2 = call ptr @malloc(i64 24)
  %t3 = getelementptr inbounds { ptr, i64, i64 }, ptr %t2, i64 0, i32 0
  %t20 = mul i64 %t19, 12
  %t21 = call ptr @realloc(ptr %t12, i64 %t20)
  call void @free(ptr %t46)
  call void @free(ptr %t43)
define internal i32 @fn.5() {
  %t9 = insertvalue { ptr, i64 } %t8, i64 2, 1
  %t19 = call i32 @snprintf(ptr %t14, i64 %t18, ptr @.str.m0.3, i32 7)
  call void @to_string$usize(ptr %t23, i64 %t10)
  %t29 = call i1 @string_eq(ptr %t30, ptr %t31)
  %t39 = alloca i128
  %t41 = insertvalue { i64, i128 } %t40, i128 %t38, 1
@$consume = internal alias void ({ ptr, i64 }), ptr @fn.2
@$score = internal alias i32 ({ i64, i128 }), ptr @fn.3
@$use_dyn = internal alias i32 (), ptr @fn.4
@$main = alias i32 (), ptr @fn.5
