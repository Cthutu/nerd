use std.io

make_words :: fn () -> [..]string {
    words: [..]string
    words.push("look")
    words.push("north")
    return words
}

main :: fn () -> i32 {
    empty: [..]string
    on empty != nil => return 1
    on empty.count != 0 => return 2
    on empty.capacity != 0 => return 3

    names: [4..]string = ["north", "south"]
    on names.count != 2 => return 4
    on names.capacity < 4 => return 5

    names.push("east")
    on names.count != 3 => return 6

    extra :: ["west", "up"]
    names.append(extra[..])
    on names.count != 5 => return 7

    last := names.pop()
    on last != "up" => return 8
    on names.count != 4 => return 9
    names.push(last)

    view := names[..]
    on view.count != 5 => return 10

    prn($"{view[0]} {view[1]} {view[2]} {view[3]} {view[4]}")

    names.reserve_to(10)
    on names.capacity < 10 => return 11

    words := make_words()
    on words.count != 2 => return 12
    prn($"{words[0]} {words[1]}")
    words.free()

    nums: [..]i32
    nums.push(1)
    nums.push(2)
    nums.push(3)
    nums.push(4)
    nums.push(5)
    nums.delete(1)
    on nums.count != 4 => return 13
    on nums[0] != 1 => return 14
    on nums[1] != 3 => return 15
    on nums[2] != 4 => return 16
    on nums[3] != 5 => return 17
    nums.swap_delete(1)
    on nums.count != 3 => return 18
    on nums[0] != 1 => return 19
    on nums[1] != 5 => return 20
    on nums[2] != 4 => return 21
    nums.free()

    names.clear()
    on names.count != 0 => return 22
    names.free()
    on names != nil => return 23
    return 0
}
¬
0
¬
north south east west up
look north

¬
hir 0
module module.0(099-dynamic-arrays.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind make_words = fn.0
bind main = fn.1
func fn.0() -> [..]string {
  expr <unknown> default
  let words: [..]string = <unknown> default
  expr void call fn (string) -> void field([..]string local.0(words), push)(string "look")
  expr void call fn (string) -> void field([..]string local.0(words), push)(string "north")
  return [..]string local.0(words)
}
func fn.1() -> i32 {
  expr <unknown> default
  let empty: [..]string = <unknown> default
  expr void on bool not_equal([..]string local.2(empty), [..]string nil) {
    value(bool yes) => {
      return i32 1
    }
  }
  expr void on bool not_equal(usize field([..]string local.2(empty), count), usize 0) {
    value(bool yes) => {
      return i32 2
    }
  }
  expr void on bool not_equal(usize field([..]string local.2(empty), capacity), usize 0) {
    value(bool yes) => {
      return i32 3
    }
  }
  let names: [..]string = [..]string array(string "north", string "south"; min_capacity 4)
  expr void on bool not_equal(usize field([..]string local.3(names), count), usize 2) {
    value(bool yes) => {
      return i32 4
    }
  }
  expr void on bool less(usize field([..]string local.3(names), capacity), usize 4) {
    value(bool yes) => {
      return i32 5
    }
  }
  expr void call fn (string) -> void field([..]string local.3(names), push)(string "east")
  expr void on bool not_equal(usize field([..]string local.3(names), count), usize 3) {
    value(bool yes) => {
      return i32 6
    }
  }
  let extra: [2]string = [2]string array(string "west", string "up")
  expr void call fn ([]string[]string) field([..]string local.3(names), append)([]string slice([2]string local.1(extra), <none>, <none>))
  expr void on bool not_equal(usize field([..]string local.3(names), count), usize 5) {
    value(bool yes) => {
      return i32 7
    }
  }
  let last: string = string call fn () -> string field([..]string local.3(names), pop)()
  expr void on bool not_equal(string local.4(last), string "up") {
    value(bool yes) => {
      return i32 8
    }
  }
  expr void on bool not_equal(usize field([..]string local.3(names), count), usize 4) {
    value(bool yes) => {
      return i32 9
    }
  }
  expr void call fn (string) -> void field([..]string local.3(names), push)(string local.4(last))
  let view: []string = []string slice([..]string local.3(names), <none>, <none>)
  expr void on bool not_equal(usize field([]string local.5(view), count), usize 5) {
    value(bool yes) => {
      return i32 10
    }
  }
  expr void call bind.0(prn)(string interpolate(string index([]string local.5(view), untyped integer 0), <unknown> " ", string index([]string local.5(view), untyped integer 1), <unknown> " ", string index([]string local.5(view), untyped integer 2), <unknown> " ", string index([]string local.5(view), untyped integer 3), <unknown> " ", string index([]string local.5(view), untyped integer 4)))
  expr void call fn (usize) -> void field([..]string local.3(names), reserve_to)(usize 10)
  expr void on bool less(usize field([..]string local.3(names), capacity), usize 10) {
    value(bool yes) => {
      return i32 11
    }
  }
  let words: [..]string = [..]string call bind.2(make_words)()
  expr void on bool not_equal(usize field([..]string local.6(words), count), usize 2) {
    value(bool yes) => {
      return i32 12
    }
  }
  expr void call bind.0(prn)(string interpolate(string index([..]string local.6(words), untyped integer 0), <unknown> " ", string index([..]string local.6(words), untyped integer 1)))
  expr void call fn () -> void field([..]string local.6(words), free)()
  expr <unknown> default
  let nums: [..]i32 = <unknown> default
  expr void call fn (i32) -> void field([..]i32 local.7(nums), push)(i32 1)
  expr void call fn (i32) -> void field([..]i32 local.7(nums), push)(i32 2)
  expr void call fn (i32) -> void field([..]i32 local.7(nums), push)(i32 3)
  expr void call fn (i32) -> void field([..]i32 local.7(nums), push)(i32 4)
  expr void call fn (i32) -> void field([..]i32 local.7(nums), push)(i32 5)
  expr void call fn (usize) -> void field([..]i32 local.7(nums), delete)(usize 1)
  expr void on bool not_equal(usize field([..]i32 local.7(nums), count), usize 4) {
    value(bool yes) => {
      return i32 13
    }
  }
  expr void on bool not_equal(i32 index([..]i32 local.7(nums), untyped integer 0), i32 1) {
    value(bool yes) => {
      return i32 14
    }
  }
  expr void on bool not_equal(i32 index([..]i32 local.7(nums), untyped integer 1), i32 3) {
    value(bool yes) => {
      return i32 15
    }
  }
  expr void on bool not_equal(i32 index([..]i32 local.7(nums), untyped integer 2), i32 4) {
    value(bool yes) => {
      return i32 16
    }
  }
  expr void on bool not_equal(i32 index([..]i32 local.7(nums), untyped integer 3), i32 5) {
    value(bool yes) => {
      return i32 17
    }
  }
  expr void call fn (usize) -> void field([..]i32 local.7(nums), swap_delete)(usize 1)
  expr void on bool not_equal(usize field([..]i32 local.7(nums), count), usize 3) {
    value(bool yes) => {
      return i32 18
    }
  }
  expr void on bool not_equal(i32 index([..]i32 local.7(nums), untyped integer 0), i32 1) {
    value(bool yes) => {
      return i32 19
    }
  }
  expr void on bool not_equal(i32 index([..]i32 local.7(nums), untyped integer 1), i32 5) {
    value(bool yes) => {
      return i32 20
    }
  }
  expr void on bool not_equal(i32 index([..]i32 local.7(nums), untyped integer 2), i32 4) {
    value(bool yes) => {
      return i32 21
    }
  }
  expr void call fn () -> void field([..]i32 local.7(nums), free)()
  expr void call fn () -> void field([..]string local.3(names), clear)()
  expr void on bool not_equal(usize field([..]string local.3(names), count), usize 0) {
    value(bool yes) => {
      return i32 22
    }
  }
  expr void call fn () -> void field([..]string local.3(names), free)()
  expr void on bool not_equal([..]string local.3(names), [..]string nil) {
    value(bool yes) => {
      return i32 23
    }
  }
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [58 x i8] c"tests/language/099-dynamic-arrays.t\00"
@.str.m0.0 = private unnamed_addr constant [5 x i8] c"look\00"
@.str.m0.1 = private unnamed_addr constant [6 x i8] c"north\00"
@.str.m0.2 = private unnamed_addr constant [6 x i8] c"north\00"
@.str.m0.3 = private unnamed_addr constant [6 x i8] c"south\00"
@.str.m0.4 = private unnamed_addr constant [5 x i8] c"east\00"
@.str.m0.5 = private unnamed_addr constant [5 x i8] c"west\00"
@.str.m0.6 = private unnamed_addr constant [3 x i8] c"up\00"
@.str.m0.7 = private unnamed_addr constant [3 x i8] c"up\00"
@.str.m0.8 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.9 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.10 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.11 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.12 = private unnamed_addr constant [2 x i8] c" \00"

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

declare void @$prn({ ptr, i64 })
declare ptr @$input({ ptr, i64 })

define internal ptr @fn.0() {
  %local.0 = alloca ptr
  store ptr null, ptr %local.0
  %t0 = load ptr, ptr %local.0
  %t1 = icmp eq ptr %t0, null
  br i1 %t1, label %dynarray.alloc.0, label %dynarray.ready.1
dynarray.alloc.0:
  %t2 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 5)
  %t3 = getelementptr inbounds { ptr, i64, i64 }, ptr %t2, i64 0, i32 0
  %t4 = getelementptr inbounds { ptr, i64, i64 }, ptr %t2, i64 0, i32 1
  %t5 = getelementptr inbounds { ptr, i64, i64 }, ptr %t2, i64 0, i32 2
  %t6 = getelementptr inbounds i8, ptr %t2, i64 24
  store ptr %t6, ptr %t3
  store i64 0, ptr %t4
  store i64 0, ptr %t5
  store ptr %t6, ptr %local.0
  br label %dynarray.ready.1
dynarray.ready.1:
  %t7 = load ptr, ptr %local.0
  %t8 = getelementptr inbounds i8, ptr %t7, i64 -24
  %t9 = getelementptr inbounds { ptr, i64, i64 }, ptr %t8, i64 0, i32 0
  %t10 = getelementptr inbounds { ptr, i64, i64 }, ptr %t8, i64 0, i32 1
  %t11 = getelementptr inbounds { ptr, i64, i64 }, ptr %t8, i64 0, i32 2
  %t12 = load ptr, ptr %t9
  %t13 = load i64, ptr %t10
  %t14 = load i64, ptr %t11
  %t15 = add i64 %t13, 1
  %t16 = icmp ugt i64 %t15, %t14
  br i1 %t16, label %dynarray.grow.2, label %dynarray.store.3
dynarray.grow.2:
  %t17 = icmp eq i64 %t14, 0
  %t18 = mul i64 %t14, 2
  %t19 = select i1 %t17, i64 1, i64 %t18
  %t20 = mul i64 %t19, 16
  %t21 = add i64 24, %t20
  %t22 = call ptr @nrt_mem_realloc(ptr %t8, i64 %t21, i64 16, ptr @.macro.file.m0, i32 5)
  %t23 = getelementptr inbounds i8, ptr %t22, i64 24
  %t24 = getelementptr inbounds { ptr, i64, i64 }, ptr %t22, i64 0, i32 0
  %t25 = getelementptr inbounds { ptr, i64, i64 }, ptr %t22, i64 0, i32 2
  store ptr %t23, ptr %t24
  store i64 %t19, ptr %t25
  store ptr %t23, ptr %local.0
  br label %dynarray.store.3
dynarray.store.3:
  %t26 = load ptr, ptr %local.0
  %t27 = getelementptr inbounds i8, ptr %t26, i64 -24
  %t28 = getelementptr inbounds { ptr, i64, i64 }, ptr %t27, i64 0, i32 0
  %t29 = getelementptr inbounds { ptr, i64, i64 }, ptr %t27, i64 0, i32 1
  %t30 = load ptr, ptr %t28
  %t31 = getelementptr inbounds { ptr, i64 }, ptr %t30, i64 %t13
  store { ptr, i64 } { ptr @.str.m0.0, i64 4 }, ptr %t31
  store i64 %t15, ptr %t29
  %t32 = load ptr, ptr %local.0
  %t33 = icmp eq ptr %t32, null
  br i1 %t33, label %dynarray.alloc.4, label %dynarray.ready.5
dynarray.alloc.4:
  %t34 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 6)
  %t35 = getelementptr inbounds { ptr, i64, i64 }, ptr %t34, i64 0, i32 0
  %t36 = getelementptr inbounds { ptr, i64, i64 }, ptr %t34, i64 0, i32 1
  %t37 = getelementptr inbounds { ptr, i64, i64 }, ptr %t34, i64 0, i32 2
  %t38 = getelementptr inbounds i8, ptr %t34, i64 24
  store ptr %t38, ptr %t35
  store i64 0, ptr %t36
  store i64 0, ptr %t37
  store ptr %t38, ptr %local.0
  br label %dynarray.ready.5
dynarray.ready.5:
  %t39 = load ptr, ptr %local.0
  %t40 = getelementptr inbounds i8, ptr %t39, i64 -24
  %t41 = getelementptr inbounds { ptr, i64, i64 }, ptr %t40, i64 0, i32 0
  %t42 = getelementptr inbounds { ptr, i64, i64 }, ptr %t40, i64 0, i32 1
  %t43 = getelementptr inbounds { ptr, i64, i64 }, ptr %t40, i64 0, i32 2
  %t44 = load ptr, ptr %t41
  %t45 = load i64, ptr %t42
  %t46 = load i64, ptr %t43
  %t47 = add i64 %t45, 1
  %t48 = icmp ugt i64 %t47, %t46
  br i1 %t48, label %dynarray.grow.6, label %dynarray.store.7
dynarray.grow.6:
  %t49 = icmp eq i64 %t46, 0
  %t50 = mul i64 %t46, 2
  %t51 = select i1 %t49, i64 1, i64 %t50
  %t52 = mul i64 %t51, 16
  %t53 = add i64 24, %t52
  %t54 = call ptr @nrt_mem_realloc(ptr %t40, i64 %t53, i64 16, ptr @.macro.file.m0, i32 6)
  %t55 = getelementptr inbounds i8, ptr %t54, i64 24
  %t56 = getelementptr inbounds { ptr, i64, i64 }, ptr %t54, i64 0, i32 0
  %t57 = getelementptr inbounds { ptr, i64, i64 }, ptr %t54, i64 0, i32 2
  store ptr %t55, ptr %t56
  store i64 %t51, ptr %t57
  store ptr %t55, ptr %local.0
  br label %dynarray.store.7
dynarray.store.7:
  %t58 = load ptr, ptr %local.0
  %t59 = getelementptr inbounds i8, ptr %t58, i64 -24
  %t60 = getelementptr inbounds { ptr, i64, i64 }, ptr %t59, i64 0, i32 0
  %t61 = getelementptr inbounds { ptr, i64, i64 }, ptr %t59, i64 0, i32 1
  %t62 = load ptr, ptr %t60
  %t63 = getelementptr inbounds { ptr, i64 }, ptr %t62, i64 %t45
  store { ptr, i64 } { ptr @.str.m0.1, i64 5 }, ptr %t63
  store i64 %t47, ptr %t61
  %t64 = load ptr, ptr %local.0
  ret ptr %t64
}

define internal i32 @fn.1() {
  %local.2 = alloca ptr
  store ptr null, ptr %local.2
  %t0 = load ptr, ptr %local.2
  %t1 = icmp ne ptr %t0, null
  %t2 = icmp eq i1 %t1, 1
  br i1 %t2, label %on.body.1, label %on.end.0
on.body.1:
  ret i32 1
on.end.0:
  %t3 = load ptr, ptr %local.2
  %t4 = alloca i64
  %t5 = icmp eq ptr %t3, null
  br i1 %t5, label %dynarray.field.empty.2, label %dynarray.field.load.3
dynarray.field.empty.2:
  store i64 0, ptr %t4
  br label %dynarray.field.done.4
dynarray.field.load.3:
  %t6 = getelementptr inbounds i8, ptr %t3, i64 -24
  %t7 = getelementptr inbounds { ptr, i64, i64 }, ptr %t6, i64 0, i32 1
  %t8 = load i64, ptr %t7
  store i64 %t8, ptr %t4
  br label %dynarray.field.done.4
dynarray.field.done.4:
  %t9 = load i64, ptr %t4
  %t10 = icmp ne i64 %t9, 0
  %t11 = icmp eq i1 %t10, 1
  br i1 %t11, label %on.body.6, label %on.end.5
on.body.6:
  ret i32 2
on.end.5:
  %t12 = load ptr, ptr %local.2
  %t13 = alloca i64
  %t14 = icmp eq ptr %t12, null
  br i1 %t14, label %dynarray.field.empty.7, label %dynarray.field.load.8
dynarray.field.empty.7:
  store i64 0, ptr %t13
  br label %dynarray.field.done.9
dynarray.field.load.8:
  %t15 = getelementptr inbounds i8, ptr %t12, i64 -24
  %t16 = getelementptr inbounds { ptr, i64, i64 }, ptr %t15, i64 0, i32 2
  %t17 = load i64, ptr %t16
  store i64 %t17, ptr %t13
  br label %dynarray.field.done.9
dynarray.field.done.9:
  %t18 = load i64, ptr %t13
  %t19 = icmp ne i64 %t18, 0
  %t20 = icmp eq i1 %t19, 1
  br i1 %t20, label %on.body.11, label %on.end.10
on.body.11:
  ret i32 3
on.end.10:
  %t21 = call ptr @nrt_mem_alloc(i64 88, i64 16, ptr @.macro.file.m0, i32 0)
  %t22 = getelementptr inbounds { ptr, i64, i64 }, ptr %t21, i64 0, i32 0
  %t23 = getelementptr inbounds { ptr, i64, i64 }, ptr %t21, i64 0, i32 1
  %t24 = getelementptr inbounds { ptr, i64, i64 }, ptr %t21, i64 0, i32 2
  %t25 = getelementptr inbounds i8, ptr %t21, i64 24
  %t26 = getelementptr inbounds { ptr, i64 }, ptr %t25, i64 0
  store { ptr, i64 } { ptr @.str.m0.2, i64 5 }, ptr %t26
  %t27 = getelementptr inbounds { ptr, i64 }, ptr %t25, i64 1
  store { ptr, i64 } { ptr @.str.m0.3, i64 5 }, ptr %t27
  store ptr %t25, ptr %t22
  store i64 2, ptr %t23
  store i64 4, ptr %t24
  %local.3 = alloca ptr
  store ptr %t25, ptr %local.3
  %t28 = load ptr, ptr %local.3
  %t29 = alloca i64
  %t30 = icmp eq ptr %t28, null
  br i1 %t30, label %dynarray.field.empty.12, label %dynarray.field.load.13
dynarray.field.empty.12:
  store i64 0, ptr %t29
  br label %dynarray.field.done.14
dynarray.field.load.13:
  %t31 = getelementptr inbounds i8, ptr %t28, i64 -24
  %t32 = getelementptr inbounds { ptr, i64, i64 }, ptr %t31, i64 0, i32 1
  %t33 = load i64, ptr %t32
  store i64 %t33, ptr %t29
  br label %dynarray.field.done.14
dynarray.field.done.14:
  %t34 = load i64, ptr %t29
  %t35 = icmp ne i64 %t34, 2
  %t36 = icmp eq i1 %t35, 1
  br i1 %t36, label %on.body.16, label %on.end.15
on.body.16:
  ret i32 4
on.end.15:
  %t37 = load ptr, ptr %local.3
  %t38 = alloca i64
  %t39 = icmp eq ptr %t37, null
  br i1 %t39, label %dynarray.field.empty.17, label %dynarray.field.load.18
dynarray.field.empty.17:
  store i64 0, ptr %t38
  br label %dynarray.field.done.19
dynarray.field.load.18:
  %t40 = getelementptr inbounds i8, ptr %t37, i64 -24
  %t41 = getelementptr inbounds { ptr, i64, i64 }, ptr %t40, i64 0, i32 2
  %t42 = load i64, ptr %t41
  store i64 %t42, ptr %t38
  br label %dynarray.field.done.19
dynarray.field.done.19:
  %t43 = load i64, ptr %t38
  %t44 = icmp slt i64 %t43, 4
  %t45 = icmp eq i1 %t44, 1
  br i1 %t45, label %on.body.21, label %on.end.20
on.body.21:
  ret i32 5
on.end.20:
  %t46 = load ptr, ptr %local.3
  %t47 = icmp eq ptr %t46, null
  br i1 %t47, label %dynarray.alloc.22, label %dynarray.ready.23
dynarray.alloc.22:
  %t48 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 20)
  %t49 = getelementptr inbounds { ptr, i64, i64 }, ptr %t48, i64 0, i32 0
  %t50 = getelementptr inbounds { ptr, i64, i64 }, ptr %t48, i64 0, i32 1
  %t51 = getelementptr inbounds { ptr, i64, i64 }, ptr %t48, i64 0, i32 2
  %t52 = getelementptr inbounds i8, ptr %t48, i64 24
  store ptr %t52, ptr %t49
  store i64 0, ptr %t50
  store i64 0, ptr %t51
  store ptr %t52, ptr %local.3
  br label %dynarray.ready.23
dynarray.ready.23:
  %t53 = load ptr, ptr %local.3
  %t54 = getelementptr inbounds i8, ptr %t53, i64 -24
  %t55 = getelementptr inbounds { ptr, i64, i64 }, ptr %t54, i64 0, i32 0
  %t56 = getelementptr inbounds { ptr, i64, i64 }, ptr %t54, i64 0, i32 1
  %t57 = getelementptr inbounds { ptr, i64, i64 }, ptr %t54, i64 0, i32 2
  %t58 = load ptr, ptr %t55
  %t59 = load i64, ptr %t56
  %t60 = load i64, ptr %t57
  %t61 = add i64 %t59, 1
  %t62 = icmp ugt i64 %t61, %t60
  br i1 %t62, label %dynarray.grow.24, label %dynarray.store.25
dynarray.grow.24:
  %t63 = icmp eq i64 %t60, 0
  %t64 = mul i64 %t60, 2
  %t65 = select i1 %t63, i64 1, i64 %t64
  %t66 = mul i64 %t65, 16
  %t67 = add i64 24, %t66
  %t68 = call ptr @nrt_mem_realloc(ptr %t54, i64 %t67, i64 16, ptr @.macro.file.m0, i32 20)
  %t69 = getelementptr inbounds i8, ptr %t68, i64 24
  %t70 = getelementptr inbounds { ptr, i64, i64 }, ptr %t68, i64 0, i32 0
  %t71 = getelementptr inbounds { ptr, i64, i64 }, ptr %t68, i64 0, i32 2
  store ptr %t69, ptr %t70
  store i64 %t65, ptr %t71
  store ptr %t69, ptr %local.3
  br label %dynarray.store.25
dynarray.store.25:
  %t72 = load ptr, ptr %local.3
  %t73 = getelementptr inbounds i8, ptr %t72, i64 -24
  %t74 = getelementptr inbounds { ptr, i64, i64 }, ptr %t73, i64 0, i32 0
  %t75 = getelementptr inbounds { ptr, i64, i64 }, ptr %t73, i64 0, i32 1
  %t76 = load ptr, ptr %t74
  %t77 = getelementptr inbounds { ptr, i64 }, ptr %t76, i64 %t59
  store { ptr, i64 } { ptr @.str.m0.4, i64 4 }, ptr %t77
  store i64 %t61, ptr %t75
  %t78 = load ptr, ptr %local.3
  %t79 = alloca i64
  %t80 = icmp eq ptr %t78, null
  br i1 %t80, label %dynarray.field.empty.26, label %dynarray.field.load.27
dynarray.field.empty.26:
  store i64 0, ptr %t79
  br label %dynarray.field.done.28
dynarray.field.load.27:
  %t81 = getelementptr inbounds i8, ptr %t78, i64 -24
  %t82 = getelementptr inbounds { ptr, i64, i64 }, ptr %t81, i64 0, i32 1
  %t83 = load i64, ptr %t82
  store i64 %t83, ptr %t79
  br label %dynarray.field.done.28
dynarray.field.done.28:
  %t84 = load i64, ptr %t79
  %t85 = icmp ne i64 %t84, 3
  %t86 = icmp eq i1 %t85, 1
  br i1 %t86, label %on.body.30, label %on.end.29
on.body.30:
  ret i32 6
on.end.29:
  %t87 = insertvalue [2 x { ptr, i64 }] poison, { ptr, i64 } { ptr @.str.m0.5, i64 4 }, 0
  %t88 = insertvalue [2 x { ptr, i64 }] %t87, { ptr, i64 } { ptr @.str.m0.6, i64 2 }, 1
  %local.1 = alloca [2 x { ptr, i64 }]
  store [2 x { ptr, i64 }] %t88, ptr %local.1
  %t89 = getelementptr inbounds [2 x { ptr, i64 }], ptr %local.1, i64 0, i64 0
  %t90 = insertvalue { ptr, i64 } poison, ptr %t89, 0
  %t91 = insertvalue { ptr, i64 } %t90, i64 2, 1
  %t92 = extractvalue { ptr, i64 } %t91, 0
  %t93 = extractvalue { ptr, i64 } %t91, 1
  %t94 = load ptr, ptr %local.3
  %t95 = icmp eq ptr %t94, null
  br i1 %t95, label %dynarray.alloc.31, label %dynarray.ready.32
dynarray.alloc.31:
  %t96 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 24)
  %t97 = getelementptr inbounds { ptr, i64, i64 }, ptr %t96, i64 0, i32 0
  %t98 = getelementptr inbounds { ptr, i64, i64 }, ptr %t96, i64 0, i32 1
  %t99 = getelementptr inbounds { ptr, i64, i64 }, ptr %t96, i64 0, i32 2
  %t100 = getelementptr inbounds i8, ptr %t96, i64 24
  store ptr %t100, ptr %t97
  store i64 0, ptr %t98
  store i64 0, ptr %t99
  store ptr %t100, ptr %local.3
  br label %dynarray.ready.32
dynarray.ready.32:
  %t101 = load ptr, ptr %local.3
  %t102 = getelementptr inbounds i8, ptr %t101, i64 -24
  %t103 = getelementptr inbounds { ptr, i64, i64 }, ptr %t102, i64 0, i32 0
  %t104 = getelementptr inbounds { ptr, i64, i64 }, ptr %t102, i64 0, i32 1
  %t105 = getelementptr inbounds { ptr, i64, i64 }, ptr %t102, i64 0, i32 2
  %t106 = load ptr, ptr %t103
  %t107 = load i64, ptr %t104
  %t108 = load i64, ptr %t105
  %t109 = add i64 %t107, %t93
  %t110 = icmp ugt i64 %t109, %t108
  br i1 %t110, label %dynarray.append.grow.33, label %dynarray.append.copy.34
dynarray.append.grow.33:
  %t111 = mul i64 %t109, 16
  %t112 = add i64 24, %t111
  %t113 = call ptr @nrt_mem_realloc(ptr %t102, i64 %t112, i64 16, ptr @.macro.file.m0, i32 24)
  %t114 = getelementptr inbounds i8, ptr %t113, i64 24
  %t115 = getelementptr inbounds { ptr, i64, i64 }, ptr %t113, i64 0, i32 0
  %t116 = getelementptr inbounds { ptr, i64, i64 }, ptr %t113, i64 0, i32 2
  store ptr %t114, ptr %t115
  store i64 %t109, ptr %t116
  store ptr %t114, ptr %local.3
  br label %dynarray.append.copy.34
dynarray.append.copy.34:
  %t117 = load ptr, ptr %local.3
  %t118 = getelementptr inbounds i8, ptr %t117, i64 -24
  %t119 = getelementptr inbounds { ptr, i64, i64 }, ptr %t118, i64 0, i32 0
  %t120 = getelementptr inbounds { ptr, i64, i64 }, ptr %t118, i64 0, i32 1
  %t121 = load ptr, ptr %t119
  %t122 = alloca i64
  store i64 0, ptr %t122
  br label %dynarray.append.loop.35
dynarray.append.loop.35:
  %t123 = load i64, ptr %t122
  %t124 = icmp ult i64 %t123, %t93
  br i1 %t124, label %dynarray.append.body.36, label %dynarray.append.done.37
dynarray.append.body.36:
  %t125 = add i64 %t107, %t123
  %t126 = getelementptr inbounds { ptr, i64 }, ptr %t92, i64 %t123
  %t127 = getelementptr inbounds { ptr, i64 }, ptr %t121, i64 %t125
  %t128 = load { ptr, i64 }, ptr %t126
  store { ptr, i64 } %t128, ptr %t127
  %t129 = add i64 %t123, 1
  store i64 %t129, ptr %t122
  br label %dynarray.append.loop.35
dynarray.append.done.37:
  store i64 %t109, ptr %t120
  %t130 = load ptr, ptr %local.3
  %t131 = alloca i64
  %t132 = icmp eq ptr %t130, null
  br i1 %t132, label %dynarray.field.empty.38, label %dynarray.field.load.39
dynarray.field.empty.38:
  store i64 0, ptr %t131
  br label %dynarray.field.done.40
dynarray.field.load.39:
  %t133 = getelementptr inbounds i8, ptr %t130, i64 -24
  %t134 = getelementptr inbounds { ptr, i64, i64 }, ptr %t133, i64 0, i32 1
  %t135 = load i64, ptr %t134
  store i64 %t135, ptr %t131
  br label %dynarray.field.done.40
dynarray.field.done.40:
  %t136 = load i64, ptr %t131
  %t137 = icmp ne i64 %t136, 5
  %t138 = icmp eq i1 %t137, 1
  br i1 %t138, label %on.body.42, label %on.end.41
on.body.42:
  ret i32 7
on.end.41:
  %t139 = load ptr, ptr %local.3
  %t140 = getelementptr inbounds i8, ptr %t139, i64 -24
  %t141 = getelementptr inbounds { ptr, i64, i64 }, ptr %t140, i64 0, i32 1
  %t142 = load i64, ptr %t141
  %t143 = sub i64 %t142, 1
  %t144 = getelementptr inbounds { ptr, i64 }, ptr %t139, i64 %t143
  %t145 = load { ptr, i64 }, ptr %t144
  store i64 %t143, ptr %t141
  %t147 = alloca { ptr, i64 }
  store { ptr, i64 } %t145, ptr %t147
  %t148 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.7, i64 2 }, ptr %t148
  %t146 = call i1 @string_eq(ptr %t147, ptr %t148)
  %t149 = xor i1 %t146, 1
  %t150 = icmp eq i1 %t149, 1
  br i1 %t150, label %on.body.44, label %on.end.43
on.body.44:
  ret i32 8
on.end.43:
  %t151 = load ptr, ptr %local.3
  %t152 = alloca i64
  %t153 = icmp eq ptr %t151, null
  br i1 %t153, label %dynarray.field.empty.45, label %dynarray.field.load.46
dynarray.field.empty.45:
  store i64 0, ptr %t152
  br label %dynarray.field.done.47
dynarray.field.load.46:
  %t154 = getelementptr inbounds i8, ptr %t151, i64 -24
  %t155 = getelementptr inbounds { ptr, i64, i64 }, ptr %t154, i64 0, i32 1
  %t156 = load i64, ptr %t155
  store i64 %t156, ptr %t152
  br label %dynarray.field.done.47
dynarray.field.done.47:
  %t157 = load i64, ptr %t152
  %t158 = icmp ne i64 %t157, 4
  %t159 = icmp eq i1 %t158, 1
  br i1 %t159, label %on.body.49, label %on.end.48
on.body.49:
  ret i32 9
on.end.48:
  %t160 = load ptr, ptr %local.3
  %t161 = icmp eq ptr %t160, null
  br i1 %t161, label %dynarray.alloc.50, label %dynarray.ready.51
dynarray.alloc.50:
  %t162 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 30)
  %t163 = getelementptr inbounds { ptr, i64, i64 }, ptr %t162, i64 0, i32 0
  %t164 = getelementptr inbounds { ptr, i64, i64 }, ptr %t162, i64 0, i32 1
  %t165 = getelementptr inbounds { ptr, i64, i64 }, ptr %t162, i64 0, i32 2
  %t166 = getelementptr inbounds i8, ptr %t162, i64 24
  store ptr %t166, ptr %t163
  store i64 0, ptr %t164
  store i64 0, ptr %t165
  store ptr %t166, ptr %local.3
  br label %dynarray.ready.51
dynarray.ready.51:
  %t167 = load ptr, ptr %local.3
  %t168 = getelementptr inbounds i8, ptr %t167, i64 -24
  %t169 = getelementptr inbounds { ptr, i64, i64 }, ptr %t168, i64 0, i32 0
  %t170 = getelementptr inbounds { ptr, i64, i64 }, ptr %t168, i64 0, i32 1
  %t171 = getelementptr inbounds { ptr, i64, i64 }, ptr %t168, i64 0, i32 2
  %t172 = load ptr, ptr %t169
  %t173 = load i64, ptr %t170
  %t174 = load i64, ptr %t171
  %t175 = add i64 %t173, 1
  %t176 = icmp ugt i64 %t175, %t174
  br i1 %t176, label %dynarray.grow.52, label %dynarray.store.53
dynarray.grow.52:
  %t177 = icmp eq i64 %t174, 0
  %t178 = mul i64 %t174, 2
  %t179 = select i1 %t177, i64 1, i64 %t178
  %t180 = mul i64 %t179, 16
  %t181 = add i64 24, %t180
  %t182 = call ptr @nrt_mem_realloc(ptr %t168, i64 %t181, i64 16, ptr @.macro.file.m0, i32 30)
  %t183 = getelementptr inbounds i8, ptr %t182, i64 24
  %t184 = getelementptr inbounds { ptr, i64, i64 }, ptr %t182, i64 0, i32 0
  %t185 = getelementptr inbounds { ptr, i64, i64 }, ptr %t182, i64 0, i32 2
  store ptr %t183, ptr %t184
  store i64 %t179, ptr %t185
  store ptr %t183, ptr %local.3
  br label %dynarray.store.53
dynarray.store.53:
  %t186 = load ptr, ptr %local.3
  %t187 = getelementptr inbounds i8, ptr %t186, i64 -24
  %t188 = getelementptr inbounds { ptr, i64, i64 }, ptr %t187, i64 0, i32 0
  %t189 = getelementptr inbounds { ptr, i64, i64 }, ptr %t187, i64 0, i32 1
  %t190 = load ptr, ptr %t188
  %t191 = getelementptr inbounds { ptr, i64 }, ptr %t190, i64 %t173
  store { ptr, i64 } %t145, ptr %t191
  store i64 %t175, ptr %t189
  %t192 = load ptr, ptr %local.3
  %t195 = alloca ptr
  %t196 = alloca i64
  %t197 = icmp eq ptr %t192, null
  br i1 %t197, label %dynarray.slice.empty.54, label %dynarray.slice.load.55
dynarray.slice.empty.54:
  store ptr null, ptr %t195
  store i64 0, ptr %t196
  br label %dynarray.slice.ready.56
dynarray.slice.load.55:
  %t198 = getelementptr inbounds i8, ptr %t192, i64 -24
  %t199 = getelementptr inbounds { ptr, i64, i64 }, ptr %t198, i64 0, i32 0
  %t200 = load ptr, ptr %t199
  %t201 = getelementptr inbounds i8, ptr %t192, i64 -24
  %t202 = getelementptr inbounds { ptr, i64, i64 }, ptr %t201, i64 0, i32 1
  %t203 = load i64, ptr %t202
  store ptr %t200, ptr %t195
  store i64 %t203, ptr %t196
  br label %dynarray.slice.ready.56
dynarray.slice.ready.56:
  %t204 = load i64, ptr %t196
  %t205 = load ptr, ptr %t195
  %t206 = getelementptr inbounds { ptr, i64 }, ptr %t205, i64 0
  %t207 = insertvalue { ptr, i64 } poison, ptr %t206, 0
  %t208 = insertvalue { ptr, i64 } %t207, i64 %t204, 1
  %t209 = extractvalue { ptr, i64 } %t208, 1
  %t210 = icmp ne i64 %t209, 5
  %t211 = icmp eq i1 %t210, 1
  br i1 %t211, label %on.body.58, label %on.end.57
on.body.58:
  ret i32 10
on.end.57:
  %t212 = call i64 @string_builder_mark()
  %t213 = extractvalue { ptr, i64 } %t208, 0
  %t214 = getelementptr inbounds { ptr, i64 }, ptr %t213, i32 0
  %t215 = load { ptr, i64 }, ptr %t214
  %t216 = alloca { ptr, i64 }
  %t217 = alloca { ptr, i64 }
  store { ptr, i64 } %t215, ptr %t217
  call void @to_string$string(ptr %t216, ptr %t217)
  call void @string_builder_append_string(ptr %t216)
  %t218 = alloca { ptr, i64 }
  %t219 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.8, i64 1 }, ptr %t219
  call void @to_string$string(ptr %t218, ptr %t219)
  call void @string_builder_append_string(ptr %t218)
  %t220 = extractvalue { ptr, i64 } %t208, 0
  %t221 = getelementptr inbounds { ptr, i64 }, ptr %t220, i32 1
  %t222 = load { ptr, i64 }, ptr %t221
  %t223 = alloca { ptr, i64 }
  %t224 = alloca { ptr, i64 }
  store { ptr, i64 } %t222, ptr %t224
  call void @to_string$string(ptr %t223, ptr %t224)
  call void @string_builder_append_string(ptr %t223)
  %t225 = alloca { ptr, i64 }
  %t226 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.9, i64 1 }, ptr %t226
  call void @to_string$string(ptr %t225, ptr %t226)
  call void @string_builder_append_string(ptr %t225)
  %t227 = extractvalue { ptr, i64 } %t208, 0
  %t228 = getelementptr inbounds { ptr, i64 }, ptr %t227, i32 2
  %t229 = load { ptr, i64 }, ptr %t228
  %t230 = alloca { ptr, i64 }
  %t231 = alloca { ptr, i64 }
  store { ptr, i64 } %t229, ptr %t231
  call void @to_string$string(ptr %t230, ptr %t231)
  call void @string_builder_append_string(ptr %t230)
  %t232 = alloca { ptr, i64 }
  %t233 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.10, i64 1 }, ptr %t233
  call void @to_string$string(ptr %t232, ptr %t233)
  call void @string_builder_append_string(ptr %t232)
  %t234 = extractvalue { ptr, i64 } %t208, 0
  %t235 = getelementptr inbounds { ptr, i64 }, ptr %t234, i32 3
  %t236 = load { ptr, i64 }, ptr %t235
  %t237 = alloca { ptr, i64 }
  %t238 = alloca { ptr, i64 }
  store { ptr, i64 } %t236, ptr %t238
  call void @to_string$string(ptr %t237, ptr %t238)
  call void @string_builder_append_string(ptr %t237)
  %t239 = alloca { ptr, i64 }
  %t240 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.11, i64 1 }, ptr %t240
  call void @to_string$string(ptr %t239, ptr %t240)
  call void @string_builder_append_string(ptr %t239)
  %t241 = extractvalue { ptr, i64 } %t208, 0
  %t242 = getelementptr inbounds { ptr, i64 }, ptr %t241, i32 4
  %t243 = load { ptr, i64 }, ptr %t242
  %t244 = alloca { ptr, i64 }
  %t245 = alloca { ptr, i64 }
  store { ptr, i64 } %t243, ptr %t245
  call void @to_string$string(ptr %t244, ptr %t245)
  call void @string_builder_append_string(ptr %t244)
  %t246 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t246, i64 %t212)
  %t247 = load { ptr, i64 }, ptr %t246
  call void @$prn({ ptr, i64 } %t247)
  %t248 = load ptr, ptr %local.3
  %t249 = icmp eq ptr %t248, null
  br i1 %t249, label %dynarray.alloc.59, label %dynarray.ready.60
dynarray.alloc.59:
  %t250 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 37)
  %t251 = getelementptr inbounds { ptr, i64, i64 }, ptr %t250, i64 0, i32 0
  %t252 = getelementptr inbounds { ptr, i64, i64 }, ptr %t250, i64 0, i32 1
  %t253 = getelementptr inbounds { ptr, i64, i64 }, ptr %t250, i64 0, i32 2
  %t254 = getelementptr inbounds i8, ptr %t250, i64 24
  store ptr %t254, ptr %t251
  store i64 0, ptr %t252
  store i64 0, ptr %t253
  store ptr %t254, ptr %local.3
  br label %dynarray.ready.60
dynarray.ready.60:
  %t255 = load ptr, ptr %local.3
  %t256 = getelementptr inbounds i8, ptr %t255, i64 -24
  %t257 = getelementptr inbounds { ptr, i64, i64 }, ptr %t256, i64 0, i32 0
  %t258 = getelementptr inbounds { ptr, i64, i64 }, ptr %t256, i64 0, i32 2
  %t259 = load i64, ptr %t258
  %t260 = icmp ugt i64 10, %t259
  br i1 %t260, label %dynarray.reserve.grow.61, label %dynarray.reserve.done.62
dynarray.reserve.grow.61:
  %t261 = mul i64 10, 16
  %t262 = add i64 24, %t261
  %t263 = call ptr @nrt_mem_realloc(ptr %t256, i64 %t262, i64 16, ptr @.macro.file.m0, i32 37)
  %t264 = getelementptr inbounds i8, ptr %t263, i64 24
  %t265 = getelementptr inbounds { ptr, i64, i64 }, ptr %t263, i64 0, i32 0
  %t266 = getelementptr inbounds { ptr, i64, i64 }, ptr %t263, i64 0, i32 2
  store ptr %t264, ptr %t265
  store i64 10, ptr %t266
  store ptr %t264, ptr %local.3
  br label %dynarray.reserve.done.62
dynarray.reserve.done.62:
  %t267 = load ptr, ptr %local.3
  %t268 = alloca i64
  %t269 = icmp eq ptr %t267, null
  br i1 %t269, label %dynarray.field.empty.63, label %dynarray.field.load.64
dynarray.field.empty.63:
  store i64 0, ptr %t268
  br label %dynarray.field.done.65
dynarray.field.load.64:
  %t270 = getelementptr inbounds i8, ptr %t267, i64 -24
  %t271 = getelementptr inbounds { ptr, i64, i64 }, ptr %t270, i64 0, i32 2
  %t272 = load i64, ptr %t271
  store i64 %t272, ptr %t268
  br label %dynarray.field.done.65
dynarray.field.done.65:
  %t273 = load i64, ptr %t268
  %t274 = icmp slt i64 %t273, 10
  %t275 = icmp eq i1 %t274, 1
  br i1 %t275, label %on.body.67, label %on.end.66
on.body.67:
  ret i32 11
on.end.66:
  %t276 = call ptr @fn.0()
  %local.6 = alloca ptr
  store ptr %t276, ptr %local.6
  %t277 = load ptr, ptr %local.6
  %t278 = alloca i64
  %t279 = icmp eq ptr %t277, null
  br i1 %t279, label %dynarray.field.empty.68, label %dynarray.field.load.69
dynarray.field.empty.68:
  store i64 0, ptr %t278
  br label %dynarray.field.done.70
dynarray.field.load.69:
  %t280 = getelementptr inbounds i8, ptr %t277, i64 -24
  %t281 = getelementptr inbounds { ptr, i64, i64 }, ptr %t280, i64 0, i32 1
  %t282 = load i64, ptr %t281
  store i64 %t282, ptr %t278
  br label %dynarray.field.done.70
dynarray.field.done.70:
  %t283 = load i64, ptr %t278
  %t284 = icmp ne i64 %t283, 2
  %t285 = icmp eq i1 %t284, 1
  br i1 %t285, label %on.body.72, label %on.end.71
on.body.72:
  ret i32 12
on.end.71:
  %t286 = call i64 @string_builder_mark()
  %t287 = load ptr, ptr %local.6
  %t288 = getelementptr inbounds i8, ptr %t287, i64 -24
  %t289 = getelementptr inbounds { ptr, i64, i64 }, ptr %t288, i64 0, i32 0
  %t290 = load ptr, ptr %t289
  %t291 = getelementptr inbounds { ptr, i64 }, ptr %t290, i32 0
  %t292 = load { ptr, i64 }, ptr %t291
  %t293 = alloca { ptr, i64 }
  %t294 = alloca { ptr, i64 }
  store { ptr, i64 } %t292, ptr %t294
  call void @to_string$string(ptr %t293, ptr %t294)
  call void @string_builder_append_string(ptr %t293)
  %t295 = alloca { ptr, i64 }
  %t296 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.12, i64 1 }, ptr %t296
  call void @to_string$string(ptr %t295, ptr %t296)
  call void @string_builder_append_string(ptr %t295)
  %t297 = load ptr, ptr %local.6
  %t298 = getelementptr inbounds i8, ptr %t297, i64 -24
  %t299 = getelementptr inbounds { ptr, i64, i64 }, ptr %t298, i64 0, i32 0
  %t300 = load ptr, ptr %t299
  %t301 = getelementptr inbounds { ptr, i64 }, ptr %t300, i32 1
  %t302 = load { ptr, i64 }, ptr %t301
  %t303 = alloca { ptr, i64 }
  %t304 = alloca { ptr, i64 }
  store { ptr, i64 } %t302, ptr %t304
  call void @to_string$string(ptr %t303, ptr %t304)
  call void @string_builder_append_string(ptr %t303)
  %t305 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t305, i64 %t286)
  %t306 = load { ptr, i64 }, ptr %t305
  call void @$prn({ ptr, i64 } %t306)
  %t307 = load ptr, ptr %local.6
  %t308 = icmp eq ptr %t307, null
  br i1 %t308, label %dynarray.free.done.74, label %dynarray.free.73
dynarray.free.73:
  %t309 = getelementptr inbounds i8, ptr %t307, i64 -24
  call void @nrt_mem_free(ptr %t309)
  store ptr null, ptr %local.6
  br label %dynarray.free.done.74
dynarray.free.done.74:
  %local.7 = alloca ptr
  store ptr null, ptr %local.7
  %t310 = load ptr, ptr %local.7
  %t311 = icmp eq ptr %t310, null
  br i1 %t311, label %dynarray.alloc.75, label %dynarray.ready.76
dynarray.alloc.75:
  %t312 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 46)
  %t313 = getelementptr inbounds { ptr, i64, i64 }, ptr %t312, i64 0, i32 0
  %t314 = getelementptr inbounds { ptr, i64, i64 }, ptr %t312, i64 0, i32 1
  %t315 = getelementptr inbounds { ptr, i64, i64 }, ptr %t312, i64 0, i32 2
  %t316 = getelementptr inbounds i8, ptr %t312, i64 24
  store ptr %t316, ptr %t313
  store i64 0, ptr %t314
  store i64 0, ptr %t315
  store ptr %t316, ptr %local.7
  br label %dynarray.ready.76
dynarray.ready.76:
  %t317 = load ptr, ptr %local.7
  %t318 = getelementptr inbounds i8, ptr %t317, i64 -24
  %t319 = getelementptr inbounds { ptr, i64, i64 }, ptr %t318, i64 0, i32 0
  %t320 = getelementptr inbounds { ptr, i64, i64 }, ptr %t318, i64 0, i32 1
  %t321 = getelementptr inbounds { ptr, i64, i64 }, ptr %t318, i64 0, i32 2
  %t322 = load ptr, ptr %t319
  %t323 = load i64, ptr %t320
  %t324 = load i64, ptr %t321
  %t325 = add i64 %t323, 1
  %t326 = icmp ugt i64 %t325, %t324
  br i1 %t326, label %dynarray.grow.77, label %dynarray.store.78
dynarray.grow.77:
  %t327 = icmp eq i64 %t324, 0
  %t328 = mul i64 %t324, 2
  %t329 = select i1 %t327, i64 1, i64 %t328
  %t330 = mul i64 %t329, 4
  %t331 = add i64 24, %t330
  %t332 = call ptr @nrt_mem_realloc(ptr %t318, i64 %t331, i64 16, ptr @.macro.file.m0, i32 46)
  %t333 = getelementptr inbounds i8, ptr %t332, i64 24
  %t334 = getelementptr inbounds { ptr, i64, i64 }, ptr %t332, i64 0, i32 0
  %t335 = getelementptr inbounds { ptr, i64, i64 }, ptr %t332, i64 0, i32 2
  store ptr %t333, ptr %t334
  store i64 %t329, ptr %t335
  store ptr %t333, ptr %local.7
  br label %dynarray.store.78
dynarray.store.78:
  %t336 = load ptr, ptr %local.7
  %t337 = getelementptr inbounds i8, ptr %t336, i64 -24
  %t338 = getelementptr inbounds { ptr, i64, i64 }, ptr %t337, i64 0, i32 0
  %t339 = getelementptr inbounds { ptr, i64, i64 }, ptr %t337, i64 0, i32 1
  %t340 = load ptr, ptr %t338
  %t341 = getelementptr inbounds i32, ptr %t340, i64 %t323
  store i32 1, ptr %t341
  store i64 %t325, ptr %t339
  %t342 = load ptr, ptr %local.7
  %t343 = icmp eq ptr %t342, null
  br i1 %t343, label %dynarray.alloc.79, label %dynarray.ready.80
dynarray.alloc.79:
  %t344 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 47)
  %t345 = getelementptr inbounds { ptr, i64, i64 }, ptr %t344, i64 0, i32 0
  %t346 = getelementptr inbounds { ptr, i64, i64 }, ptr %t344, i64 0, i32 1
  %t347 = getelementptr inbounds { ptr, i64, i64 }, ptr %t344, i64 0, i32 2
  %t348 = getelementptr inbounds i8, ptr %t344, i64 24
  store ptr %t348, ptr %t345
  store i64 0, ptr %t346
  store i64 0, ptr %t347
  store ptr %t348, ptr %local.7
  br label %dynarray.ready.80
dynarray.ready.80:
  %t349 = load ptr, ptr %local.7
  %t350 = getelementptr inbounds i8, ptr %t349, i64 -24
  %t351 = getelementptr inbounds { ptr, i64, i64 }, ptr %t350, i64 0, i32 0
  %t352 = getelementptr inbounds { ptr, i64, i64 }, ptr %t350, i64 0, i32 1
  %t353 = getelementptr inbounds { ptr, i64, i64 }, ptr %t350, i64 0, i32 2
  %t354 = load ptr, ptr %t351
  %t355 = load i64, ptr %t352
  %t356 = load i64, ptr %t353
  %t357 = add i64 %t355, 1
  %t358 = icmp ugt i64 %t357, %t356
  br i1 %t358, label %dynarray.grow.81, label %dynarray.store.82
dynarray.grow.81:
  %t359 = icmp eq i64 %t356, 0
  %t360 = mul i64 %t356, 2
  %t361 = select i1 %t359, i64 1, i64 %t360
  %t362 = mul i64 %t361, 4
  %t363 = add i64 24, %t362
  %t364 = call ptr @nrt_mem_realloc(ptr %t350, i64 %t363, i64 16, ptr @.macro.file.m0, i32 47)
  %t365 = getelementptr inbounds i8, ptr %t364, i64 24
  %t366 = getelementptr inbounds { ptr, i64, i64 }, ptr %t364, i64 0, i32 0
  %t367 = getelementptr inbounds { ptr, i64, i64 }, ptr %t364, i64 0, i32 2
  store ptr %t365, ptr %t366
  store i64 %t361, ptr %t367
  store ptr %t365, ptr %local.7
  br label %dynarray.store.82
dynarray.store.82:
  %t368 = load ptr, ptr %local.7
  %t369 = getelementptr inbounds i8, ptr %t368, i64 -24
  %t370 = getelementptr inbounds { ptr, i64, i64 }, ptr %t369, i64 0, i32 0
  %t371 = getelementptr inbounds { ptr, i64, i64 }, ptr %t369, i64 0, i32 1
  %t372 = load ptr, ptr %t370
  %t373 = getelementptr inbounds i32, ptr %t372, i64 %t355
  store i32 2, ptr %t373
  store i64 %t357, ptr %t371
  %t374 = load ptr, ptr %local.7
  %t375 = icmp eq ptr %t374, null
  br i1 %t375, label %dynarray.alloc.83, label %dynarray.ready.84
dynarray.alloc.83:
  %t376 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 48)
  %t377 = getelementptr inbounds { ptr, i64, i64 }, ptr %t376, i64 0, i32 0
  %t378 = getelementptr inbounds { ptr, i64, i64 }, ptr %t376, i64 0, i32 1
  %t379 = getelementptr inbounds { ptr, i64, i64 }, ptr %t376, i64 0, i32 2
  %t380 = getelementptr inbounds i8, ptr %t376, i64 24
  store ptr %t380, ptr %t377
  store i64 0, ptr %t378
  store i64 0, ptr %t379
  store ptr %t380, ptr %local.7
  br label %dynarray.ready.84
dynarray.ready.84:
  %t381 = load ptr, ptr %local.7
  %t382 = getelementptr inbounds i8, ptr %t381, i64 -24
  %t383 = getelementptr inbounds { ptr, i64, i64 }, ptr %t382, i64 0, i32 0
  %t384 = getelementptr inbounds { ptr, i64, i64 }, ptr %t382, i64 0, i32 1
  %t385 = getelementptr inbounds { ptr, i64, i64 }, ptr %t382, i64 0, i32 2
  %t386 = load ptr, ptr %t383
  %t387 = load i64, ptr %t384
  %t388 = load i64, ptr %t385
  %t389 = add i64 %t387, 1
  %t390 = icmp ugt i64 %t389, %t388
  br i1 %t390, label %dynarray.grow.85, label %dynarray.store.86
dynarray.grow.85:
  %t391 = icmp eq i64 %t388, 0
  %t392 = mul i64 %t388, 2
  %t393 = select i1 %t391, i64 1, i64 %t392
  %t394 = mul i64 %t393, 4
  %t395 = add i64 24, %t394
  %t396 = call ptr @nrt_mem_realloc(ptr %t382, i64 %t395, i64 16, ptr @.macro.file.m0, i32 48)
  %t397 = getelementptr inbounds i8, ptr %t396, i64 24
  %t398 = getelementptr inbounds { ptr, i64, i64 }, ptr %t396, i64 0, i32 0
  %t399 = getelementptr inbounds { ptr, i64, i64 }, ptr %t396, i64 0, i32 2
  store ptr %t397, ptr %t398
  store i64 %t393, ptr %t399
  store ptr %t397, ptr %local.7
  br label %dynarray.store.86
dynarray.store.86:
  %t400 = load ptr, ptr %local.7
  %t401 = getelementptr inbounds i8, ptr %t400, i64 -24
  %t402 = getelementptr inbounds { ptr, i64, i64 }, ptr %t401, i64 0, i32 0
  %t403 = getelementptr inbounds { ptr, i64, i64 }, ptr %t401, i64 0, i32 1
  %t404 = load ptr, ptr %t402
  %t405 = getelementptr inbounds i32, ptr %t404, i64 %t387
  store i32 3, ptr %t405
  store i64 %t389, ptr %t403
  %t406 = load ptr, ptr %local.7
  %t407 = icmp eq ptr %t406, null
  br i1 %t407, label %dynarray.alloc.87, label %dynarray.ready.88
dynarray.alloc.87:
  %t408 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 49)
  %t409 = getelementptr inbounds { ptr, i64, i64 }, ptr %t408, i64 0, i32 0
  %t410 = getelementptr inbounds { ptr, i64, i64 }, ptr %t408, i64 0, i32 1
  %t411 = getelementptr inbounds { ptr, i64, i64 }, ptr %t408, i64 0, i32 2
  %t412 = getelementptr inbounds i8, ptr %t408, i64 24
  store ptr %t412, ptr %t409
  store i64 0, ptr %t410
  store i64 0, ptr %t411
  store ptr %t412, ptr %local.7
  br label %dynarray.ready.88
dynarray.ready.88:
  %t413 = load ptr, ptr %local.7
  %t414 = getelementptr inbounds i8, ptr %t413, i64 -24
  %t415 = getelementptr inbounds { ptr, i64, i64 }, ptr %t414, i64 0, i32 0
  %t416 = getelementptr inbounds { ptr, i64, i64 }, ptr %t414, i64 0, i32 1
  %t417 = getelementptr inbounds { ptr, i64, i64 }, ptr %t414, i64 0, i32 2
  %t418 = load ptr, ptr %t415
  %t419 = load i64, ptr %t416
  %t420 = load i64, ptr %t417
  %t421 = add i64 %t419, 1
  %t422 = icmp ugt i64 %t421, %t420
  br i1 %t422, label %dynarray.grow.89, label %dynarray.store.90
dynarray.grow.89:
  %t423 = icmp eq i64 %t420, 0
  %t424 = mul i64 %t420, 2
  %t425 = select i1 %t423, i64 1, i64 %t424
  %t426 = mul i64 %t425, 4
  %t427 = add i64 24, %t426
  %t428 = call ptr @nrt_mem_realloc(ptr %t414, i64 %t427, i64 16, ptr @.macro.file.m0, i32 49)
  %t429 = getelementptr inbounds i8, ptr %t428, i64 24
  %t430 = getelementptr inbounds { ptr, i64, i64 }, ptr %t428, i64 0, i32 0
  %t431 = getelementptr inbounds { ptr, i64, i64 }, ptr %t428, i64 0, i32 2
  store ptr %t429, ptr %t430
  store i64 %t425, ptr %t431
  store ptr %t429, ptr %local.7
  br label %dynarray.store.90
dynarray.store.90:
  %t432 = load ptr, ptr %local.7
  %t433 = getelementptr inbounds i8, ptr %t432, i64 -24
  %t434 = getelementptr inbounds { ptr, i64, i64 }, ptr %t433, i64 0, i32 0
  %t435 = getelementptr inbounds { ptr, i64, i64 }, ptr %t433, i64 0, i32 1
  %t436 = load ptr, ptr %t434
  %t437 = getelementptr inbounds i32, ptr %t436, i64 %t419
  store i32 4, ptr %t437
  store i64 %t421, ptr %t435
  %t438 = load ptr, ptr %local.7
  %t439 = icmp eq ptr %t438, null
  br i1 %t439, label %dynarray.alloc.91, label %dynarray.ready.92
dynarray.alloc.91:
  %t440 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 50)
  %t441 = getelementptr inbounds { ptr, i64, i64 }, ptr %t440, i64 0, i32 0
  %t442 = getelementptr inbounds { ptr, i64, i64 }, ptr %t440, i64 0, i32 1
  %t443 = getelementptr inbounds { ptr, i64, i64 }, ptr %t440, i64 0, i32 2
  %t444 = getelementptr inbounds i8, ptr %t440, i64 24
  store ptr %t444, ptr %t441
  store i64 0, ptr %t442
  store i64 0, ptr %t443
  store ptr %t444, ptr %local.7
  br label %dynarray.ready.92
dynarray.ready.92:
  %t445 = load ptr, ptr %local.7
  %t446 = getelementptr inbounds i8, ptr %t445, i64 -24
  %t447 = getelementptr inbounds { ptr, i64, i64 }, ptr %t446, i64 0, i32 0
  %t448 = getelementptr inbounds { ptr, i64, i64 }, ptr %t446, i64 0, i32 1
  %t449 = getelementptr inbounds { ptr, i64, i64 }, ptr %t446, i64 0, i32 2
  %t450 = load ptr, ptr %t447
  %t451 = load i64, ptr %t448
  %t452 = load i64, ptr %t449
  %t453 = add i64 %t451, 1
  %t454 = icmp ugt i64 %t453, %t452
  br i1 %t454, label %dynarray.grow.93, label %dynarray.store.94
dynarray.grow.93:
  %t455 = icmp eq i64 %t452, 0
  %t456 = mul i64 %t452, 2
  %t457 = select i1 %t455, i64 1, i64 %t456
  %t458 = mul i64 %t457, 4
  %t459 = add i64 24, %t458
  %t460 = call ptr @nrt_mem_realloc(ptr %t446, i64 %t459, i64 16, ptr @.macro.file.m0, i32 50)
  %t461 = getelementptr inbounds i8, ptr %t460, i64 24
  %t462 = getelementptr inbounds { ptr, i64, i64 }, ptr %t460, i64 0, i32 0
  %t463 = getelementptr inbounds { ptr, i64, i64 }, ptr %t460, i64 0, i32 2
  store ptr %t461, ptr %t462
  store i64 %t457, ptr %t463
  store ptr %t461, ptr %local.7
  br label %dynarray.store.94
dynarray.store.94:
  %t464 = load ptr, ptr %local.7
  %t465 = getelementptr inbounds i8, ptr %t464, i64 -24
  %t466 = getelementptr inbounds { ptr, i64, i64 }, ptr %t465, i64 0, i32 0
  %t467 = getelementptr inbounds { ptr, i64, i64 }, ptr %t465, i64 0, i32 1
  %t468 = load ptr, ptr %t466
  %t469 = getelementptr inbounds i32, ptr %t468, i64 %t451
  store i32 5, ptr %t469
  store i64 %t453, ptr %t467
  %t470 = load ptr, ptr %local.7
  %t471 = getelementptr inbounds i8, ptr %t470, i64 -24
  %t472 = getelementptr inbounds { ptr, i64, i64 }, ptr %t471, i64 0, i32 1
  %t473 = load i64, ptr %t472
  %t474 = sub i64 %t473, 1
  %t475 = alloca i64
  store i64 1, ptr %t475
  br label %dynarray.delete.loop.95
dynarray.delete.loop.95:
  %t476 = load i64, ptr %t475
  %t477 = icmp ult i64 %t476, %t474
  br i1 %t477, label %dynarray.delete.body.96, label %dynarray.delete.done.97
dynarray.delete.body.96:
  %t478 = add i64 %t476, 1
  %t479 = getelementptr inbounds i32, ptr %t470, i64 %t478
  %t480 = getelementptr inbounds i32, ptr %t470, i64 %t476
  %t481 = load i32, ptr %t479
  store i32 %t481, ptr %t480
  store i64 %t478, ptr %t475
  br label %dynarray.delete.loop.95
dynarray.delete.done.97:
  store i64 %t474, ptr %t472
  %t482 = load ptr, ptr %local.7
  %t483 = alloca i64
  %t484 = icmp eq ptr %t482, null
  br i1 %t484, label %dynarray.field.empty.98, label %dynarray.field.load.99
dynarray.field.empty.98:
  store i64 0, ptr %t483
  br label %dynarray.field.done.100
dynarray.field.load.99:
  %t485 = getelementptr inbounds i8, ptr %t482, i64 -24
  %t486 = getelementptr inbounds { ptr, i64, i64 }, ptr %t485, i64 0, i32 1
  %t487 = load i64, ptr %t486
  store i64 %t487, ptr %t483
  br label %dynarray.field.done.100
dynarray.field.done.100:
  %t488 = load i64, ptr %t483
  %t489 = icmp ne i64 %t488, 4
  %t490 = icmp eq i1 %t489, 1
  br i1 %t490, label %on.body.102, label %on.end.101
on.body.102:
  ret i32 13
on.end.101:
  %t491 = load ptr, ptr %local.7
  %t492 = getelementptr inbounds i8, ptr %t491, i64 -24
  %t493 = getelementptr inbounds { ptr, i64, i64 }, ptr %t492, i64 0, i32 0
  %t494 = load ptr, ptr %t493
  %t495 = getelementptr inbounds i32, ptr %t494, i32 0
  %t496 = load i32, ptr %t495
  %t497 = icmp ne i32 %t496, 1
  %t498 = icmp eq i1 %t497, 1
  br i1 %t498, label %on.body.104, label %on.end.103
on.body.104:
  ret i32 14
on.end.103:
  %t499 = load ptr, ptr %local.7
  %t500 = getelementptr inbounds i8, ptr %t499, i64 -24
  %t501 = getelementptr inbounds { ptr, i64, i64 }, ptr %t500, i64 0, i32 0
  %t502 = load ptr, ptr %t501
  %t503 = getelementptr inbounds i32, ptr %t502, i32 1
  %t504 = load i32, ptr %t503
  %t505 = icmp ne i32 %t504, 3
  %t506 = icmp eq i1 %t505, 1
  br i1 %t506, label %on.body.106, label %on.end.105
on.body.106:
  ret i32 15
on.end.105:
  %t507 = load ptr, ptr %local.7
  %t508 = getelementptr inbounds i8, ptr %t507, i64 -24
  %t509 = getelementptr inbounds { ptr, i64, i64 }, ptr %t508, i64 0, i32 0
  %t510 = load ptr, ptr %t509
  %t511 = getelementptr inbounds i32, ptr %t510, i32 2
  %t512 = load i32, ptr %t511
  %t513 = icmp ne i32 %t512, 4
  %t514 = icmp eq i1 %t513, 1
  br i1 %t514, label %on.body.108, label %on.end.107
on.body.108:
  ret i32 16
on.end.107:
  %t515 = load ptr, ptr %local.7
  %t516 = getelementptr inbounds i8, ptr %t515, i64 -24
  %t517 = getelementptr inbounds { ptr, i64, i64 }, ptr %t516, i64 0, i32 0
  %t518 = load ptr, ptr %t517
  %t519 = getelementptr inbounds i32, ptr %t518, i32 3
  %t520 = load i32, ptr %t519
  %t521 = icmp ne i32 %t520, 5
  %t522 = icmp eq i1 %t521, 1
  br i1 %t522, label %on.body.110, label %on.end.109
on.body.110:
  ret i32 17
on.end.109:
  %t523 = load ptr, ptr %local.7
  %t524 = getelementptr inbounds i8, ptr %t523, i64 -24
  %t525 = getelementptr inbounds { ptr, i64, i64 }, ptr %t524, i64 0, i32 1
  %t526 = load i64, ptr %t525
  %t527 = sub i64 %t526, 1
  %t528 = getelementptr inbounds i32, ptr %t523, i64 %t527
  %t529 = getelementptr inbounds i32, ptr %t523, i64 1
  %t530 = load i32, ptr %t528
  store i32 %t530, ptr %t529
  store i64 %t527, ptr %t525
  %t531 = load ptr, ptr %local.7
  %t532 = alloca i64
  %t533 = icmp eq ptr %t531, null
  br i1 %t533, label %dynarray.field.empty.111, label %dynarray.field.load.112
dynarray.field.empty.111:
  store i64 0, ptr %t532
  br label %dynarray.field.done.113
dynarray.field.load.112:
  %t534 = getelementptr inbounds i8, ptr %t531, i64 -24
  %t535 = getelementptr inbounds { ptr, i64, i64 }, ptr %t534, i64 0, i32 1
  %t536 = load i64, ptr %t535
  store i64 %t536, ptr %t532
  br label %dynarray.field.done.113
dynarray.field.done.113:
  %t537 = load i64, ptr %t532
  %t538 = icmp ne i64 %t537, 3
  %t539 = icmp eq i1 %t538, 1
  br i1 %t539, label %on.body.115, label %on.end.114
on.body.115:
  ret i32 18
on.end.114:
  %t540 = load ptr, ptr %local.7
  %t541 = getelementptr inbounds i8, ptr %t540, i64 -24
  %t542 = getelementptr inbounds { ptr, i64, i64 }, ptr %t541, i64 0, i32 0
  %t543 = load ptr, ptr %t542
  %t544 = getelementptr inbounds i32, ptr %t543, i32 0
  %t545 = load i32, ptr %t544
  %t546 = icmp ne i32 %t545, 1
  %t547 = icmp eq i1 %t546, 1
  br i1 %t547, label %on.body.117, label %on.end.116
on.body.117:
  ret i32 19
on.end.116:
  %t548 = load ptr, ptr %local.7
  %t549 = getelementptr inbounds i8, ptr %t548, i64 -24
  %t550 = getelementptr inbounds { ptr, i64, i64 }, ptr %t549, i64 0, i32 0
  %t551 = load ptr, ptr %t550
  %t552 = getelementptr inbounds i32, ptr %t551, i32 1
  %t553 = load i32, ptr %t552
  %t554 = icmp ne i32 %t553, 5
  %t555 = icmp eq i1 %t554, 1
  br i1 %t555, label %on.body.119, label %on.end.118
on.body.119:
  ret i32 20
on.end.118:
  %t556 = load ptr, ptr %local.7
  %t557 = getelementptr inbounds i8, ptr %t556, i64 -24
  %t558 = getelementptr inbounds { ptr, i64, i64 }, ptr %t557, i64 0, i32 0
  %t559 = load ptr, ptr %t558
  %t560 = getelementptr inbounds i32, ptr %t559, i32 2
  %t561 = load i32, ptr %t560
  %t562 = icmp ne i32 %t561, 4
  %t563 = icmp eq i1 %t562, 1
  br i1 %t563, label %on.body.121, label %on.end.120
on.body.121:
  ret i32 21
on.end.120:
  %t564 = load ptr, ptr %local.7
  %t565 = icmp eq ptr %t564, null
  br i1 %t565, label %dynarray.free.done.123, label %dynarray.free.122
dynarray.free.122:
  %t566 = getelementptr inbounds i8, ptr %t564, i64 -24
  call void @nrt_mem_free(ptr %t566)
  store ptr null, ptr %local.7
  br label %dynarray.free.done.123
dynarray.free.done.123:
  %t567 = load ptr, ptr %local.3
  %t568 = icmp eq ptr %t567, null
  br i1 %t568, label %dynarray.clear.done.125, label %dynarray.clear.124
dynarray.clear.124:
  %t569 = getelementptr inbounds i8, ptr %t567, i64 -24
  %t570 = getelementptr inbounds { ptr, i64, i64 }, ptr %t569, i64 0, i32 1
  store i64 0, ptr %t570
  br label %dynarray.clear.done.125
dynarray.clear.done.125:
  %t571 = load ptr, ptr %local.3
  %t572 = alloca i64
  %t573 = icmp eq ptr %t571, null
  br i1 %t573, label %dynarray.field.empty.126, label %dynarray.field.load.127
dynarray.field.empty.126:
  store i64 0, ptr %t572
  br label %dynarray.field.done.128
dynarray.field.load.127:
  %t574 = getelementptr inbounds i8, ptr %t571, i64 -24
  %t575 = getelementptr inbounds { ptr, i64, i64 }, ptr %t574, i64 0, i32 1
  %t576 = load i64, ptr %t575
  store i64 %t576, ptr %t572
  br label %dynarray.field.done.128
dynarray.field.done.128:
  %t577 = load i64, ptr %t572
  %t578 = icmp ne i64 %t577, 0
  %t579 = icmp eq i1 %t578, 1
  br i1 %t579, label %on.body.130, label %on.end.129
on.body.130:
  ret i32 22
on.end.129:
  %t580 = load ptr, ptr %local.3
  %t581 = icmp eq ptr %t580, null
  br i1 %t581, label %dynarray.free.done.132, label %dynarray.free.131
dynarray.free.131:
  %t582 = getelementptr inbounds i8, ptr %t580, i64 -24
  call void @nrt_mem_free(ptr %t582)
  store ptr null, ptr %local.3
  br label %dynarray.free.done.132
dynarray.free.done.132:
  %t583 = load ptr, ptr %local.3
  %t584 = icmp ne ptr %t583, null
  %t585 = icmp eq i1 %t584, 1
  br i1 %t585, label %on.body.134, label %on.end.133
on.body.134:
  ret i32 23
on.end.133:
  ret i32 0
}

@$make_words = internal alias ptr (), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
