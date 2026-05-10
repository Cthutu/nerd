io :: use std.io

Box :: plex [T] {
    value T
}

Cell :: union [T] {
    number T
    text   string
}

Maybe :: enum [T] { None Some(T) }

main :: fn () {
    b: Box[i32]
    b.value = 7

    c: Cell[i32]
    c.number = b.value + 1

    m: Maybe[i32] = Some(c.number)
    on m {
        Some(as value) => io.prn($"generic {value}")
        None => io.prn("none")
    }
}
¬
0
¬
generic 8

¬
hir 0
module module.0(127-generic-types.input)
import module.1(std.io)
import import.0 prn from module.1(std.io).decl.11: fn (string) -> void
bind prn = import.0
bind io = module.1
bind Box = type.0
bind Cell = type.1
bind Maybe = type.2
bind main = fn.0
generic type type.0 = <unknown>
generic type type.1 = <unknown>
generic type type.2 = <unknown>
func fn.0() -> void {
  expr <unknown> <unsupported>
  let b: plex { i32 value } = <unknown> <unsupported>
  assign i32 field(plex { i32 value } local.0(b), value) = i32 7
  expr <unknown> <unsupported>
  let c: union { i32 number, string text } = <unknown> <unsupported>
  assign i32 field(union { i32 number, string text } local.1(c), number) = i32 add(i32 field(plex { i32 value } local.0(b), value), i32 1)
  let m: enum { None, Some(i32) } = enum { None, Some(i32) } call Some(i32 field(union { i32 number, string text } local.1(c), number))
  expr void on enum { None, Some(i32) } local.2(m) {
    enum_variant(Some, as value) => {
      expr void call fn (string) -> void field(module bind.1(io), prn)(string interpolate(<unknown> "generic ", i32 local.3(value)))
    }
    value(enum { None, Some(i32) } None) => {
      expr void call fn (string) -> void field(module bind.1(io), prn)(string "none")
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [9 x i8] c"generic \00"
@.str.m0.1 = private unnamed_addr constant [5 x i8] c"none\00"

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

declare void @$prn({ ptr, i64 })

define void @fn.0() {
  %local.0 = alloca { i32 }
  store { i32 } zeroinitializer, ptr %local.0
  %t0 = getelementptr inbounds { i32 }, ptr %local.0, i64 0, i32 0
  store i32 7, ptr %t0
  %t1 = load { i32 }, ptr %local.0
  %t2 = extractvalue { i32 } %t1, 0
  %t3 = add i32 %t2, 1
  ret void
}

@$main = alias void (), ptr @fn.0
