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
  ret i32 0
}

define internal i32 @fn.1() {
  %t0 = insertvalue [3 x i32] poison, i32 1, 0
  %t1 = insertvalue [3 x i32] %t0, i32 2, 1
  %t2 = insertvalue [3 x i32] %t1, i32 3, 2
  %local.1 = alloca [3 x i32]
  store [3 x i32] %t2, ptr %local.1
  %t3 = getelementptr inbounds [3 x i32], ptr %local.1, i64 0, i64 0
  %t4 = insertvalue { ptr, i64 } poison, ptr %t3, 0
  %t5 = insertvalue { ptr, i64 } %t4, i64 3, 1
  %t6 = call i64 @string_builder_mark()
  %t7 = alloca { ptr, i64 }
  %t8 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 4 }, ptr %t8
  call void @to_string$string(ptr %t7, ptr %t8)
  call void @string_builder_append_string(ptr %t7)
  %t9 = alloca { ptr, i64 }
  call void @to_string$usize(ptr %t9, i64 4)
  call void @string_builder_append_string(ptr %t9)
  %t10 = alloca { ptr, i64 }
  %t11 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 9 }, ptr %t11
  call void @to_string$string(ptr %t10, ptr %t11)
  call void @string_builder_append_string(ptr %t10)
  %t12 = alloca { ptr, i64 }
  call void @to_string$usize(ptr %t12, i64 4)
  call void @string_builder_append_string(ptr %t12)
  %t13 = alloca { ptr, i64 }
  %t14 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 7 }, ptr %t14
  call void @to_string$string(ptr %t13, ptr %t14)
  call void @string_builder_append_string(ptr %t13)
  %t15 = alloca { ptr, i64 }
  call void @to_string$usize(ptr %t15, i64 12)
  call void @string_builder_append_string(ptr %t15)
  %t16 = alloca { ptr, i64 }
  %t17 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 7 }, ptr %t17
  call void @to_string$string(ptr %t16, ptr %t17)
  call void @string_builder_append_string(ptr %t16)
  %t18 = alloca { ptr, i64 }
  call void @to_string$usize(ptr %t18, i64 16)
  call void @string_builder_append_string(ptr %t18)
  %t19 = alloca { ptr, i64 }
  %t20 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.5, i64 8 }, ptr %t20
  call void @to_string$string(ptr %t19, ptr %t20)
  call void @string_builder_append_string(ptr %t19)
  %t21 = alloca { ptr, i64 }
  call void @to_string$usize(ptr %t21, i64 16)
  call void @string_builder_append_string(ptr %t21)
  %t22 = alloca { ptr, i64 }
  %t23 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.6, i64 5 }, ptr %t23
  call void @to_string$string(ptr %t22, ptr %t23)
  call void @string_builder_append_string(ptr %t22)
  %t24 = alloca { ptr, i64 }
  call void @to_string$usize(ptr %t24, i64 8)
  call void @string_builder_append_string(ptr %t24)
  %t25 = alloca { ptr, i64 }
  %t26 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.7, i64 5 }, ptr %t26
  call void @to_string$string(ptr %t25, ptr %t26)
  call void @string_builder_append_string(ptr %t25)
  %t27 = alloca { ptr, i64 }
  call void @to_string$usize(ptr %t27, i64 0)
  call void @string_builder_append_string(ptr %t27)
  %t28 = alloca { ptr, i64 }
  %t29 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.8, i64 4 }, ptr %t29
  call void @to_string$string(ptr %t28, ptr %t29)
  call void @string_builder_append_string(ptr %t28)
  %t30 = alloca { ptr, i64 }
  call void @to_string$usize(ptr %t30, i64 8)
  call void @string_builder_append_string(ptr %t30)
  %t31 = alloca { ptr, i64 }
  %t32 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.9, i64 6 }, ptr %t32
  call void @to_string$string(ptr %t31, ptr %t32)
  call void @string_builder_append_string(ptr %t31)
  %t33 = alloca { ptr, i64 }
  call void @to_string$usize(ptr %t33, i64 0)
  call void @string_builder_append_string(ptr %t33)
  %t34 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t34, i64 %t6)
  %t35 = load { ptr, i64 }, ptr %t34
  call void @$prn({ ptr, i64 } %t35)
  %t36 = add i64 4, 4
  %t37 = add i64 %t36, 12
  %t38 = add i64 %t37, 16
  %t39 = add i64 %t38, 16
  %t40 = add i64 %t39, 8
  %t41 = add i64 %t40, 0
  %t42 = add i64 %t41, 8
  %t43 = add i64 %t42, 0
  %t44 = trunc i64 %t43 to i32
  ret i32 %t44
}

@$helper = internal alias i32 (), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
