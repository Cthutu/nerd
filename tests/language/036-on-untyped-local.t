use std.io

-- Block-form `on` materialises untyped local integer constants to i32.
main :: fn () {
    i :: 2

    prn(on i {
        0 => "zero"
        else as x => $"non-zero: {x}"
    })
}
¬
0
¬
non-zero: 2

¬
hir 0
module module.0(036-on-untyped-local.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind main = fn.0
func fn.0() -> void {
  let i: untyped integer = untyped integer 2
  expr void call bind.0(prn)(string on i32 local.0(i) {
    value(i32 0) => {
      expr string "zero"
    }
    else as x => {
      expr string interpolate(<unknown> "non-zero: ", i32 local.1(x))
    }
  })
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [60 x i8] c"tests/language/036-on-untyped-local.t\00"
@.str.m0.0 = private unnamed_addr constant [5 x i8] c"zero\00"
@.str.m0.1 = private unnamed_addr constant [11 x i8] c"non-zero: \00"

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
declare ptr @$input({ ptr, i64 })

define internal void @fn.0() {
  %t0 = icmp eq i32 2, 0
  br i1 %t0, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  br label %on.body.4
on.body.4:
  %t1 = call i64 @string_builder_mark()
  %t2 = alloca { ptr, i64 }
  %t3 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 10 }, ptr %t3
  call void @to_string$string(ptr %t2, ptr %t3)
  call void @string_builder_append_string(ptr %t2)
  %t4 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t4, i32 2)
  call void @string_builder_append_string(ptr %t4)
  %t5 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t5, i64 %t1)
  %t6 = load { ptr, i64 }, ptr %t5
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.end.0:
  %t7 = phi { ptr, i64 } [{ ptr @.str.m0.0, i64 4 }, %on.value.3], [%t6, %on.value.6]
  call void @$prn({ ptr, i64 } %t7)
  ret void
}

@$main = alias void (), ptr @fn.0
