use std.io

-- Matches strings with block-form `on` branches.
choice :: "blue"
warm :: "warm"
cool :: "cool"
unknown :: "unknown"

classify :: fn (choice: string) -> i32 {
    return on choice {
        "red", "green" => 1
        "blue" => 2
        else => 3
    }
}

label :: fn (choice: string) -> string {
    return on choice {
        "red" => warm
        "blue" => cool
        else => unknown
    }
}

main :: fn () {
    prn($"red: {classify("red")} {label("red")}")
    prn($"green: {classify("green")} {label("green")}")
    prn($"blue: {classify("blue")} {label("blue")}")
    prn($"other: {classify("other")} {label("other")}")

    return classify(choice)
}
¬
2
¬
red: 1 warm
green: 1 unknown
blue: 2 cool
other: 3 unknown

¬
hir 0
module module.0(046-on-string-patterns.input)
import module.1(std.io)
import import.0 prn from module.2(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.5: fn (string) -> string
bind prn = import.0
bind input = import.1
bind choice = value.0
bind warm = value.1
bind cool = value.2
bind unknown = value.3
bind classify = fn.0
bind label = fn.1
bind main = fn.2
const value.0: string = string "blue"
const value.1: string = string "warm"
const value.2: string = string "cool"
const value.3: string = string "unknown"
func fn.0(choice: string) -> i32 {
  return i32 on string local.0(choice) {
    value(string "red"), value(string "green") => {
      expr i32 1
    }
    value(string "blue") => {
      expr i32 2
    }
    else => {
      expr i32 3
    }
  }
}
func fn.1(choice: string) -> string {
  return string on string local.1(choice) {
    value(string "red") => {
      expr string bind.3(warm)
    }
    value(string "blue") => {
      expr string bind.4(cool)
    }
    else => {
      expr string bind.5(unknown)
    }
  }
}
func fn.2() -> i32 {
  expr void call bind.0(prn)(string interpolate(<unknown> "red: ", i32 call bind.6(classify)(string "red"), <unknown> " ", string call bind.7(label)(string "red")))
  expr void call bind.0(prn)(string interpolate(<unknown> "green: ", i32 call bind.6(classify)(string "green"), <unknown> " ", string call bind.7(label)(string "green")))
  expr void call bind.0(prn)(string interpolate(<unknown> "blue: ", i32 call bind.6(classify)(string "blue"), <unknown> " ", string call bind.7(label)(string "blue")))
  expr void call bind.0(prn)(string interpolate(<unknown> "other: ", i32 call bind.6(classify)(string "other"), <unknown> " ", string call bind.7(label)(string "other")))
  return i32 call bind.6(classify)(string bind.2(choice))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [5 x i8] c"blue\00"
@.str.m0.1 = private unnamed_addr constant [5 x i8] c"warm\00"
@.str.m0.2 = private unnamed_addr constant [5 x i8] c"cool\00"
@.str.m0.3 = private unnamed_addr constant [8 x i8] c"unknown\00"
@.str.m0.4 = private unnamed_addr constant [4 x i8] c"red\00"
@.str.m0.5 = private unnamed_addr constant [6 x i8] c"green\00"
@.str.m0.6 = private unnamed_addr constant [5 x i8] c"blue\00"
@.str.m0.7 = private unnamed_addr constant [4 x i8] c"red\00"
@.str.m0.8 = private unnamed_addr constant [5 x i8] c"blue\00"
@.str.m0.9 = private unnamed_addr constant [6 x i8] c"red: \00"
@.str.m0.10 = private unnamed_addr constant [4 x i8] c"red\00"
@.str.m0.11 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.12 = private unnamed_addr constant [4 x i8] c"red\00"
@.str.m0.13 = private unnamed_addr constant [8 x i8] c"green: \00"
@.str.m0.14 = private unnamed_addr constant [6 x i8] c"green\00"
@.str.m0.15 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.16 = private unnamed_addr constant [6 x i8] c"green\00"
@.str.m0.17 = private unnamed_addr constant [7 x i8] c"blue: \00"
@.str.m0.18 = private unnamed_addr constant [5 x i8] c"blue\00"
@.str.m0.19 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.20 = private unnamed_addr constant [5 x i8] c"blue\00"
@.str.m0.21 = private unnamed_addr constant [8 x i8] c"other: \00"
@.str.m0.22 = private unnamed_addr constant [6 x i8] c"other\00"
@.str.m0.23 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.24 = private unnamed_addr constant [6 x i8] c"other\00"

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
declare { ptr, i64 } @$input({ ptr, i64 })

define internal i32 @fn.0({ ptr, i64 } %choice) {
  %t0 = alloca { ptr, i64 }
  store { ptr, i64 } %choice, ptr %t0
  %t1 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 3 }, ptr %t1
  %t2 = call i1 @string_eq(ptr %t0, ptr %t1)
  %t3 = alloca { ptr, i64 }
  store { ptr, i64 } %choice, ptr %t3
  %t4 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.5, i64 5 }, ptr %t4
  %t5 = call i1 @string_eq(ptr %t3, ptr %t4)
  %t6 = or i1 %t2, %t5
  br i1 %t6, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t7 = alloca { ptr, i64 }
  store { ptr, i64 } %choice, ptr %t7
  %t8 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.6, i64 4 }, ptr %t8
  %t9 = call i1 @string_eq(ptr %t7, ptr %t8)
  br i1 %t9, label %on.body.4, label %on.next.5
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
  %t10 = phi i32 [1, %on.value.3], [2, %on.value.6], [3, %on.value.9]
  ret i32 %t10
}

define internal { ptr, i64 } @fn.1({ ptr, i64 } %choice) {
  %t0 = alloca { ptr, i64 }
  store { ptr, i64 } %choice, ptr %t0
  %t1 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.7, i64 3 }, ptr %t1
  %t2 = call i1 @string_eq(ptr %t0, ptr %t1)
  br i1 %t2, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t3 = alloca { ptr, i64 }
  store { ptr, i64 } %choice, ptr %t3
  %t4 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.8, i64 4 }, ptr %t4
  %t5 = call i1 @string_eq(ptr %t3, ptr %t4)
  br i1 %t5, label %on.body.4, label %on.next.5
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
  %t6 = phi { ptr, i64 } [{ ptr @.str.m0.1, i64 4 }, %on.value.3], [{ ptr @.str.m0.2, i64 4 }, %on.value.6], [{ ptr @.str.m0.3, i64 7 }, %on.value.9]
  ret { ptr, i64 } %t6
}

define internal i32 @fn.2() {
  %t0 = call i64 @string_builder_mark()
  %t1 = alloca { ptr, i64 }
  %t2 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.9, i64 5 }, ptr %t2
  call void @to_string$string(ptr %t1, ptr %t2)
  call void @string_builder_append_string(ptr %t1)
  %t3 = call i32 @fn.0({ ptr, i64 } { ptr @.str.m0.10, i64 3 })
  %t4 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t4, i32 %t3)
  call void @string_builder_append_string(ptr %t4)
  %t5 = alloca { ptr, i64 }
  %t6 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.11, i64 1 }, ptr %t6
  call void @to_string$string(ptr %t5, ptr %t6)
  call void @string_builder_append_string(ptr %t5)
  %t7 = call { ptr, i64 } @fn.1({ ptr, i64 } { ptr @.str.m0.12, i64 3 })
  %t8 = alloca { ptr, i64 }
  %t9 = alloca { ptr, i64 }
  store { ptr, i64 } %t7, ptr %t9
  call void @to_string$string(ptr %t8, ptr %t9)
  call void @string_builder_append_string(ptr %t8)
  %t10 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t10, i64 %t0)
  %t11 = load { ptr, i64 }, ptr %t10
  call void @$prn({ ptr, i64 } %t11)
  %t12 = call i64 @string_builder_mark()
  %t13 = alloca { ptr, i64 }
  %t14 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.13, i64 7 }, ptr %t14
  call void @to_string$string(ptr %t13, ptr %t14)
  call void @string_builder_append_string(ptr %t13)
  %t15 = call i32 @fn.0({ ptr, i64 } { ptr @.str.m0.14, i64 5 })
  %t16 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t16, i32 %t15)
  call void @string_builder_append_string(ptr %t16)
  %t17 = alloca { ptr, i64 }
  %t18 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.15, i64 1 }, ptr %t18
  call void @to_string$string(ptr %t17, ptr %t18)
  call void @string_builder_append_string(ptr %t17)
  %t19 = call { ptr, i64 } @fn.1({ ptr, i64 } { ptr @.str.m0.16, i64 5 })
  %t20 = alloca { ptr, i64 }
  %t21 = alloca { ptr, i64 }
  store { ptr, i64 } %t19, ptr %t21
  call void @to_string$string(ptr %t20, ptr %t21)
  call void @string_builder_append_string(ptr %t20)
  %t22 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t22, i64 %t12)
  %t23 = load { ptr, i64 }, ptr %t22
  call void @$prn({ ptr, i64 } %t23)
  %t24 = call i64 @string_builder_mark()
  %t25 = alloca { ptr, i64 }
  %t26 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.17, i64 6 }, ptr %t26
  call void @to_string$string(ptr %t25, ptr %t26)
  call void @string_builder_append_string(ptr %t25)
  %t27 = call i32 @fn.0({ ptr, i64 } { ptr @.str.m0.18, i64 4 })
  %t28 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t28, i32 %t27)
  call void @string_builder_append_string(ptr %t28)
  %t29 = alloca { ptr, i64 }
  %t30 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.19, i64 1 }, ptr %t30
  call void @to_string$string(ptr %t29, ptr %t30)
  call void @string_builder_append_string(ptr %t29)
  %t31 = call { ptr, i64 } @fn.1({ ptr, i64 } { ptr @.str.m0.20, i64 4 })
  %t32 = alloca { ptr, i64 }
  %t33 = alloca { ptr, i64 }
  store { ptr, i64 } %t31, ptr %t33
  call void @to_string$string(ptr %t32, ptr %t33)
  call void @string_builder_append_string(ptr %t32)
  %t34 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t34, i64 %t24)
  %t35 = load { ptr, i64 }, ptr %t34
  call void @$prn({ ptr, i64 } %t35)
  %t36 = call i64 @string_builder_mark()
  %t37 = alloca { ptr, i64 }
  %t38 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.21, i64 7 }, ptr %t38
  call void @to_string$string(ptr %t37, ptr %t38)
  call void @string_builder_append_string(ptr %t37)
  %t39 = call i32 @fn.0({ ptr, i64 } { ptr @.str.m0.22, i64 5 })
  %t40 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t40, i32 %t39)
  call void @string_builder_append_string(ptr %t40)
  %t41 = alloca { ptr, i64 }
  %t42 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.23, i64 1 }, ptr %t42
  call void @to_string$string(ptr %t41, ptr %t42)
  call void @string_builder_append_string(ptr %t41)
  %t43 = call { ptr, i64 } @fn.1({ ptr, i64 } { ptr @.str.m0.24, i64 5 })
  %t44 = alloca { ptr, i64 }
  %t45 = alloca { ptr, i64 }
  store { ptr, i64 } %t43, ptr %t45
  call void @to_string$string(ptr %t44, ptr %t45)
  call void @string_builder_append_string(ptr %t44)
  %t46 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t46, i64 %t36)
  %t47 = load { ptr, i64 }, ptr %t46
  call void @$prn({ ptr, i64 } %t47)
  %t48 = call i32 @fn.0({ ptr, i64 } { ptr @.str.m0.0, i64 4 })
  ret i32 %t48
}

@$classify = internal alias i32 ({ ptr, i64 }), ptr @fn.0
@$label = internal alias { ptr, i64 } ({ ptr, i64 }), ptr @fn.1
@$main = alias i32 (), ptr @fn.2