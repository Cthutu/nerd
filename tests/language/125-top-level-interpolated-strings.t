name :: "world"
count :: 40 + 2
truth :: yes
greeting :: $"Hello, {name}! count={count}, ok={truth}"

make :: fn () -> string {
    return $"constant {name} {count}"
}

main :: fn () => ((greeting != "Hello, world! count=42, ok=yes") ||
                  (make() != "constant world 42")).as(i32)
¬
0
¬

¬
hir 0
bind name = value.0
bind count = value.1
bind truth = value.2
bind greeting = value.3
bind make = fn.0
bind main = fn.1
const value.0: string = string "world"
const value.1: untyped integer = untyped integer add(untyped integer 40, untyped integer 2)
const value.2: bool = bool yes
const value.3: string = string interpolate(<unknown> "Hello, ", string bind.0(name), <unknown> "! count=", untyped integer bind.1(count), <unknown> ", ok=", bool bind.2(truth))
func fn.0() -> string {
  return string interpolate(<unknown> "constant ", string bind.0(name), <unknown> " ", untyped integer bind.1(count))
}
func fn.1() -> i32 {
  return i32 cast(bool logical_or(bool not_equal(string bind.3(greeting), string "Hello, world! count=42, ok=yes"), bool not_equal(string call bind.4(make)(), string "constant world 42")) as i32)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [52 x i8] c"tests/language/125-top-level-interpolated-strings.t\00"
@.str.m0.0 = private unnamed_addr constant [6 x i8] c"world\00"
@.str.m0.1 = private unnamed_addr constant [8 x i8] c"Hello, \00"
@.str.m0.2 = private unnamed_addr constant [9 x i8] c"! count=\00"
@.str.m0.3 = private unnamed_addr constant [6 x i8] c", ok=\00"
@.str.m0.4 = private unnamed_addr constant [10 x i8] c"constant \00"
@.str.m0.5 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.6 = private unnamed_addr constant [31 x i8] c"Hello, world! count=42, ok=yes\00"
@.str.m0.7 = private unnamed_addr constant [18 x i8] c"constant world 42\00"

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

define internal { ptr, i64 } @fn.0() {
  %t2 = alloca { ptr, i64 }
  %t4 = alloca { ptr, i64 }
  %t6 = alloca { ptr, i64 }
  %t0 = call i64 @string_builder_mark()
  %t1 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 9 }, ptr %t2
  call void @to_string$string(ptr %t1, ptr %t2)
  call void @string_builder_append_string(ptr %t1)
  %t3 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 5 }, ptr %t4
  call void @to_string$string(ptr %t3, ptr %t4)
  call void @string_builder_append_string(ptr %t3)
  %t5 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.5, i64 1 }, ptr %t6
  call void @to_string$string(ptr %t5, ptr %t6)
  call void @string_builder_append_string(ptr %t5)
  %t7 = add i32 40, 2
  %t8 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t8, i32 %t7)
  call void @string_builder_append_string(ptr %t8)
  %t9 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t9, i64 %t0)
  %t10 = load { ptr, i64 }, ptr %t9
  ret { ptr, i64 } %t10
}

define internal i32 @fn.1() {
  %t2 = alloca { ptr, i64 }
  %t4 = alloca { ptr, i64 }
  %t6 = alloca { ptr, i64 }
  %t10 = alloca { ptr, i64 }
  %t15 = alloca { ptr, i64 }
  %t16 = alloca { ptr, i64 }
  %t22 = alloca { ptr, i64 }
  %t23 = alloca { ptr, i64 }
  %t0 = call i64 @string_builder_mark()
  %t1 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 7 }, ptr %t2
  call void @to_string$string(ptr %t1, ptr %t2)
  call void @string_builder_append_string(ptr %t1)
  %t3 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 5 }, ptr %t4
  call void @to_string$string(ptr %t3, ptr %t4)
  call void @string_builder_append_string(ptr %t3)
  %t5 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 8 }, ptr %t6
  call void @to_string$string(ptr %t5, ptr %t6)
  call void @string_builder_append_string(ptr %t5)
  %t7 = add i32 40, 2
  %t8 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t8, i32 %t7)
  call void @string_builder_append_string(ptr %t8)
  %t9 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 5 }, ptr %t10
  call void @to_string$string(ptr %t9, ptr %t10)
  call void @string_builder_append_string(ptr %t9)
  %t11 = alloca { ptr, i64 }
  call void @to_string$bool(ptr %t11, i1 zeroext 1)
  call void @string_builder_append_string(ptr %t11)
  %t12 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t12, i64 %t0)
  %t13 = load { ptr, i64 }, ptr %t12
  store { ptr, i64 } %t13, ptr %t15
  store { ptr, i64 } { ptr @.str.m0.6, i64 30 }, ptr %t16
  %t14 = call i1 @string_eq(ptr %t15, ptr %t16)
  %t17 = xor i1 %t14, 1
  %t18 = alloca i1
  br i1 %t17, label %logical.short.1, label %logical.rhs.0
logical.short.1:
  store i1 1, ptr %t18
  br label %logical.end.2
logical.rhs.0:
  %t20 = call { ptr, i64 } @fn.0()
  store { ptr, i64 } %t20, ptr %t22
  store { ptr, i64 } { ptr @.str.m0.7, i64 17 }, ptr %t23
  %t21 = call i1 @string_eq(ptr %t22, ptr %t23)
  %t24 = xor i1 %t21, 1
  store i1 %t24, ptr %t18
  br label %logical.end.2
logical.end.2:
  %t19 = load i1, ptr %t18
  %t25 = zext i1 %t19 to i32
  ret i32 %t25
}

@$make = internal alias { ptr, i64 } (), ptr @fn.0
@$main = alias i32 (), ptr @fn.1

