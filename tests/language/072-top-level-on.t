on debug {
    use std.io
    answer :: 7

    on !feature {
        fallback :: 9
    }
}

main :: fn () {
    prn($"answer={answer}")
    prn($"fallback={fallback}")
}
¬
0
¬
answer=7
fallback=9

¬
hir 0
module module.0(072-top-level-on.input)
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
bind answer = value.0
bind fallback = value.1
bind main = fn.0
const value.0: untyped integer = untyped integer 7
const value.1: untyped integer = untyped integer 9
func fn.0() -> void {
  expr void call bind.2(prn)(string interpolate(<unknown> "answer=", untyped integer bind.5(answer)))
  expr void call bind.2(prn)(string interpolate(<unknown> "fallback=", untyped integer bind.6(fallback)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [8 x i8] c"answer=\00"
@.str.m0.1 = private unnamed_addr constant [10 x i8] c"fallback=\00"

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
  %t1 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 7 })
  call void @string_builder_append_string({ ptr, i64 } %t1)
  %t2 = call { ptr, i64 } @to_string$i32(i32 7)
  call void @string_builder_append_string({ ptr, i64 } %t2)
  %t3 = call { ptr, i64 } @string_builder_finish(i64 %t0)
  call void @$prn({ ptr, i64 } %t3)
  %t4 = call i64 @string_builder_mark()
  %t5 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 9 })
  call void @string_builder_append_string({ ptr, i64 } %t5)
  %t6 = call { ptr, i64 } @to_string$i32(i32 9)
  call void @string_builder_append_string({ ptr, i64 } %t6)
  %t7 = call { ptr, i64 } @string_builder_finish(i64 %t4)
  call void @$prn({ ptr, i64 } %t7)
  ret void
}

@$main = alias void (), ptr @fn.0
