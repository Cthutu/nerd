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
import import.0 pr from module.1(std.io).decl.9: fn (string) -> void
import import.1 epr from module.1(std.io).decl.10: fn (string) -> void
import import.2 prn from module.1(std.io).decl.11: fn (string) -> void
import import.3 eprn from module.1(std.io).decl.12: fn (string) -> void
import import.4 input from module.1(std.io).decl.13: fn (string) -> string
bind pr = import.0
bind epr = import.1
bind prn = import.2
bind eprn = import.3
bind input = import.4
bind Colour = type.0
bind main = fn.0
type type.0 = Colour
func fn.0() -> void {
  let colour: Colour = Colour RED
  expr void call bind.2(prn)(string interpolate(<unknown> "Colour = ", string on Colour local.0(colour) {
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

@.str.m0.0 = private unnamed_addr constant [10 x i8] c"Colour = \00"
@.str.m0.1 = private unnamed_addr constant [4 x i8] c"red\00"
@.str.m0.2 = private unnamed_addr constant [6 x i8] c"green\00"
@.str.m0.3 = private unnamed_addr constant [5 x i8] c"blue\00"

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

declare void @$pr({ ptr, i64 })
declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

define void @fn.0() {
  %t0 = insertvalue { i64, i64 } poison, i64 0, 0
  %t1 = insertvalue { i64, i64 } %t0, i64 0, 1
  %t2 = call i64 @string_builder_mark()
  %t3 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 9 })
  call void @string_builder_append_string({ ptr, i64 } %t3)
  %t4 = insertvalue { i64, i64 } poison, i64 0, 0
  %t5 = insertvalue { i64, i64 } %t4, i64 0, 1
  %t6 = extractvalue { i64, i64 } %t1, 0
  %t7 = extractvalue { i64, i64 } %t5, 0
  %t8 = icmp eq i64 %t6, %t7
  br i1 %t8, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t9 = insertvalue { i64, i64 } poison, i64 1, 0
  %t10 = insertvalue { i64, i64 } %t9, i64 0, 1
  %t11 = extractvalue { i64, i64 } %t1, 0
  %t12 = extractvalue { i64, i64 } %t10, 0
  %t13 = icmp eq i64 %t11, %t12
  br i1 %t13, label %on.body.4, label %on.next.5
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  %t14 = insertvalue { i64, i64 } poison, i64 2, 0
  %t15 = insertvalue { i64, i64 } %t14, i64 0, 1
  %t16 = extractvalue { i64, i64 } %t1, 0
  %t17 = extractvalue { i64, i64 } %t15, 0
  %t18 = icmp eq i64 %t16, %t17
  br i1 %t18, label %on.body.7, label %on.next.8
on.body.7:
  br label %on.value.9
on.value.9:
  br label %on.end.0
on.next.8:
  unreachable
on.end.0:
  %t19 = phi { ptr, i64 } [{ ptr @.str.m0.1, i64 3 }, %on.value.3], [{ ptr @.str.m0.2, i64 5 }, %on.value.6], [{ ptr @.str.m0.3, i64 4 }, %on.value.9]
  %t20 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t19)
  call void @string_builder_append_string({ ptr, i64 } %t20)
  %t21 = call { ptr, i64 } @string_builder_finish(i64 %t2)
  call void @$prn({ ptr, i64 } %t21)
  ret void
}

@$main = alias void (), ptr @fn.0
