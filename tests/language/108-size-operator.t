use std.io

helper :: fn () -> i32 {
    return 0
}

main :: fn () -> i32 {
    nums: [3]i32 = [1, 2, 3]
    view: []i32 = nums[..]
    text :: "hi"
    ptr: ^i32 = nil
    i32_size := i32.size
    literal_size := 128.size
    array_size := nums.size
    slice_size := view.size
    string_size := text.size
    ptr_size := ptr.size
    nil_size := nil.size
    fn_size := helper.size
    void_size := void.size
    prn($"i32={i32_size} literal={literal_size} array={array_size} slice={slice_size} string={string_size} ptr={ptr_size} nil={nil_size} fn={fn_size} void={void_size}")
    total := i32_size + literal_size + array_size + slice_size + string_size +
        ptr_size + nil_size + fn_size + void_size
    return total.as(i32)
}
¬
68
¬
i32=4 literal=4 array=12 slice=16 string=16 ptr=8 nil=0 fn=8 void=0

¬
hir 0
module module.0(108-size-operator.input)
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
bind helper = fn.0
bind main = fn.1
func fn.0() -> i32 {
  return i32 0
}
func fn.1() -> i32 {
  let nums: [3]i32 = [3]i32 array(i32 1, i32 2, i32 3)
  let view: []i32 = []i32 slice([3]i32 local.1(nums), <none>, <none>)
  let text: string = string "hi"
  let ptr: ^i32 = ^i32 nil
  let i32_size: usize = usize field(i32 i32, size)
  let literal_size: usize = usize field(untyped integer 128, size)
  let array_size: usize = usize field([3]i32 local.1(nums), size)
  let slice_size: usize = usize field([]i32 local.2(view), size)
  let string_size: usize = usize field(string local.0(text), size)
  let ptr_size: usize = usize field(^i32 local.3(ptr), size)
  let nil_size: usize = usize field(nil nil, size)
  let fn_size: usize = usize field(fn () -> i32 bind.5(helper), size)
  let void_size: usize = usize field(void void, size)
  expr void call bind.2(prn)(string interpolate(<unknown> "i32=", usize local.4(i32_size), <unknown> " literal=", usize local.5(literal_size), <unknown> " array=", usize local.6(array_size), <unknown> " slice=", usize local.7(slice_size), <unknown> " string=", usize local.8(string_size), <unknown> " ptr=", usize local.9(ptr_size), <unknown> " nil=", usize local.10(nil_size), <unknown> " fn=", usize local.11(fn_size), <unknown> " void=", usize local.12(void_size)))
  let total: usize = usize add(usize add(usize add(usize add(usize add(usize add(usize add(usize add(usize local.4(i32_size), usize local.5(literal_size)), usize local.6(array_size)), usize local.7(slice_size)), usize local.8(string_size)), usize local.9(ptr_size)), usize local.10(nil_size)), usize local.11(fn_size)), usize local.12(void_size))
  return i32 cast(usize local.13(total) as i32)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [3 x i8] c"hi\00"
@.str.m0.1 = private unnamed_addr constant [5 x i8] c"i32=\00"
@.str.m0.2 = private unnamed_addr constant [10 x i8] c" literal=\00"
@.str.m0.3 = private unnamed_addr constant [8 x i8] c" array=\00"
@.str.m0.4 = private unnamed_addr constant [8 x i8] c" slice=\00"
@.str.m0.5 = private unnamed_addr constant [9 x i8] c" string=\00"
@.str.m0.6 = private unnamed_addr constant [6 x i8] c" ptr=\00"
@.str.m0.7 = private unnamed_addr constant [6 x i8] c" nil=\00"
@.str.m0.8 = private unnamed_addr constant [5 x i8] c" fn=\00"
@.str.m0.9 = private unnamed_addr constant [7 x i8] c" void=\00"

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

define i32 @fn.0() {
  ret i32 0
}

define i32 @fn.1() {
  %t0 = insertvalue [3 x i32] poison, i32 1, 0
  %t1 = insertvalue [3 x i32] %t0, i32 2, 1
  %t2 = insertvalue [3 x i32] %t1, i32 3, 2
  %local.1 = alloca [3 x i32]
  store [3 x i32] %t2, ptr %local.1
  %t3 = getelementptr inbounds [3 x i32], ptr %local.1, i64 0, i64 0
  %t4 = insertvalue { ptr, i64 } poison, ptr %t3, 0
  %t5 = insertvalue { ptr, i64 } %t4, i64 3, 1
  %t6 = call i64 @string_builder_mark()
  %t7 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 4 })
  call void @string_builder_append_string({ ptr, i64 } %t7)
  %t8 = call { ptr, i64 } @to_string$usize(i64 4)
  call void @string_builder_append_string({ ptr, i64 } %t8)
  %t9 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 9 })
  call void @string_builder_append_string({ ptr, i64 } %t9)
  %t10 = call { ptr, i64 } @to_string$usize(i64 4)
  call void @string_builder_append_string({ ptr, i64 } %t10)
  %t11 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.3, i64 7 })
  call void @string_builder_append_string({ ptr, i64 } %t11)
  %t12 = call { ptr, i64 } @to_string$usize(i64 12)
  call void @string_builder_append_string({ ptr, i64 } %t12)
  %t13 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.4, i64 7 })
  call void @string_builder_append_string({ ptr, i64 } %t13)
  %t14 = call { ptr, i64 } @to_string$usize(i64 16)
  call void @string_builder_append_string({ ptr, i64 } %t14)
  %t15 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.5, i64 8 })
  call void @string_builder_append_string({ ptr, i64 } %t15)
  %t16 = call { ptr, i64 } @to_string$usize(i64 16)
  call void @string_builder_append_string({ ptr, i64 } %t16)
  %t17 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.6, i64 5 })
  call void @string_builder_append_string({ ptr, i64 } %t17)
  %t18 = call { ptr, i64 } @to_string$usize(i64 8)
  call void @string_builder_append_string({ ptr, i64 } %t18)
  %t19 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.7, i64 5 })
  call void @string_builder_append_string({ ptr, i64 } %t19)
  %t20 = call { ptr, i64 } @to_string$usize(i64 0)
  call void @string_builder_append_string({ ptr, i64 } %t20)
  %t21 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.8, i64 4 })
  call void @string_builder_append_string({ ptr, i64 } %t21)
  %t22 = call { ptr, i64 } @to_string$usize(i64 8)
  call void @string_builder_append_string({ ptr, i64 } %t22)
  %t23 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.9, i64 6 })
  call void @string_builder_append_string({ ptr, i64 } %t23)
  %t24 = call { ptr, i64 } @to_string$usize(i64 0)
  call void @string_builder_append_string({ ptr, i64 } %t24)
  %t25 = call { ptr, i64 } @string_builder_finish(i64 %t6)
  call void @$prn({ ptr, i64 } %t25)
  %t26 = add i64 4, 4
  %t27 = add i64 %t26, 12
  %t28 = add i64 %t27, 16
  %t29 = add i64 %t28, 16
  %t30 = add i64 %t29, 8
  %t31 = add i64 %t30, 0
  %t32 = add i64 %t31, 8
  %t33 = add i64 %t32, 0
  %t34 = trunc i64 %t33 to i32
  ret i32 %t34
}

@$helper = internal alias i32 (), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
