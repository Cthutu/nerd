use std.io

-- Builds an interpolated string from a constant and a variable.
name :: "world"
count := 3

check_escape :: fn (escape: string) -> bool {
    on escape.count != 4 => return no
    on escape.data[0] != 27 => return no
    on escape.data[1] != '[' => return no
    on escape.data[2] != '3' => return no
    on escape.data[3] != 'm' => return no
    return yes
}

main :: fn () {
    prn($"Hello, {name}! count={count}")
    on !check_escape($"\x1b[{count}m") => return
}
¬
0
¬
Hello, world! count=3

¬
hir 0
module module.0(019-interpolated-strings.input)
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
bind name = value.0
bind count = value.1
bind check_escape = fn.0
bind main = fn.1
const value.0: string = string "world"
global value.1: i32 = untyped integer 3
func fn.0(escape: string) -> bool {
  expr void on bool not_equal(usize field(string local.0(escape), count), usize 4) {
    value(bool yes) => {
      return bool no
    }
  }
  expr void on bool not_equal(u8 index(^u8 field(string local.0(escape), data), untyped integer 0), u8 27) {
    value(bool yes) => {
      return bool no
    }
  }
  expr void on bool not_equal(u8 index(^u8 field(string local.0(escape), data), untyped integer 1), u8 91) {
    value(bool yes) => {
      return bool no
    }
  }
  expr void on bool not_equal(u8 index(^u8 field(string local.0(escape), data), untyped integer 2), u8 51) {
    value(bool yes) => {
      return bool no
    }
  }
  expr void on bool not_equal(u8 index(^u8 field(string local.0(escape), data), untyped integer 3), u8 109) {
    value(bool yes) => {
      return bool no
    }
  }
  return bool yes
}
func fn.1() -> void {
  expr void call bind.2(prn)(string interpolate(<unknown> "Hello, ", string bind.5(name), <unknown> "! count=", i32 bind.6(count)))
  expr void on bool logical_not(bool call bind.7(check_escape)(string interpolate(<unknown> "[", i32 bind.6(count), <unknown> "m"))) {
    value(bool yes) => {
      return <none>
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [6 x i8] c"world\00"
@.str.m0.1 = private unnamed_addr constant [8 x i8] c"Hello, \00"
@.str.m0.2 = private unnamed_addr constant [9 x i8] c"! count=\00"
@.str.m0.3 = private unnamed_addr constant [3 x i8] c"\1B[\00"
@.str.m0.4 = private unnamed_addr constant [2 x i8] c"m\00"

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

@$count = internal global i32 0

define void @m0.init() {
  store i32 3, ptr @$count
  ret void
}

define internal i1 @fn.0({ ptr, i64 } %escape) {
  %t0 = extractvalue { ptr, i64 } %escape, 1
  %t1 = icmp ne i64 %t0, 4
  %t2 = icmp eq i1 %t1, 1
  br i1 %t2, label %on.body.1, label %on.end.0
on.body.1:
  ret i1 0
on.end.0:
  %t3 = extractvalue { ptr, i64 } %escape, 0
  %t4 = getelementptr inbounds i8, ptr %t3, i32 0
  %t5 = load i8, ptr %t4
  %t6 = icmp ne i8 %t5, 27
  %t7 = icmp eq i1 %t6, 1
  br i1 %t7, label %on.body.3, label %on.end.2
on.body.3:
  ret i1 0
on.end.2:
  %t8 = extractvalue { ptr, i64 } %escape, 0
  %t9 = getelementptr inbounds i8, ptr %t8, i32 1
  %t10 = load i8, ptr %t9
  %t11 = icmp ne i8 %t10, 91
  %t12 = icmp eq i1 %t11, 1
  br i1 %t12, label %on.body.5, label %on.end.4
on.body.5:
  ret i1 0
on.end.4:
  %t13 = extractvalue { ptr, i64 } %escape, 0
  %t14 = getelementptr inbounds i8, ptr %t13, i32 2
  %t15 = load i8, ptr %t14
  %t16 = icmp ne i8 %t15, 51
  %t17 = icmp eq i1 %t16, 1
  br i1 %t17, label %on.body.7, label %on.end.6
on.body.7:
  ret i1 0
on.end.6:
  %t18 = extractvalue { ptr, i64 } %escape, 0
  %t19 = getelementptr inbounds i8, ptr %t18, i32 3
  %t20 = load i8, ptr %t19
  %t21 = icmp ne i8 %t20, 109
  %t22 = icmp eq i1 %t21, 1
  br i1 %t22, label %on.body.9, label %on.end.8
on.body.9:
  ret i1 0
on.end.8:
  ret i1 1
}

define internal void @fn.1() {
  %t0 = call i64 @string_builder_mark()
  %t1 = alloca { ptr, i64 }
  %t2 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 7 }, ptr %t2
  call void @to_string$string(ptr %t1, ptr %t2)
  call void @string_builder_append_string(ptr %t1)
  %t3 = alloca { ptr, i64 }
  %t4 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 5 }, ptr %t4
  call void @to_string$string(ptr %t3, ptr %t4)
  call void @string_builder_append_string(ptr %t3)
  %t5 = alloca { ptr, i64 }
  %t6 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 8 }, ptr %t6
  call void @to_string$string(ptr %t5, ptr %t6)
  call void @string_builder_append_string(ptr %t5)
  %t7 = load i32, ptr @$count
  %t8 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t8, i32 %t7)
  call void @string_builder_append_string(ptr %t8)
  %t9 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t9, i64 %t0)
  %t10 = load { ptr, i64 }, ptr %t9
  call void @$prn({ ptr, i64 } %t10)
  %t11 = call i64 @string_builder_mark()
  %t12 = alloca { ptr, i64 }
  %t13 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 2 }, ptr %t13
  call void @to_string$string(ptr %t12, ptr %t13)
  call void @string_builder_append_string(ptr %t12)
  %t14 = load i32, ptr @$count
  %t15 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t15, i32 %t14)
  call void @string_builder_append_string(ptr %t15)
  %t16 = alloca { ptr, i64 }
  %t17 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 1 }, ptr %t17
  call void @to_string$string(ptr %t16, ptr %t17)
  call void @string_builder_append_string(ptr %t16)
  %t18 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t18, i64 %t11)
  %t19 = load { ptr, i64 }, ptr %t18
  %t20 = call i1 @fn.0({ ptr, i64 } %t19)
  %t21 = xor i1 %t20, 1
  %t22 = icmp eq i1 %t21, 1
  br i1 %t22, label %on.body.1, label %on.end.0
on.body.1:
  ret void
on.end.0:
  ret void
}

@$check_escape = internal alias i1 ({ ptr, i64 }), ptr @fn.0
@$main = alias void (), ptr @fn.1
