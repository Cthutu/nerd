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
        Some(as value) => prn($"generic {value}")
        None => prn("none")
    }
}
¬
0
¬
generic 8

¬
hir 0
module module.0(127-generic-types.input)
import import.0 prn from module.1(core).decl.13: fn (string) -> void
bind prn = import.0
bind Box = type.0
bind Cell = type.1
bind Maybe = type.2
bind main = fn.0
generic type type.0 = <unknown>
generic type type.1 = <unknown>
generic type type.2 = <unknown>
func fn.0() -> void {
  expr <unknown> default
  let b: plex { i32 value } = <unknown> default
  assign i32 field(plex { i32 value } local.0(b), value) = i32 7
  expr <unknown> default
  let c: union { i32 number, string text } = <unknown> default
  assign i32 field(union { i32 number, string text } local.1(c), number) = i32 add(i32 field(plex { i32 value } local.0(b), value), i32 1)
  let m: enum { None, Some(i32) } = enum { None, Some(i32) } call Some(i32 field(union { i32 number, string text } local.1(c), number))
  expr void on enum { None, Some(i32) } local.2(m) {
    enum_variant(Some, as value) => {
      expr void call bind.0(prn)(string interpolate(<unknown> "generic ", i32 local.3(value)))
    }
    value(enum { None, Some(i32) } None) => {
      expr void call bind.0(prn)(string "none")
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [9 x i8] c"generic \00"
@.str.m0.1 = private unnamed_addr constant [5 x i8] c"none\00"

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

define internal void @fn.0() {
  %local.0 = alloca { i32 }
  store { i32 } zeroinitializer, ptr %local.0
  %t0 = getelementptr inbounds { i32 }, ptr %local.0, i64 0, i32 0
  store i32 7, ptr %t0
  %t1 = load { i32 }, ptr %local.0
  %t2 = extractvalue { i32 } %t1, 0
  %t3 = add i32 %t2, 1
  %t4 = zext i32 %t3 to i128
  %local.1 = alloca i128
  store i128 %t4, ptr %local.1
  %t5 = load i128, ptr %local.1
  %t6 = trunc i128 %t5 to i32
  %t7 = zext i32 %t6 to i64
  %t8 = insertvalue { i64, i64 } poison, i64 1, 0
  %t9 = insertvalue { i64, i64 } %t8, i64 %t7, 1
  %t10 = extractvalue { i64, i64 } %t9, 0
  %t11 = icmp eq i64 %t10, 1
  %t12 = extractvalue { i64, i64 } %t9, 1
  %t13 = trunc i64 %t12 to i32
  %t14 = and i1 %t11, 1
  br i1 %t14, label %on.body.1, label %on.next.2
on.body.1:
  %t15 = call i64 @string_builder_mark()
  %t16 = alloca { ptr, i64 }
  %t17 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 8 }, ptr %t17
  call void @to_string$string(ptr %t16, ptr %t17)
  call void @string_builder_append_string(ptr %t16)
  %t18 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t18, i32 %t13)
  call void @string_builder_append_string(ptr %t18)
  %t19 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t19, i64 %t15)
  %t20 = load { ptr, i64 }, ptr %t19
  call void @$prn({ ptr, i64 } %t20)
  br label %on.end.0
on.next.2:
  %t21 = insertvalue { i64, i64 } poison, i64 0, 0
  %t22 = insertvalue { i64, i64 } %t21, i64 0, 1
  %t23 = extractvalue { i64, i64 } %t9, 0
  %t24 = extractvalue { i64, i64 } %t22, 0
  %t25 = icmp eq i64 %t23, %t24
  br i1 %t25, label %on.body.3, label %on.end.0
on.body.3:
  call void @$prn({ ptr, i64 } { ptr @.str.m0.1, i64 4 })
  br label %on.end.0
on.end.0:
  ret void
}

@$main = alias void (), ptr @fn.0