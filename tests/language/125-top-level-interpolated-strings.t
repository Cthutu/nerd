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

@.str.m0.0 = private unnamed_addr constant [6 x i8] c"world\00"
@.str.m0.1 = private unnamed_addr constant [8 x i8] c"Hello, \00"
@.str.m0.2 = private unnamed_addr constant [9 x i8] c"! count=\00"
@.str.m0.3 = private unnamed_addr constant [6 x i8] c", ok=\00"
@.str.m0.4 = private unnamed_addr constant [10 x i8] c"constant \00"
@.str.m0.5 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.6 = private unnamed_addr constant [31 x i8] c"Hello, world! count=42, ok=yes\00"
@.str.m0.7 = private unnamed_addr constant [18 x i8] c"constant world 42\00"

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

define internal { ptr, i64 } @fn.0() {
  %t0 = call i64 @string_builder_mark()
  %t1 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.4, i64 9 })
  call void @string_builder_append_string({ ptr, i64 } %t1)
  %t2 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 5 })
  call void @string_builder_append_string({ ptr, i64 } %t2)
  %t3 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.5, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t3)
  %t4 = add i32 40, 2
  %t5 = call { ptr, i64 } @to_string$i32(i32 %t4)
  call void @string_builder_append_string({ ptr, i64 } %t5)
  %t6 = call { ptr, i64 } @string_builder_finish(i64 %t0)
  ret { ptr, i64 } %t6
}

define internal i32 @fn.1() {
  %t0 = call i64 @string_builder_mark()
  %t1 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 7 })
  call void @string_builder_append_string({ ptr, i64 } %t1)
  %t2 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 5 })
  call void @string_builder_append_string({ ptr, i64 } %t2)
  %t3 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 8 })
  call void @string_builder_append_string({ ptr, i64 } %t3)
  %t4 = add i32 40, 2
  %t5 = call { ptr, i64 } @to_string$i32(i32 %t4)
  call void @string_builder_append_string({ ptr, i64 } %t5)
  %t6 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.3, i64 5 })
  call void @string_builder_append_string({ ptr, i64 } %t6)
  %t7 = call { ptr, i64 } @to_string$bool(i1 1)
  call void @string_builder_append_string({ ptr, i64 } %t7)
  %t8 = call { ptr, i64 } @string_builder_finish(i64 %t0)
  %t9 = call i1 @string_eq({ ptr, i64 } %t8, { ptr, i64 } { ptr @.str.m0.6, i64 30 })
  %t10 = xor i1 %t9, 1
  %t11 = call { ptr, i64 } @fn.0()
  %t12 = call i1 @string_eq({ ptr, i64 } %t11, { ptr, i64 } { ptr @.str.m0.7, i64 17 })
  %t13 = xor i1 %t12, 1
  %t14 = or i1 %t10, %t13
  %t15 = zext i1 %t14 to i32
  ret i32 %t15
}

@$make = internal alias { ptr, i64 } (), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
