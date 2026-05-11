use std.io

Person :: plex { name string age u8 }

matt :: Person {
name: "Matt"
          age: 53
}

main :: fn () {
    prn($"His name is {matt.name} and he is {matt.age} years old")
}
¬
0
¬
His name is Matt and he is 53 years old

¬
hir 0
module module.0(057-plex-newline-fields.input)
import module.1(std.io)
import import.0 pr from module.1(std.io).decl.6: fn (string) -> void
import import.1 epr from module.1(std.io).decl.7: fn (string) -> void
import import.2 prn from module.1(std.io).decl.8: fn (string) -> void
import import.3 eprn from module.1(std.io).decl.9: fn (string) -> void
import import.4 input from module.1(std.io).decl.10: fn (string) -> string
bind pr = import.0
bind epr = import.1
bind prn = import.2
bind eprn = import.3
bind input = import.4
bind Person = type.0
bind matt = value.0
bind main = fn.0
type type.0 = Person
const value.0: Person = Person plex(name: string "Matt", age: u8 53)
func fn.0() -> void {
  expr void call bind.2(prn)(string interpolate(<unknown> "His name is ", string field(Person bind.6(matt), name), <unknown> " and he is ", u8 field(Person bind.6(matt), age), <unknown> " years old"))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [5 x i8] c"Matt\00"
@.str.m0.1 = private unnamed_addr constant [13 x i8] c"His name is \00"
@.str.m0.2 = private unnamed_addr constant [12 x i8] c" and he is \00"
@.str.m0.3 = private unnamed_addr constant [11 x i8] c" years old\00"

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

define internal void @fn.0() {
  %t0 = call i64 @string_builder_mark()
  %t1 = alloca { ptr, i64 }
  %t2 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 12 }, ptr %t2
  call void @to_string$string(ptr %t1, ptr %t2)
  call void @string_builder_append_string(ptr %t1)
  %t3 = insertvalue { { ptr, i64 }, i8 } poison, { ptr, i64 } { ptr @.str.m0.0, i64 4 }, 0
  %t4 = insertvalue { { ptr, i64 }, i8 } %t3, i8 53, 1
  %t5 = extractvalue { { ptr, i64 }, i8 } %t4, 0
  %t6 = alloca { ptr, i64 }
  %t7 = alloca { ptr, i64 }
  store { ptr, i64 } %t5, ptr %t7
  call void @to_string$string(ptr %t6, ptr %t7)
  call void @string_builder_append_string(ptr %t6)
  %t8 = alloca { ptr, i64 }
  %t9 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 11 }, ptr %t9
  call void @to_string$string(ptr %t8, ptr %t9)
  call void @string_builder_append_string(ptr %t8)
  %t10 = insertvalue { { ptr, i64 }, i8 } poison, { ptr, i64 } { ptr @.str.m0.0, i64 4 }, 0
  %t11 = insertvalue { { ptr, i64 }, i8 } %t10, i8 53, 1
  %t12 = extractvalue { { ptr, i64 }, i8 } %t11, 1
  %t13 = alloca { ptr, i64 }
  call void @to_string$u8(ptr %t13, i8 %t12)
  call void @string_builder_append_string(ptr %t13)
  %t14 = alloca { ptr, i64 }
  %t15 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 10 }, ptr %t15
  call void @to_string$string(ptr %t14, ptr %t15)
  call void @string_builder_append_string(ptr %t14)
  %t16 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t16, i64 %t0)
  %t17 = load { ptr, i64 }, ptr %t16
  call void @$prn({ ptr, i64 } %t17)
  ret void
}

@$main = alias void (), ptr @fn.0
