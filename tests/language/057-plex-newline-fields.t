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

define internal void @fn.0() {
  %t0 = call i64 @string_builder_mark()
  %t1 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 12 })
  call void @string_builder_append_string({ ptr, i64 } %t1)
  %t2 = insertvalue { { ptr, i64 }, i8 } poison, { ptr, i64 } { ptr @.str.m0.0, i64 4 }, 0
  %t3 = insertvalue { { ptr, i64 }, i8 } %t2, i8 53, 1
  %t4 = extractvalue { { ptr, i64 }, i8 } %t3, 0
  %t5 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t4)
  call void @string_builder_append_string({ ptr, i64 } %t5)
  %t6 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 11 })
  call void @string_builder_append_string({ ptr, i64 } %t6)
  %t7 = insertvalue { { ptr, i64 }, i8 } poison, { ptr, i64 } { ptr @.str.m0.0, i64 4 }, 0
  %t8 = insertvalue { { ptr, i64 }, i8 } %t7, i8 53, 1
  %t9 = extractvalue { { ptr, i64 }, i8 } %t8, 1
  %t10 = call { ptr, i64 } @to_string$u8(i8 %t9)
  call void @string_builder_append_string({ ptr, i64 } %t10)
  %t11 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.3, i64 10 })
  call void @string_builder_append_string({ ptr, i64 } %t11)
  %t12 = call { ptr, i64 } @string_builder_finish(i64 %t0)
  call void @$prn({ ptr, i64 } %t12)
  ret void
}

@$main = alias void (), ptr @fn.0
