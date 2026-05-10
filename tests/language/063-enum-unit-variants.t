use std.io

Colour :: enum { Red Green Blue }

score :: fn (colour: Colour) -> i32 {
    return on colour {
        Red => 10
        Green => 20
        Blue => 30
    }
}

pick_shadowed :: fn () -> Colour {
    Red: Colour = Green
    picked: Colour = Red
    return picked
}

main :: fn () -> i32 {
    red: Colour = Red
    green: Colour = Green
    blue := Colour.Blue

    prn($"red {score(red)}")
    prn($"green {score(green)}")
    prn($"blue {score(blue)}")
    prn($"shadowed {score(pick_shadowed())}")
    prn($"total {score(red) + score(green) + score(blue)}")

    return 0
}
¬
0
¬
red 10
green 20
blue 30
shadowed 10
total 60

¬
hir 0
module module.0(063-enum-unit-variants.input)
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
bind score = fn.0
bind pick_shadowed = fn.1
bind main = fn.2
type type.0 = Colour
func fn.0(colour: Colour) -> i32 {
  return i32 on Colour local.0(colour) {
    value(Colour Red) => {
      expr i32 10
    }
    value(Colour Green) => {
      expr i32 20
    }
    value(Colour Blue) => {
      expr i32 30
    }
  }
}
func fn.1() -> Colour {
  let Red: Colour = Colour Green
  let picked: Colour = Colour Red
  return Colour local.2(picked)
}
func fn.2() -> i32 {
  let red: Colour = Colour Red
  let green: Colour = Colour Green
  let blue: Colour = Colour field(Colour bind.5(Colour), Blue)
  expr void call bind.2(prn)(string interpolate(<unknown> "red ", i32 call bind.6(score)(Colour local.3(red))))
  expr void call bind.2(prn)(string interpolate(<unknown> "green ", i32 call bind.6(score)(Colour local.4(green))))
  expr void call bind.2(prn)(string interpolate(<unknown> "blue ", i32 call bind.6(score)(Colour local.5(blue))))
  expr void call bind.2(prn)(string interpolate(<unknown> "shadowed ", i32 call bind.6(score)(Colour call bind.7(pick_shadowed)())))
  expr void call bind.2(prn)(string interpolate(<unknown> "total ", i32 add(i32 add(i32 call bind.6(score)(Colour local.3(red)), i32 call bind.6(score)(Colour local.4(green))), i32 call bind.6(score)(Colour local.5(blue)))))
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [5 x i8] c"red \00"
@.str.m0.1 = private unnamed_addr constant [7 x i8] c"green \00"
@.str.m0.2 = private unnamed_addr constant [6 x i8] c"blue \00"
@.str.m0.3 = private unnamed_addr constant [10 x i8] c"shadowed \00"
@.str.m0.4 = private unnamed_addr constant [7 x i8] c"total \00"

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

define i32 @fn.0({ i64, i64 } %colour) {
  %t0 = insertvalue { i64, i64 } poison, i64 0, 0
  %t1 = insertvalue { i64, i64 } %t0, i64 0, 1
  %t2 = extractvalue { i64, i64 } %colour, 0
  %t3 = extractvalue { i64, i64 } %t1, 0
  %t4 = icmp eq i64 %t2, %t3
  br i1 %t4, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t5 = insertvalue { i64, i64 } poison, i64 1, 0
  %t6 = insertvalue { i64, i64 } %t5, i64 0, 1
  %t7 = extractvalue { i64, i64 } %colour, 0
  %t8 = extractvalue { i64, i64 } %t6, 0
  %t9 = icmp eq i64 %t7, %t8
  br i1 %t9, label %on.body.4, label %on.next.5
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  %t10 = insertvalue { i64, i64 } poison, i64 2, 0
  %t11 = insertvalue { i64, i64 } %t10, i64 0, 1
  %t12 = extractvalue { i64, i64 } %colour, 0
  %t13 = extractvalue { i64, i64 } %t11, 0
  %t14 = icmp eq i64 %t12, %t13
  br i1 %t14, label %on.body.7, label %on.next.8
on.body.7:
  br label %on.value.9
on.value.9:
  br label %on.end.0
on.next.8:
  unreachable
on.end.0:
  %t15 = phi i32 [10, %on.value.3], [20, %on.value.6], [30, %on.value.9]
  ret i32 %t15
}

define { i64, i64 } @fn.1() {
  %t0 = insertvalue { i64, i64 } poison, i64 1, 0
  %t1 = insertvalue { i64, i64 } %t0, i64 0, 1
  %t2 = insertvalue { i64, i64 } poison, i64 0, 0
  %t3 = insertvalue { i64, i64 } %t2, i64 0, 1
  ret { i64, i64 } %t3
}

define i32 @fn.2() {
  %t0 = insertvalue { i64, i64 } poison, i64 0, 0
  %t1 = insertvalue { i64, i64 } %t0, i64 0, 1
  %t2 = insertvalue { i64, i64 } poison, i64 1, 0
  %t3 = insertvalue { i64, i64 } %t2, i64 0, 1
  %t4 = insertvalue { i64, i64 } poison, i64 2, 0
  %t5 = insertvalue { i64, i64 } %t4, i64 0, 1
  %t6 = call i64 @string_builder_mark()
  %t7 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 4 })
  call void @string_builder_append_string({ ptr, i64 } %t7)
  %t8 = call i32 @fn.0({ i64, i64 } %t1)
  %t9 = call { ptr, i64 } @to_string$i32(i32 %t8)
  call void @string_builder_append_string({ ptr, i64 } %t9)
  %t10 = call { ptr, i64 } @string_builder_finish(i64 %t6)
  call void @$prn({ ptr, i64 } %t10)
  %t11 = call i64 @string_builder_mark()
  %t12 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 6 })
  call void @string_builder_append_string({ ptr, i64 } %t12)
  %t13 = call i32 @fn.0({ i64, i64 } %t3)
  %t14 = call { ptr, i64 } @to_string$i32(i32 %t13)
  call void @string_builder_append_string({ ptr, i64 } %t14)
  %t15 = call { ptr, i64 } @string_builder_finish(i64 %t11)
  call void @$prn({ ptr, i64 } %t15)
  %t16 = call i64 @string_builder_mark()
  %t17 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 5 })
  call void @string_builder_append_string({ ptr, i64 } %t17)
  %t18 = call i32 @fn.0({ i64, i64 } %t5)
  %t19 = call { ptr, i64 } @to_string$i32(i32 %t18)
  call void @string_builder_append_string({ ptr, i64 } %t19)
  %t20 = call { ptr, i64 } @string_builder_finish(i64 %t16)
  call void @$prn({ ptr, i64 } %t20)
  %t21 = call i64 @string_builder_mark()
  %t22 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.3, i64 9 })
  call void @string_builder_append_string({ ptr, i64 } %t22)
  %t23 = call { i64, i64 } @fn.1()
  %t24 = call i32 @fn.0({ i64, i64 } %t23)
  %t25 = call { ptr, i64 } @to_string$i32(i32 %t24)
  call void @string_builder_append_string({ ptr, i64 } %t25)
  %t26 = call { ptr, i64 } @string_builder_finish(i64 %t21)
  call void @$prn({ ptr, i64 } %t26)
  %t27 = call i64 @string_builder_mark()
  %t28 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.4, i64 6 })
  call void @string_builder_append_string({ ptr, i64 } %t28)
  %t29 = call i32 @fn.0({ i64, i64 } %t1)
  %t30 = call i32 @fn.0({ i64, i64 } %t3)
  %t31 = add i32 %t29, %t30
  %t32 = call i32 @fn.0({ i64, i64 } %t5)
  %t33 = add i32 %t31, %t32
  %t34 = call { ptr, i64 } @to_string$i32(i32 %t33)
  call void @string_builder_append_string({ ptr, i64 } %t34)
  %t35 = call { ptr, i64 } @string_builder_finish(i64 %t27)
  call void @$prn({ ptr, i64 } %t35)
  ret i32 0
}

@$score = alias i32 ({ i64, i64 }), ptr @fn.0
@$pick_shadowed = alias { i64, i64 } (), ptr @fn.1
@$main = alias i32 (), ptr @fn.2
