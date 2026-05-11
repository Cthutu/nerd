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
      expr string bind.6(warm)
    }
    value(string "blue") => {
      expr string bind.7(cool)
    }
    else => {
      expr string bind.8(unknown)
    }
  }
}
func fn.2() -> i32 {
  expr void call bind.2(prn)(string interpolate(<unknown> "red: ", i32 call bind.9(classify)(string "red"), <unknown> " ", string call bind.10(label)(string "red")))
  expr void call bind.2(prn)(string interpolate(<unknown> "green: ", i32 call bind.9(classify)(string "green"), <unknown> " ", string call bind.10(label)(string "green")))
  expr void call bind.2(prn)(string interpolate(<unknown> "blue: ", i32 call bind.9(classify)(string "blue"), <unknown> " ", string call bind.10(label)(string "blue")))
  expr void call bind.2(prn)(string interpolate(<unknown> "other: ", i32 call bind.9(classify)(string "other"), <unknown> " ", string call bind.10(label)(string "other")))
  return i32 call bind.9(classify)(string bind.5(choice))
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

define internal i32 @fn.0({ ptr, i64 } %choice) {
  %t0 = call i1 @string_eq({ ptr, i64 } %choice, { ptr, i64 } { ptr @.str.m0.4, i64 3 })
  %t1 = call i1 @string_eq({ ptr, i64 } %choice, { ptr, i64 } { ptr @.str.m0.5, i64 5 })
  %t2 = or i1 %t0, %t1
  br i1 %t2, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t3 = call i1 @string_eq({ ptr, i64 } %choice, { ptr, i64 } { ptr @.str.m0.6, i64 4 })
  br i1 %t3, label %on.body.4, label %on.next.5
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
  %t4 = phi i32 [1, %on.value.3], [2, %on.value.6], [3, %on.value.9]
  ret i32 %t4
}

define internal { ptr, i64 } @fn.1({ ptr, i64 } %choice) {
  %t0 = call i1 @string_eq({ ptr, i64 } %choice, { ptr, i64 } { ptr @.str.m0.7, i64 3 })
  br i1 %t0, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t1 = call i1 @string_eq({ ptr, i64 } %choice, { ptr, i64 } { ptr @.str.m0.8, i64 4 })
  br i1 %t1, label %on.body.4, label %on.next.5
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
  %t2 = phi { ptr, i64 } [{ ptr @.str.m0.1, i64 4 }, %on.value.3], [{ ptr @.str.m0.2, i64 4 }, %on.value.6], [{ ptr @.str.m0.3, i64 7 }, %on.value.9]
  ret { ptr, i64 } %t2
}

define internal i32 @fn.2() {
  %t0 = call i64 @string_builder_mark()
  %t1 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.9, i64 5 })
  call void @string_builder_append_string({ ptr, i64 } %t1)
  %t2 = call i32 @fn.0({ ptr, i64 } { ptr @.str.m0.10, i64 3 })
  %t3 = call { ptr, i64 } @to_string$i32(i32 %t2)
  call void @string_builder_append_string({ ptr, i64 } %t3)
  %t4 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.11, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t4)
  %t5 = call { ptr, i64 } @fn.1({ ptr, i64 } { ptr @.str.m0.12, i64 3 })
  %t6 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t5)
  call void @string_builder_append_string({ ptr, i64 } %t6)
  %t7 = call { ptr, i64 } @string_builder_finish(i64 %t0)
  call void @$prn({ ptr, i64 } %t7)
  %t8 = call i64 @string_builder_mark()
  %t9 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.13, i64 7 })
  call void @string_builder_append_string({ ptr, i64 } %t9)
  %t10 = call i32 @fn.0({ ptr, i64 } { ptr @.str.m0.14, i64 5 })
  %t11 = call { ptr, i64 } @to_string$i32(i32 %t10)
  call void @string_builder_append_string({ ptr, i64 } %t11)
  %t12 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.15, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t12)
  %t13 = call { ptr, i64 } @fn.1({ ptr, i64 } { ptr @.str.m0.16, i64 5 })
  %t14 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t13)
  call void @string_builder_append_string({ ptr, i64 } %t14)
  %t15 = call { ptr, i64 } @string_builder_finish(i64 %t8)
  call void @$prn({ ptr, i64 } %t15)
  %t16 = call i64 @string_builder_mark()
  %t17 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.17, i64 6 })
  call void @string_builder_append_string({ ptr, i64 } %t17)
  %t18 = call i32 @fn.0({ ptr, i64 } { ptr @.str.m0.18, i64 4 })
  %t19 = call { ptr, i64 } @to_string$i32(i32 %t18)
  call void @string_builder_append_string({ ptr, i64 } %t19)
  %t20 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.19, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t20)
  %t21 = call { ptr, i64 } @fn.1({ ptr, i64 } { ptr @.str.m0.20, i64 4 })
  %t22 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t21)
  call void @string_builder_append_string({ ptr, i64 } %t22)
  %t23 = call { ptr, i64 } @string_builder_finish(i64 %t16)
  call void @$prn({ ptr, i64 } %t23)
  %t24 = call i64 @string_builder_mark()
  %t25 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.21, i64 7 })
  call void @string_builder_append_string({ ptr, i64 } %t25)
  %t26 = call i32 @fn.0({ ptr, i64 } { ptr @.str.m0.22, i64 5 })
  %t27 = call { ptr, i64 } @to_string$i32(i32 %t26)
  call void @string_builder_append_string({ ptr, i64 } %t27)
  %t28 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.23, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t28)
  %t29 = call { ptr, i64 } @fn.1({ ptr, i64 } { ptr @.str.m0.24, i64 5 })
  %t30 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t29)
  call void @string_builder_append_string({ ptr, i64 } %t30)
  %t31 = call { ptr, i64 } @string_builder_finish(i64 %t24)
  call void @$prn({ ptr, i64 } %t31)
  %t32 = call i32 @fn.0({ ptr, i64 } { ptr @.str.m0.0, i64 4 })
  ret i32 %t32
}

@$classify = internal alias i32 ({ ptr, i64 }), ptr @fn.0
@$label = internal alias { ptr, i64 } ({ ptr, i64 }), ptr @fn.1
@$main = alias i32 (), ptr @fn.2
