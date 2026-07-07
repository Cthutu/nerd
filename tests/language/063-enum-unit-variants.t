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
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
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
  let blue: Colour = Colour field(Colour bind.2(Colour), Blue)
  expr void call bind.0(prn)(string interpolate(<unknown> "red ", i32 call bind.3(score)(Colour local.3(red))))
  expr void call bind.0(prn)(string interpolate(<unknown> "green ", i32 call bind.3(score)(Colour local.4(green))))
  expr void call bind.0(prn)(string interpolate(<unknown> "blue ", i32 call bind.3(score)(Colour local.5(blue))))
  expr void call bind.0(prn)(string interpolate(<unknown> "shadowed ", i32 call bind.3(score)(Colour call bind.4(pick_shadowed)())))
  expr void call bind.0(prn)(string interpolate(<unknown> "total ", i32 add(i32 add(i32 call bind.3(score)(Colour local.3(red)), i32 call bind.3(score)(Colour local.4(green))), i32 call bind.3(score)(Colour local.5(blue)))))
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [40 x i8] c"tests/language/063-enum-unit-variants.t\00"
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

declare void @$prn({ ptr, i64 })
declare ptr @$input({ ptr, i64 })

define internal i32 @fn.0({ i64, i8 } %colour) {
  %t0 = insertvalue { i64, i8 } poison, i64 0, 0
  %t1 = insertvalue { i64, i8 } %t0, i8 0, 1
  %t2 = extractvalue { i64, i8 } %colour, 0
  %t3 = extractvalue { i64, i8 } %t1, 0
  %t4 = icmp eq i64 %t2, %t3
  br i1 %t4, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t5 = insertvalue { i64, i8 } poison, i64 1, 0
  %t6 = insertvalue { i64, i8 } %t5, i8 0, 1
  %t7 = extractvalue { i64, i8 } %colour, 0
  %t8 = extractvalue { i64, i8 } %t6, 0
  %t9 = icmp eq i64 %t7, %t8
  br i1 %t9, label %on.body.4, label %on.next.5
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  %t10 = insertvalue { i64, i8 } poison, i64 2, 0
  %t11 = insertvalue { i64, i8 } %t10, i8 0, 1
  %t12 = extractvalue { i64, i8 } %colour, 0
  %t13 = extractvalue { i64, i8 } %t11, 0
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

define internal { i64, i8 } @fn.1() {
  %local.1 = alloca { i64, i8 }
  %local.2 = alloca { i64, i8 }
  %t0 = insertvalue { i64, i8 } poison, i64 1, 0
  %t1 = insertvalue { i64, i8 } %t0, i8 0, 1
  store { i64, i8 } %t1, ptr %local.1
  %t2 = insertvalue { i64, i8 } poison, i64 0, 0
  %t3 = insertvalue { i64, i8 } %t2, i8 0, 1
  store { i64, i8 } %t3, ptr %local.2
  %t4 = load { i64, i8 }, ptr %local.2
  ret { i64, i8 } %t4
}

define internal i32 @fn.2() {
  %local.3 = alloca { i64, i8 }
  %local.4 = alloca { i64, i8 }
  %local.5 = alloca { i64, i8 }
  %t8 = alloca { ptr, i64 }
  %t16 = alloca { ptr, i64 }
  %t24 = alloca { ptr, i64 }
  %t32 = alloca { ptr, i64 }
  %t40 = alloca { ptr, i64 }
  %t0 = insertvalue { i64, i8 } poison, i64 0, 0
  %t1 = insertvalue { i64, i8 } %t0, i8 0, 1
  store { i64, i8 } %t1, ptr %local.3
  %t2 = insertvalue { i64, i8 } poison, i64 1, 0
  %t3 = insertvalue { i64, i8 } %t2, i8 0, 1
  store { i64, i8 } %t3, ptr %local.4
  %t4 = insertvalue { i64, i8 } poison, i64 2, 0
  %t5 = insertvalue { i64, i8 } %t4, i8 0, 1
  store { i64, i8 } %t5, ptr %local.5
  %t6 = call i64 @string_builder_mark()
  %t7 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 4 }, ptr %t8
  call void @to_string$string(ptr %t7, ptr %t8)
  call void @string_builder_append_string(ptr %t7)
  %t9 = load { i64, i8 }, ptr %local.3
  %t10 = call i32 @fn.0({ i64, i8 } %t9)
  %t11 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t11, i32 %t10)
  call void @string_builder_append_string(ptr %t11)
  %t12 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t12, i64 %t6)
  %t13 = load { ptr, i64 }, ptr %t12
  call void @$prn({ ptr, i64 } %t13)
  %t14 = call i64 @string_builder_mark()
  %t15 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 6 }, ptr %t16
  call void @to_string$string(ptr %t15, ptr %t16)
  call void @string_builder_append_string(ptr %t15)
  %t17 = load { i64, i8 }, ptr %local.4
  %t18 = call i32 @fn.0({ i64, i8 } %t17)
  %t19 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t19, i32 %t18)
  call void @string_builder_append_string(ptr %t19)
  %t20 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t20, i64 %t14)
  %t21 = load { ptr, i64 }, ptr %t20
  call void @$prn({ ptr, i64 } %t21)
  %t22 = call i64 @string_builder_mark()
  %t23 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 5 }, ptr %t24
  call void @to_string$string(ptr %t23, ptr %t24)
  call void @string_builder_append_string(ptr %t23)
  %t25 = load { i64, i8 }, ptr %local.5
  %t26 = call i32 @fn.0({ i64, i8 } %t25)
  %t27 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t27, i32 %t26)
  call void @string_builder_append_string(ptr %t27)
  %t28 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t28, i64 %t22)
  %t29 = load { ptr, i64 }, ptr %t28
  call void @$prn({ ptr, i64 } %t29)
  %t30 = call i64 @string_builder_mark()
  %t31 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 9 }, ptr %t32
  call void @to_string$string(ptr %t31, ptr %t32)
  call void @string_builder_append_string(ptr %t31)
  %t33 = call { i64, i8 } @fn.1()
  %t34 = call i32 @fn.0({ i64, i8 } %t33)
  %t35 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t35, i32 %t34)
  call void @string_builder_append_string(ptr %t35)
  %t36 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t36, i64 %t30)
  %t37 = load { ptr, i64 }, ptr %t36
  call void @$prn({ ptr, i64 } %t37)
  %t38 = call i64 @string_builder_mark()
  %t39 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 6 }, ptr %t40
  call void @to_string$string(ptr %t39, ptr %t40)
  call void @string_builder_append_string(ptr %t39)
  %t41 = load { i64, i8 }, ptr %local.3
  %t42 = call i32 @fn.0({ i64, i8 } %t41)
  %t43 = load { i64, i8 }, ptr %local.4
  %t44 = call i32 @fn.0({ i64, i8 } %t43)
  %t45 = add i32 %t42, %t44
  %t46 = load { i64, i8 }, ptr %local.5
  %t47 = call i32 @fn.0({ i64, i8 } %t46)
  %t48 = add i32 %t45, %t47
  %t49 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t49, i32 %t48)
  call void @string_builder_append_string(ptr %t49)
  %t50 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t50, i64 %t38)
  %t51 = load { ptr, i64 }, ptr %t50
  call void @$prn({ ptr, i64 } %t51)
  ret i32 0
}

@$score = internal alias i32 ({ i64, i8 }), ptr @fn.0
@$pick_shadowed = internal alias { i64, i8 } (), ptr @fn.1
@$main = alias i32 (), ptr @fn.2

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)
