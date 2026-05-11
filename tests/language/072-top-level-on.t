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
  store { ptr, i64 } { ptr @.str.m0.0, i64 7 }, ptr %t2
  call void @to_string$string(ptr %t1, ptr %t2)
  call void @string_builder_append_string(ptr %t1)
  %t3 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t3, i32 7)
  call void @string_builder_append_string(ptr %t3)
  %t4 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t4, i64 %t0)
  %t5 = load { ptr, i64 }, ptr %t4
  call void @$prn({ ptr, i64 } %t5)
  %t6 = call i64 @string_builder_mark()
  %t7 = alloca { ptr, i64 }
  %t8 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 9 }, ptr %t8
  call void @to_string$string(ptr %t7, ptr %t8)
  call void @string_builder_append_string(ptr %t7)
  %t9 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t9, i32 9)
  call void @string_builder_append_string(ptr %t9)
  %t10 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t10, i64 %t6)
  %t11 = load { ptr, i64 }, ptr %t10
  call void @$prn({ ptr, i64 } %t11)
  ret void
}

@$main = alias void (), ptr @fn.0
