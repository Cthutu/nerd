ffi "c" strlen (^i8) -> usize
ffi "c" snprintf (^void, usize, ^i8, ...) -> i32

Pair :: plex {
    left i32
    right i64
}

Choice :: enum {
    None
    Pair(Pair)
    Text(string)
}

consume :: fn(_value: string) {
}

score :: fn(choice: Choice) -> i32 {
    return on choice {
        None => 0
        Pair(pair) => pair.left + pair.right.as(i32)
        Text(text) => text.count.as(i32)
    }
}

use_dyn :: fn() -> i32 {
    values: [..]Pair
    values.push(Pair { left: 3, right: 4 })
    values.reserve_to(4)
    first := values[0]
    values.free()
    return first.left + first.right.as(i32)
}

main :: fn() -> i32 {
    bytes: [4]u8 = ['a', 'b', 'c', 'd']
    ptr := bytes[..].data
    view := ptr.as([]u8, 2)
    ctext := c"hello"
    length := strlen(ctext)
    buffer: [16]u8
    written := snprintf(buffer[..].data, buffer[..].count, c"%d", 7)
    consume($"abi {length} {written}")
    same := "same" == "same"
    choice := Choice.Pair(Pair { left: view[0].as(i32), right: 8 })
    return score(choice) + use_dyn() + same.as(i32)
}

¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [42 x i8] c"tests/llvm/029-abi-layout-runtime.input.n\00"
@.str.m0.0 = private unnamed_addr constant [2 x i8] c"c\00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c"c\00"
@.str.m0.2 = private unnamed_addr constant [6 x i8] c"hello\00"
@.str.m0.3 = private unnamed_addr constant [3 x i8] c"%d\00"
@.str.m0.4 = private unnamed_addr constant [5 x i8] c"abi \00"
@.str.m0.5 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.6 = private unnamed_addr constant [5 x i8] c"same\00"
@.str.m0.7 = private unnamed_addr constant [5 x i8] c"same\00"
@.slice.const.m0.41 = private unnamed_addr constant [4 x i8] [i8 97, i8 98, i8 99, i8 100]

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
declare ptr @nrt_mem_alloc(i64, i64, ptr, i32)
declare ptr @nrt_mem_realloc(ptr, i64, i64, ptr, i32)
declare void @nrt_mem_free(ptr)
declare i64 @nrt_mem_size(ptr)

declare i64 @strlen(ptr)

declare i32 @snprintf(ptr, i64, ptr, ...)

define internal void @fn.2({ ptr, i64 } %_value) {
  ret void
}

define internal i32 @fn.3({ i64, i128 } %choice) {
  %t9 = alloca i128
  %t19 = alloca i128
  %t0 = insertvalue { i64, i128 } poison, i64 0, 0
  %t1 = insertvalue { i64, i128 } %t0, i128 0, 1
  %t2 = extractvalue { i64, i128 } %choice, 0
  %t3 = extractvalue { i64, i128 } %t1, 0
  %t4 = icmp eq i64 %t2, %t3
  br i1 %t4, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t5 = extractvalue { i64, i128 } %choice, 0
  %t6 = icmp eq i64 %t5, 1
  %t7 = extractvalue { i64, i128 } %choice, 1
  store i128 %t7, ptr %t9
  %t8 = load { i32, i64 }, ptr %t9
  %t10 = and i1 %t6, 1
  br i1 %t10, label %on.body.4, label %on.next.5
on.body.4:
  %t11 = extractvalue { i32, i64 } %t8, 0
  %t12 = extractvalue { i32, i64 } %t8, 1
  %t13 = trunc i64 %t12 to i32
  %t14 = add i32 %t11, %t13
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  %t15 = extractvalue { i64, i128 } %choice, 0
  %t16 = icmp eq i64 %t15, 2
  %t17 = extractvalue { i64, i128 } %choice, 1
  store i128 %t17, ptr %t19
  %t18 = load { ptr, i64 }, ptr %t19
  %t20 = and i1 %t16, 1
  br i1 %t20, label %on.body.7, label %on.next.8
on.body.7:
  %t21 = extractvalue { ptr, i64 } %t18, 1
  %t22 = trunc i64 %t21 to i32
  br label %on.value.9
on.value.9:
  br label %on.end.0
on.next.8:
  unreachable
on.end.0:
  %t23 = phi i32 [0, %on.value.3], [%t14, %on.value.6], [%t22, %on.value.9]
  ret i32 %t23
}

define internal i32 @fn.4() {
  %local.4 = alloca ptr
  store ptr null, ptr %local.4
  %t0 = load ptr, ptr %local.4
  %t1 = icmp eq ptr %t0, null
  br i1 %t1, label %dynarray.alloc.0, label %dynarray.ready.1
dynarray.alloc.0:
  %t2 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 28)
  %t3 = getelementptr inbounds { ptr, i64, i64 }, ptr %t2, i64 0, i32 0
  %t4 = getelementptr inbounds { ptr, i64, i64 }, ptr %t2, i64 0, i32 1
  %t5 = getelementptr inbounds { ptr, i64, i64 }, ptr %t2, i64 0, i32 2
  %t6 = getelementptr inbounds i8, ptr %t2, i64 24
  store ptr %t6, ptr %t3
  store i64 0, ptr %t4
  store i64 0, ptr %t5
  store ptr %t6, ptr %local.4
  br label %dynarray.ready.1
dynarray.ready.1:
  %t7 = load ptr, ptr %local.4
  %t8 = getelementptr inbounds i8, ptr %t7, i64 -24
  %t9 = insertvalue { i32, i64 } poison, i32 3, 0
  %t10 = insertvalue { i32, i64 } %t9, i64 4, 1
  %t11 = getelementptr inbounds { ptr, i64, i64 }, ptr %t8, i64 0, i32 0
  %t12 = getelementptr inbounds { ptr, i64, i64 }, ptr %t8, i64 0, i32 1
  %t13 = getelementptr inbounds { ptr, i64, i64 }, ptr %t8, i64 0, i32 2
  %t14 = load ptr, ptr %t11
  %t15 = load i64, ptr %t12
  %t16 = load i64, ptr %t13
  %t17 = add i64 %t15, 1
  %t18 = icmp ugt i64 %t17, %t16
  br i1 %t18, label %dynarray.grow.2, label %dynarray.store.3
dynarray.grow.2:
  %t19 = icmp eq i64 %t16, 0
  %t20 = mul i64 %t16, 2
  %t21 = select i1 %t19, i64 1, i64 %t20
  %t22 = mul i64 %t21, 16
  %t23 = add i64 24, %t22
  %t24 = call ptr @nrt_mem_realloc(ptr %t8, i64 %t23, i64 16, ptr @.macro.file.m0, i32 28)
  %t25 = getelementptr inbounds i8, ptr %t24, i64 24
  %t26 = getelementptr inbounds { ptr, i64, i64 }, ptr %t24, i64 0, i32 0
  %t27 = getelementptr inbounds { ptr, i64, i64 }, ptr %t24, i64 0, i32 2
  store ptr %t25, ptr %t26
  store i64 %t21, ptr %t27
  store ptr %t25, ptr %local.4
  br label %dynarray.store.3
dynarray.store.3:
  %t28 = load ptr, ptr %local.4
  %t29 = getelementptr inbounds i8, ptr %t28, i64 -24
  %t30 = getelementptr inbounds { ptr, i64, i64 }, ptr %t29, i64 0, i32 0
  %t31 = getelementptr inbounds { ptr, i64, i64 }, ptr %t29, i64 0, i32 1
  %t32 = load ptr, ptr %t30
  %t33 = getelementptr inbounds { i32, i64 }, ptr %t32, i64 %t15
  store { i32, i64 } %t10, ptr %t33
  store i64 %t17, ptr %t31
  %t34 = load ptr, ptr %local.4
  %t35 = icmp eq ptr %t34, null
  br i1 %t35, label %dynarray.alloc.4, label %dynarray.ready.5
dynarray.alloc.4:
  %t36 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 29)
  %t37 = getelementptr inbounds { ptr, i64, i64 }, ptr %t36, i64 0, i32 0
  %t38 = getelementptr inbounds { ptr, i64, i64 }, ptr %t36, i64 0, i32 1
  %t39 = getelementptr inbounds { ptr, i64, i64 }, ptr %t36, i64 0, i32 2
  %t40 = getelementptr inbounds i8, ptr %t36, i64 24
  store ptr %t40, ptr %t37
  store i64 0, ptr %t38
  store i64 0, ptr %t39
  store ptr %t40, ptr %local.4
  br label %dynarray.ready.5
dynarray.ready.5:
  %t41 = load ptr, ptr %local.4
  %t42 = getelementptr inbounds i8, ptr %t41, i64 -24
  %t43 = getelementptr inbounds { ptr, i64, i64 }, ptr %t42, i64 0, i32 0
  %t44 = getelementptr inbounds { ptr, i64, i64 }, ptr %t42, i64 0, i32 2
  %t45 = load i64, ptr %t44
  %t46 = icmp ugt i64 4, %t45
  br i1 %t46, label %dynarray.reserve.grow.6, label %dynarray.reserve.done.7
dynarray.reserve.grow.6:
  %t47 = mul i64 4, 16
  %t48 = add i64 24, %t47
  %t49 = call ptr @nrt_mem_realloc(ptr %t42, i64 %t48, i64 16, ptr @.macro.file.m0, i32 29)
  %t50 = getelementptr inbounds i8, ptr %t49, i64 24
  %t51 = getelementptr inbounds { ptr, i64, i64 }, ptr %t49, i64 0, i32 0
  %t52 = getelementptr inbounds { ptr, i64, i64 }, ptr %t49, i64 0, i32 2
  store ptr %t50, ptr %t51
  store i64 4, ptr %t52
  store ptr %t50, ptr %local.4
  br label %dynarray.reserve.done.7
dynarray.reserve.done.7:
  %t53 = load ptr, ptr %local.4
  %t54 = getelementptr inbounds i8, ptr %t53, i64 -24
  %t55 = getelementptr inbounds { ptr, i64, i64 }, ptr %t54, i64 0, i32 0
  %t56 = load ptr, ptr %t55
  %t57 = getelementptr inbounds { i32, i64 }, ptr %t56, i32 0
  %t58 = load { i32, i64 }, ptr %t57
  %t59 = load ptr, ptr %local.4
  %t60 = icmp eq ptr %t59, null
  br i1 %t60, label %dynarray.free.done.9, label %dynarray.free.8
dynarray.free.8:
  %t61 = getelementptr inbounds i8, ptr %t59, i64 -24
  call void @nrt_mem_free(ptr %t61)
  store ptr null, ptr %local.4
  br label %dynarray.free.done.9
dynarray.free.done.9:
  %t62 = extractvalue { i32, i64 } %t58, 0
  %t63 = extractvalue { i32, i64 } %t58, 1
  %t64 = trunc i64 %t63 to i32
  %t65 = add i32 %t62, %t64
  ret i32 %t65
}

define internal i32 @fn.5() {
  %local.6 = alloca [4 x i8]
  %local.11 = alloca [16 x i8]
  %t22 = alloca { ptr, i64 }
  %t25 = alloca { ptr, i64 }
  %t30 = alloca { ptr, i64 }
  %t31 = alloca { ptr, i64 }
  %t39 = alloca i128
  %local.14 = alloca { i64, i128 }
  %t0 = insertvalue [4 x i8] poison, i8 97, 0
  %t1 = insertvalue [4 x i8] %t0, i8 98, 1
  %t2 = insertvalue [4 x i8] %t1, i8 99, 2
  %t3 = insertvalue [4 x i8] %t2, i8 100, 3
  store [4 x i8] %t3, ptr %local.6
  %t4 = getelementptr inbounds [4 x i8], ptr %local.6, i64 0, i64 0
  %t5 = insertvalue { ptr, i64 } poison, ptr %t4, 0
  %t6 = insertvalue { ptr, i64 } %t5, i64 4, 1
  %t7 = extractvalue { ptr, i64 } %t6, 0
  %t8 = insertvalue { ptr, i64 } poison, ptr %t7, 0
  %t9 = insertvalue { ptr, i64 } %t8, i64 2, 1
  %t10 = call i64 @strlen(ptr @.str.m0.2)
  store [16 x i8] zeroinitializer, ptr %local.11
  %t11 = getelementptr inbounds [16 x i8], ptr %local.11, i64 0, i64 0
  %t12 = insertvalue { ptr, i64 } poison, ptr %t11, 0
  %t13 = insertvalue { ptr, i64 } %t12, i64 16, 1
  %t14 = extractvalue { ptr, i64 } %t13, 0
  %t15 = getelementptr inbounds [16 x i8], ptr %local.11, i64 0, i64 0
  %t16 = insertvalue { ptr, i64 } poison, ptr %t15, 0
  %t17 = insertvalue { ptr, i64 } %t16, i64 16, 1
  %t18 = extractvalue { ptr, i64 } %t17, 1
  %t19 = call i32 @snprintf(ptr %t14, i64 %t18, ptr @.str.m0.3, i32 7)
  %t20 = call i64 @string_builder_mark()
  %t21 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 4 }, ptr %t22
  call void @to_string$string(ptr %t21, ptr %t22)
  call void @string_builder_append_string(ptr %t21)
  %t23 = alloca { ptr, i64 }
  call void @to_string$usize(ptr %t23, i64 %t10)
  call void @string_builder_append_string(ptr %t23)
  %t24 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.5, i64 1 }, ptr %t25
  call void @to_string$string(ptr %t24, ptr %t25)
  call void @string_builder_append_string(ptr %t24)
  %t26 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t26, i32 %t19)
  call void @string_builder_append_string(ptr %t26)
  %t27 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t27, i64 %t20)
  %t28 = load { ptr, i64 }, ptr %t27
  call void @fn.2({ ptr, i64 } %t28)
  store { ptr, i64 } { ptr @.str.m0.6, i64 4 }, ptr %t30
  store { ptr, i64 } { ptr @.str.m0.7, i64 4 }, ptr %t31
  %t29 = call i1 @string_eq(ptr %t30, ptr %t31)
  %t32 = extractvalue { ptr, i64 } %t9, 0
  %t33 = getelementptr inbounds i8, ptr %t32, i32 0
  %t34 = load i8, ptr %t33
  %t35 = zext i8 %t34 to i32
  %t36 = insertvalue { i32, i64 } poison, i32 %t35, 0
  %t37 = insertvalue { i32, i64 } %t36, i64 8, 1
  store i128 0, ptr %t39
  store { i32, i64 } %t37, ptr %t39
  %t38 = load i128, ptr %t39
  %t40 = insertvalue { i64, i128 } poison, i64 1, 0
  %t41 = insertvalue { i64, i128 } %t40, i128 %t38, 1
  store { i64, i128 } %t41, ptr %local.14
  %t42 = load { i64, i128 }, ptr %local.14
  %t43 = call i32 @fn.3({ i64, i128 } %t42)
  %t44 = call i32 @fn.4()
  %t45 = add i32 %t43, %t44
  %t46 = zext i1 %t29 to i32
  %t47 = add i32 %t45, %t46
  ret i32 %t47
}

@$consume = internal alias void ({ ptr, i64 }), ptr @fn.2
@$score = internal alias i32 ({ i64, i128 }), ptr @fn.3
@$use_dyn = internal alias i32 (), ptr @fn.4
@$main = alias i32 (), ptr @fn.5

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)
