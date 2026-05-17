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

declare void @$pr({ ptr, i64 })
declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

define internal i32 @fn.0({ i64, i64 } %colour) {
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

define internal { i64, i64 } @fn.1() {
  %t0 = insertvalue { i64, i64 } poison, i64 1, 0
  %t1 = insertvalue { i64, i64 } %t0, i64 0, 1
  %t2 = insertvalue { i64, i64 } poison, i64 0, 0
  %t3 = insertvalue { i64, i64 } %t2, i64 0, 1
  ret { i64, i64 } %t3
}

define internal i32 @fn.2() {
  %t0 = insertvalue { i64, i64 } poison, i64 0, 0
  %t1 = insertvalue { i64, i64 } %t0, i64 0, 1
  %t2 = insertvalue { i64, i64 } poison, i64 1, 0
  %t3 = insertvalue { i64, i64 } %t2, i64 0, 1
  %t4 = insertvalue { i64, i64 } poison, i64 2, 0
  %t5 = insertvalue { i64, i64 } %t4, i64 0, 1
  %t6 = call i64 @string_builder_mark()
  %t7 = alloca { ptr, i64 }
  %t8 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 4 }, ptr %t8
  call void @to_string$string(ptr %t7, ptr %t8)
  call void @string_builder_append_string(ptr %t7)
  %t9 = call i32 @fn.0({ i64, i64 } %t1)
  %t10 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t10, i32 %t9)
  call void @string_builder_append_string(ptr %t10)
  %t11 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t11, i64 %t6)
  %t12 = load { ptr, i64 }, ptr %t11
  call void @$prn({ ptr, i64 } %t12)
  %t13 = call i64 @string_builder_mark()
  %t14 = alloca { ptr, i64 }
  %t15 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 6 }, ptr %t15
  call void @to_string$string(ptr %t14, ptr %t15)
  call void @string_builder_append_string(ptr %t14)
  %t16 = call i32 @fn.0({ i64, i64 } %t3)
  %t17 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t17, i32 %t16)
  call void @string_builder_append_string(ptr %t17)
  %t18 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t18, i64 %t13)
  %t19 = load { ptr, i64 }, ptr %t18
  call void @$prn({ ptr, i64 } %t19)
  %t20 = call i64 @string_builder_mark()
  %t21 = alloca { ptr, i64 }
  %t22 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 5 }, ptr %t22
  call void @to_string$string(ptr %t21, ptr %t22)
  call void @string_builder_append_string(ptr %t21)
  %t23 = call i32 @fn.0({ i64, i64 } %t5)
  %t24 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t24, i32 %t23)
  call void @string_builder_append_string(ptr %t24)
  %t25 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t25, i64 %t20)
  %t26 = load { ptr, i64 }, ptr %t25
  call void @$prn({ ptr, i64 } %t26)
  %t27 = call i64 @string_builder_mark()
  %t28 = alloca { ptr, i64 }
  %t29 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 9 }, ptr %t29
  call void @to_string$string(ptr %t28, ptr %t29)
  call void @string_builder_append_string(ptr %t28)
  %t30 = call { i64, i64 } @fn.1()
  %t31 = call i32 @fn.0({ i64, i64 } %t30)
  %t32 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t32, i32 %t31)
  call void @string_builder_append_string(ptr %t32)
  %t33 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t33, i64 %t27)
  %t34 = load { ptr, i64 }, ptr %t33
  call void @$prn({ ptr, i64 } %t34)
  %t35 = call i64 @string_builder_mark()
  %t36 = alloca { ptr, i64 }
  %t37 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 6 }, ptr %t37
  call void @to_string$string(ptr %t36, ptr %t37)
  call void @string_builder_append_string(ptr %t36)
  %t38 = call i32 @fn.0({ i64, i64 } %t1)
  %t39 = call i32 @fn.0({ i64, i64 } %t3)
  %t40 = add i32 %t38, %t39
  %t41 = call i32 @fn.0({ i64, i64 } %t5)
  %t42 = add i32 %t40, %t41
  %t43 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t43, i32 %t42)
  call void @string_builder_append_string(ptr %t43)
  %t44 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t44, i64 %t35)
  %t45 = load { ptr, i64 }, ptr %t44
  call void @$prn({ ptr, i64 } %t45)
  ret i32 0
}

@$score = internal alias i32 ({ i64, i64 }), ptr @fn.0
@$pick_shadowed = internal alias { i64, i64 } (), ptr @fn.1
@$main = alias i32 (), ptr @fn.2
