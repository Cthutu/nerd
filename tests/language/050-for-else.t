use std.io

main :: fn () {
    total := 0
    found :: for i := 0; i < 1; i += 1 {
        total += i
        break total + 7
    } else {
        break -1
    }
    missing :: for j := 0; j < 0; j += 1 {
        break j
    } else {
        break 42
    }

    prn($"found = {found}")
    prn($"missing = {missing}")
    return found + missing
}
¬
49
¬
found = 7
missing = 42

¬
hir 0
module module.0(050-for-else.input)
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
bind main = fn.0
func fn.0() -> i32 {
  let total: i32 = untyped integer 0
  let found: i32 = i32 for c_style {
    init {
      let i: i32 = untyped integer 0
    }
    condition bool less(i32 local.3(i), i32 1)
    body {
      assign i32 local.2(total) = i32 add(i32 local.2(total), i32 local.3(i))
      break i32 add(i32 local.2(total), i32 7)
    }
    update {
      assign i32 local.3(i) = i32 add(i32 local.3(i), i32 1)
    }
    else {
      break i32 negate(i32 1)
    }
  }
  let missing: i32 = i32 for c_style {
    init {
      let j: i32 = untyped integer 0
    }
    condition bool less(i32 local.4(j), i32 0)
    body {
      break i32 local.4(j)
    }
    update {
      assign i32 local.4(j) = i32 add(i32 local.4(j), i32 1)
    }
    else {
      break i32 42
    }
  }
  expr void call bind.2(prn)(string interpolate(<unknown> "found = ", i32 local.0(found)))
  expr void call bind.2(prn)(string interpolate(<unknown> "missing = ", i32 local.1(missing)))
  return i32 add(i32 local.0(found), i32 local.1(missing))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [9 x i8] c"found = \00"
@.str.m0.1 = private unnamed_addr constant [11 x i8] c"missing = \00"

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
  %local.2 = alloca i32
  store i32 0, ptr %local.2
  %local.3 = alloca i32
  store i32 0, ptr %local.3
  br label %for.cond.0
for.cond.0:
  %t0 = load i32, ptr %local.3
  %t1 = icmp slt i32 %t0, 1
  br i1 %t1, label %for.body.1, label %for.else.2
for.body.1:
  %t2 = load i32, ptr %local.2
  %t3 = add i32 %t2, 7
  br label %for.end.3
for.else.2:
  %t4 = sub i32 0, 1
  br label %for.end.3
for.end.3:
  %t5 = phi i32 [%t3, %for.body.1], [%t4, %for.else.2]
  %local.4 = alloca i32
  store i32 0, ptr %local.4
  br label %for.cond.4
for.cond.4:
  %t6 = load i32, ptr %local.4
  %t7 = icmp slt i32 %t6, 0
  br i1 %t7, label %for.body.5, label %for.else.6
for.body.5:
  %t8 = load i32, ptr %local.4
  br label %for.end.7
for.else.6:
  br label %for.end.7
for.end.7:
  %t9 = phi i32 [%t8, %for.body.5], [42, %for.else.6]
  %t10 = call i64 @string_builder_mark()
  %t11 = alloca { ptr, i64 }
  %t12 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 8 }, ptr %t12
  call void @to_string$string(ptr %t11, ptr %t12)
  call void @string_builder_append_string(ptr %t11)
  %t13 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t13, i32 %t5)
  call void @string_builder_append_string(ptr %t13)
  %t14 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t14, i64 %t10)
  %t15 = load { ptr, i64 }, ptr %t14
  call void @$prn({ ptr, i64 } %t15)
  %t16 = call i64 @string_builder_mark()
  %t17 = alloca { ptr, i64 }
  %t18 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 10 }, ptr %t18
  call void @to_string$string(ptr %t17, ptr %t18)
  call void @string_builder_append_string(ptr %t17)
  %t19 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t19, i32 %t9)
  call void @string_builder_append_string(ptr %t19)
  %t20 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t20, i64 %t16)
  %t21 = load { ptr, i64 }, ptr %t20
  call void @$prn({ ptr, i64 } %t21)
  %t22 = add i32 %t5, %t9
  ret i32 %t22
}

@$main = alias i32 (), ptr @fn.0
