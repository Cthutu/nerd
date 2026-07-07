use std.io

Colour :: enum { RED GREEN BLUE }

main :: fn () {
    colour : Colour = RED

    prn($"Colour = {on colour { RED => "red" GREEN => "green" BLUE => "blue" }}")
}
¬
0
¬
Colour = red

¬
hir 0
module module.0(064-on-in-interpolated-string.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind Colour = type.0
bind main = fn.0
type type.0 = Colour
func fn.0() -> void {
  let colour: Colour = Colour RED
  expr void call bind.0(prn)(string interpolate(<unknown> "Colour = ", string on Colour local.0(colour) {
    value(Colour RED) => {
      expr string "red"
    }
    value(Colour GREEN) => {
      expr string "green"
    }
    value(Colour BLUE) => {
      expr string "blue"
    }
  }))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [47 x i8] c"tests/language/064-on-in-interpolated-string.t\00"
@.str.m0.0 = private unnamed_addr constant [10 x i8] c"Colour = \00"
@.str.m0.1 = private unnamed_addr constant [4 x i8] c"red\00"
@.str.m0.2 = private unnamed_addr constant [6 x i8] c"green\00"
@.str.m0.3 = private unnamed_addr constant [5 x i8] c"blue\00"

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

define internal void @fn.0() {
  %local.0 = alloca { i64, i8 }
  %t4 = alloca { ptr, i64 }
  %t23 = alloca { ptr, i64 }
  %t0 = insertvalue { i64, i8 } poison, i64 0, 0
  %t1 = insertvalue { i64, i8 } %t0, i8 0, 1
  store { i64, i8 } %t1, ptr %local.0
  %t2 = call i64 @string_builder_mark()
  %t3 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 9 }, ptr %t4
  call void @to_string$string(ptr %t3, ptr %t4)
  call void @string_builder_append_string(ptr %t3)
  %t5 = load { i64, i8 }, ptr %local.0
  %t6 = insertvalue { i64, i8 } poison, i64 0, 0
  %t7 = insertvalue { i64, i8 } %t6, i8 0, 1
  %t8 = extractvalue { i64, i8 } %t5, 0
  %t9 = extractvalue { i64, i8 } %t7, 0
  %t10 = icmp eq i64 %t8, %t9
  br i1 %t10, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t11 = insertvalue { i64, i8 } poison, i64 1, 0
  %t12 = insertvalue { i64, i8 } %t11, i8 0, 1
  %t13 = extractvalue { i64, i8 } %t5, 0
  %t14 = extractvalue { i64, i8 } %t12, 0
  %t15 = icmp eq i64 %t13, %t14
  br i1 %t15, label %on.body.4, label %on.next.5
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  %t16 = insertvalue { i64, i8 } poison, i64 2, 0
  %t17 = insertvalue { i64, i8 } %t16, i8 0, 1
  %t18 = extractvalue { i64, i8 } %t5, 0
  %t19 = extractvalue { i64, i8 } %t17, 0
  %t20 = icmp eq i64 %t18, %t19
  br i1 %t20, label %on.body.7, label %on.next.8
on.body.7:
  br label %on.value.9
on.value.9:
  br label %on.end.0
on.next.8:
  unreachable
on.end.0:
  %t21 = phi { ptr, i64 } [{ ptr @.str.m0.1, i64 3 }, %on.value.3], [{ ptr @.str.m0.2, i64 5 }, %on.value.6], [{ ptr @.str.m0.3, i64 4 }, %on.value.9]
  %t22 = alloca { ptr, i64 }
  store { ptr, i64 } %t21, ptr %t23
  call void @to_string$string(ptr %t22, ptr %t23)
  call void @string_builder_append_string(ptr %t22)
  %t24 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t24, i64 %t2)
  %t25 = load { ptr, i64 }, ptr %t24
  call void @$prn({ ptr, i64 } %t25)
  ret void
}

@$main = alias void (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)
