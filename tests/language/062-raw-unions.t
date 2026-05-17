use std.io

Value :: union {
    i i32
    f f32
}

main :: fn () -> i32 {
    a: Value = Value { i: 42 }
    b: Value = Value { f: 3.5 }

    prn($"i {a.i}")
    prn($"f {b.f}")

    return a.i
}
¬
42
¬
i 42
f 3.5

¬
hir 0
module module.0(062-raw-unions.input)
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
bind Value = type.0
bind main = fn.0
type type.0 = Value
func fn.0() -> i32 {
  let a: Value = Value plex(i: i32 42)
  let b: Value = Value plex(f: f32 3.5)
  expr void call bind.2(prn)(string interpolate(<unknown> "i ", i32 field(Value local.0(a), i)))
  expr void call bind.2(prn)(string interpolate(<unknown> "f ", f32 field(Value local.1(b), f)))
  return i32 field(Value local.0(a), i)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [3 x i8] c"i \00"
@.str.m0.1 = private unnamed_addr constant [3 x i8] c"f \00"

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

define internal i32 @fn.0() {
  %t0 = bitcast float 0x400C000000000000 to i32
  %t1 = call i64 @string_builder_mark()
  %t2 = alloca { ptr, i64 }
  %t3 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 2 }, ptr %t3
  call void @to_string$string(ptr %t2, ptr %t3)
  call void @string_builder_append_string(ptr %t2)
  %t4 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t4, i32 42)
  call void @string_builder_append_string(ptr %t4)
  %t5 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t5, i64 %t1)
  %t6 = load { ptr, i64 }, ptr %t5
  call void @$prn({ ptr, i64 } %t6)
  %t7 = call i64 @string_builder_mark()
  %t8 = alloca { ptr, i64 }
  %t9 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 2 }, ptr %t9
  call void @to_string$string(ptr %t8, ptr %t9)
  call void @string_builder_append_string(ptr %t8)
  %t10 = bitcast i32 %t0 to float
  %t11 = alloca { ptr, i64 }
  call void @to_string$f32(ptr %t11, float %t10)
  call void @string_builder_append_string(ptr %t11)
  %t12 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t12, i64 %t7)
  %t13 = load { ptr, i64 }, ptr %t12
  call void @$prn({ ptr, i64 } %t13)
  ret i32 42
}

@$main = alias i32 (), ptr @fn.0
