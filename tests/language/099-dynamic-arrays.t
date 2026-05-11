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

    names.reserve(10)
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
bind make_words = fn.0
bind main = fn.1
func fn.0() -> [..]string {
  expr <unknown> <unsupported>
  let words: [..]string = <unknown> <unsupported>
  expr void call fn (string) -> void field([..]string local.0(words), push)(string "look")
  expr void call fn (string) -> void field([..]string local.0(words), push)(string "north")
  return [..]string local.0(words)
}
func fn.1() -> i32 {
  expr <unknown> <unsupported>
  let empty: [..]string = <unknown> <unsupported>
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
  expr void call bind.2(prn)(string interpolate(string index([]string local.5(view), untyped integer 0), <unknown> " ", string index([]string local.5(view), untyped integer 1), <unknown> " ", string index([]string local.5(view), untyped integer 2), <unknown> " ", string index([]string local.5(view), untyped integer 3), <unknown> " ", string index([]string local.5(view), untyped integer 4)))
  expr void call fn (usize) -> void field([..]string local.3(names), reserve)(usize 10)
  expr void on bool less(usize field([..]string local.3(names), capacity), usize 10) {
    value(bool yes) => {
      return i32 11
    }
  }
  let words: [..]string = [..]string call bind.5(make_words)()
  expr void on bool not_equal(usize field([..]string local.6(words), count), usize 2) {
    value(bool yes) => {
      return i32 12
    }
  }
  expr void call bind.2(prn)(string interpolate(string index([..]string local.6(words), untyped integer 0), <unknown> " ", string index([..]string local.6(words), untyped integer 1)))
  expr void call fn () -> void field([..]string local.6(words), free)()
  expr <unknown> <unsupported>
  let nums: [..]i32 = <unknown> <unsupported>
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
declare ptr @malloc(i64)
declare ptr @realloc(ptr, i64)
declare void @free(ptr)

declare void @$pr({ ptr, i64 })
declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

define internal ptr @fn.0() {
  %local.0 = alloca ptr
  store ptr null, ptr %local.0
  %t0 = load ptr, ptr %local.0
  %t1 = icmp eq ptr %t0, null
  br i1 %t1, label %dynarray.alloc.0, label %dynarray.ready.1
dynarray.alloc.0:
  %t2 = call ptr @malloc(i64 24)
  %t3 = getelementptr inbounds { ptr, i64, i64 }, ptr %t2, i64 0, i32 0
  %t4 = getelementptr inbounds { ptr, i64, i64 }, ptr %t2, i64 0, i32 1
  %t5 = getelementptr inbounds { ptr, i64, i64 }, ptr %t2, i64 0, i32 2
  store ptr null, ptr %t3
  store i64 0, ptr %t4
  store i64 0, ptr %t5
  store ptr %t2, ptr %local.0
  br label %dynarray.ready.1
dynarray.ready.1:
  %t6 = load ptr, ptr %local.0
  %t7 = getelementptr inbounds { ptr, i64, i64 }, ptr %t6, i64 0, i32 0
  %t8 = getelementptr inbounds { ptr, i64, i64 }, ptr %t6, i64 0, i32 1
  %t9 = getelementptr inbounds { ptr, i64, i64 }, ptr %t6, i64 0, i32 2
  %t10 = load ptr, ptr %t7
  %t11 = load i64, ptr %t8
  %t12 = load i64, ptr %t9
  %t13 = add i64 %t11, 1
  %t14 = icmp ugt i64 %t13, %t12
  br i1 %t14, label %dynarray.grow.2, label %dynarray.store.3
dynarray.grow.2:
  %t15 = icmp eq i64 %t12, 0
  %t16 = mul i64 %t12, 2
  %t17 = select i1 %t15, i64 1, i64 %t16
  %t18 = mul i64 %t17, 16
  %t19 = call ptr @realloc(ptr %t10, i64 %t18)
  store ptr %t19, ptr %t7
  store i64 %t17, ptr %t9
  br label %dynarray.store.3
dynarray.store.3:
  %t20 = load ptr, ptr %t7
  %t21 = getelementptr inbounds { ptr, i64 }, ptr %t20, i64 %t11
  store { ptr, i64 } { ptr @.str.m0.0, i64 4 }, ptr %t21
  store i64 %t13, ptr %t8
  %t22 = load ptr, ptr %local.0
  %t23 = icmp eq ptr %t22, null
  br i1 %t23, label %dynarray.alloc.4, label %dynarray.ready.5
dynarray.alloc.4:
  %t24 = call ptr @malloc(i64 24)
  %t25 = getelementptr inbounds { ptr, i64, i64 }, ptr %t24, i64 0, i32 0
  %t26 = getelementptr inbounds { ptr, i64, i64 }, ptr %t24, i64 0, i32 1
  %t27 = getelementptr inbounds { ptr, i64, i64 }, ptr %t24, i64 0, i32 2
  store ptr null, ptr %t25
  store i64 0, ptr %t26
  store i64 0, ptr %t27
  store ptr %t24, ptr %local.0
  br label %dynarray.ready.5
dynarray.ready.5:
  %t28 = load ptr, ptr %local.0
  %t29 = getelementptr inbounds { ptr, i64, i64 }, ptr %t28, i64 0, i32 0
  %t30 = getelementptr inbounds { ptr, i64, i64 }, ptr %t28, i64 0, i32 1
  %t31 = getelementptr inbounds { ptr, i64, i64 }, ptr %t28, i64 0, i32 2
  %t32 = load ptr, ptr %t29
  %t33 = load i64, ptr %t30
  %t34 = load i64, ptr %t31
  %t35 = add i64 %t33, 1
  %t36 = icmp ugt i64 %t35, %t34
  br i1 %t36, label %dynarray.grow.6, label %dynarray.store.7
dynarray.grow.6:
  %t37 = icmp eq i64 %t34, 0
  %t38 = mul i64 %t34, 2
  %t39 = select i1 %t37, i64 1, i64 %t38
  %t40 = mul i64 %t39, 16
  %t41 = call ptr @realloc(ptr %t32, i64 %t40)
  store ptr %t41, ptr %t29
  store i64 %t39, ptr %t31
  br label %dynarray.store.7
dynarray.store.7:
  %t42 = load ptr, ptr %t29
  %t43 = getelementptr inbounds { ptr, i64 }, ptr %t42, i64 %t33
  store { ptr, i64 } { ptr @.str.m0.1, i64 5 }, ptr %t43
  store i64 %t35, ptr %t30
  %t44 = load ptr, ptr %local.0
  ret ptr %t44
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
  %t6 = getelementptr inbounds { ptr, i64, i64 }, ptr %t3, i64 0, i32 1
  %t7 = load i64, ptr %t6
  store i64 %t7, ptr %t4
  br label %dynarray.field.done.4
dynarray.field.done.4:
  %t8 = load i64, ptr %t4
  %t9 = icmp ne i64 %t8, 0
  %t10 = icmp eq i1 %t9, 1
  br i1 %t10, label %on.body.6, label %on.end.5
on.body.6:
  ret i32 2
on.end.5:
  %t11 = load ptr, ptr %local.2
  %t12 = alloca i64
  %t13 = icmp eq ptr %t11, null
  br i1 %t13, label %dynarray.field.empty.7, label %dynarray.field.load.8
dynarray.field.empty.7:
  store i64 0, ptr %t12
  br label %dynarray.field.done.9
dynarray.field.load.8:
  %t14 = getelementptr inbounds { ptr, i64, i64 }, ptr %t11, i64 0, i32 2
  %t15 = load i64, ptr %t14
  store i64 %t15, ptr %t12
  br label %dynarray.field.done.9
dynarray.field.done.9:
  %t16 = load i64, ptr %t12
  %t17 = icmp ne i64 %t16, 0
  %t18 = icmp eq i1 %t17, 1
  br i1 %t18, label %on.body.11, label %on.end.10
on.body.11:
  ret i32 3
on.end.10:
  %t19 = call ptr @malloc(i64 24)
  %t20 = getelementptr inbounds { ptr, i64, i64 }, ptr %t19, i64 0, i32 0
  %t21 = getelementptr inbounds { ptr, i64, i64 }, ptr %t19, i64 0, i32 1
  %t22 = getelementptr inbounds { ptr, i64, i64 }, ptr %t19, i64 0, i32 2
  %t23 = call ptr @malloc(i64 64)
  %t24 = getelementptr inbounds { ptr, i64 }, ptr %t23, i64 0
  store { ptr, i64 } { ptr @.str.m0.2, i64 5 }, ptr %t24
  %t25 = getelementptr inbounds { ptr, i64 }, ptr %t23, i64 1
  store { ptr, i64 } { ptr @.str.m0.3, i64 5 }, ptr %t25
  store ptr %t23, ptr %t20
  store i64 2, ptr %t21
  store i64 4, ptr %t22
  %local.3 = alloca ptr
  store ptr %t19, ptr %local.3
  %t26 = load ptr, ptr %local.3
  %t27 = alloca i64
  %t28 = icmp eq ptr %t26, null
  br i1 %t28, label %dynarray.field.empty.12, label %dynarray.field.load.13
dynarray.field.empty.12:
  store i64 0, ptr %t27
  br label %dynarray.field.done.14
dynarray.field.load.13:
  %t29 = getelementptr inbounds { ptr, i64, i64 }, ptr %t26, i64 0, i32 1
  %t30 = load i64, ptr %t29
  store i64 %t30, ptr %t27
  br label %dynarray.field.done.14
dynarray.field.done.14:
  %t31 = load i64, ptr %t27
  %t32 = icmp ne i64 %t31, 2
  %t33 = icmp eq i1 %t32, 1
  br i1 %t33, label %on.body.16, label %on.end.15
on.body.16:
  ret i32 4
on.end.15:
  %t34 = load ptr, ptr %local.3
  %t35 = alloca i64
  %t36 = icmp eq ptr %t34, null
  br i1 %t36, label %dynarray.field.empty.17, label %dynarray.field.load.18
dynarray.field.empty.17:
  store i64 0, ptr %t35
  br label %dynarray.field.done.19
dynarray.field.load.18:
  %t37 = getelementptr inbounds { ptr, i64, i64 }, ptr %t34, i64 0, i32 2
  %t38 = load i64, ptr %t37
  store i64 %t38, ptr %t35
  br label %dynarray.field.done.19
dynarray.field.done.19:
  %t39 = load i64, ptr %t35
  %t40 = icmp slt i64 %t39, 4
  %t41 = icmp eq i1 %t40, 1
  br i1 %t41, label %on.body.21, label %on.end.20
on.body.21:
  ret i32 5
on.end.20:
  %t42 = load ptr, ptr %local.3
  %t43 = icmp eq ptr %t42, null
  br i1 %t43, label %dynarray.alloc.22, label %dynarray.ready.23
dynarray.alloc.22:
  %t44 = call ptr @malloc(i64 24)
  %t45 = getelementptr inbounds { ptr, i64, i64 }, ptr %t44, i64 0, i32 0
  %t46 = getelementptr inbounds { ptr, i64, i64 }, ptr %t44, i64 0, i32 1
  %t47 = getelementptr inbounds { ptr, i64, i64 }, ptr %t44, i64 0, i32 2
  store ptr null, ptr %t45
  store i64 0, ptr %t46
  store i64 0, ptr %t47
  store ptr %t44, ptr %local.3
  br label %dynarray.ready.23
dynarray.ready.23:
  %t48 = load ptr, ptr %local.3
  %t49 = getelementptr inbounds { ptr, i64, i64 }, ptr %t48, i64 0, i32 0
  %t50 = getelementptr inbounds { ptr, i64, i64 }, ptr %t48, i64 0, i32 1
  %t51 = getelementptr inbounds { ptr, i64, i64 }, ptr %t48, i64 0, i32 2
  %t52 = load ptr, ptr %t49
  %t53 = load i64, ptr %t50
  %t54 = load i64, ptr %t51
  %t55 = add i64 %t53, 1
  %t56 = icmp ugt i64 %t55, %t54
  br i1 %t56, label %dynarray.grow.24, label %dynarray.store.25
dynarray.grow.24:
  %t57 = icmp eq i64 %t54, 0
  %t58 = mul i64 %t54, 2
  %t59 = select i1 %t57, i64 1, i64 %t58
  %t60 = mul i64 %t59, 16
  %t61 = call ptr @realloc(ptr %t52, i64 %t60)
  store ptr %t61, ptr %t49
  store i64 %t59, ptr %t51
  br label %dynarray.store.25
dynarray.store.25:
  %t62 = load ptr, ptr %t49
  %t63 = getelementptr inbounds { ptr, i64 }, ptr %t62, i64 %t53
  store { ptr, i64 } { ptr @.str.m0.4, i64 4 }, ptr %t63
  store i64 %t55, ptr %t50
  %t64 = load ptr, ptr %local.3
  %t65 = alloca i64
  %t66 = icmp eq ptr %t64, null
  br i1 %t66, label %dynarray.field.empty.26, label %dynarray.field.load.27
dynarray.field.empty.26:
  store i64 0, ptr %t65
  br label %dynarray.field.done.28
dynarray.field.load.27:
  %t67 = getelementptr inbounds { ptr, i64, i64 }, ptr %t64, i64 0, i32 1
  %t68 = load i64, ptr %t67
  store i64 %t68, ptr %t65
  br label %dynarray.field.done.28
dynarray.field.done.28:
  %t69 = load i64, ptr %t65
  %t70 = icmp ne i64 %t69, 3
  %t71 = icmp eq i1 %t70, 1
  br i1 %t71, label %on.body.30, label %on.end.29
on.body.30:
  ret i32 6
on.end.29:
  %t72 = insertvalue [2 x { ptr, i64 }] poison, { ptr, i64 } { ptr @.str.m0.5, i64 4 }, 0
  %t73 = insertvalue [2 x { ptr, i64 }] %t72, { ptr, i64 } { ptr @.str.m0.6, i64 2 }, 1
  %local.1 = alloca [2 x { ptr, i64 }]
  store [2 x { ptr, i64 }] %t73, ptr %local.1
  %t74 = getelementptr inbounds [2 x { ptr, i64 }], ptr %local.1, i64 0, i64 0
  %t75 = insertvalue { ptr, i64 } poison, ptr %t74, 0
  %t76 = insertvalue { ptr, i64 } %t75, i64 2, 1
  %t77 = extractvalue { ptr, i64 } %t76, 0
  %t78 = extractvalue { ptr, i64 } %t76, 1
  %t79 = load ptr, ptr %local.3
  %t80 = icmp eq ptr %t79, null
  br i1 %t80, label %dynarray.alloc.31, label %dynarray.ready.32
dynarray.alloc.31:
  %t81 = call ptr @malloc(i64 24)
  %t82 = getelementptr inbounds { ptr, i64, i64 }, ptr %t81, i64 0, i32 0
  %t83 = getelementptr inbounds { ptr, i64, i64 }, ptr %t81, i64 0, i32 1
  %t84 = getelementptr inbounds { ptr, i64, i64 }, ptr %t81, i64 0, i32 2
  store ptr null, ptr %t82
  store i64 0, ptr %t83
  store i64 0, ptr %t84
  store ptr %t81, ptr %local.3
  br label %dynarray.ready.32
dynarray.ready.32:
  %t85 = load ptr, ptr %local.3
  %t86 = getelementptr inbounds { ptr, i64, i64 }, ptr %t85, i64 0, i32 0
  %t87 = getelementptr inbounds { ptr, i64, i64 }, ptr %t85, i64 0, i32 1
  %t88 = getelementptr inbounds { ptr, i64, i64 }, ptr %t85, i64 0, i32 2
  %t89 = load ptr, ptr %t86
  %t90 = load i64, ptr %t87
  %t91 = load i64, ptr %t88
  %t92 = add i64 %t90, %t78
  %t93 = icmp ugt i64 %t92, %t91
  br i1 %t93, label %dynarray.append.grow.33, label %dynarray.append.copy.34
dynarray.append.grow.33:
  %t94 = mul i64 %t92, 16
  %t95 = call ptr @realloc(ptr %t89, i64 %t94)
  store ptr %t95, ptr %t86
  store i64 %t92, ptr %t88
  br label %dynarray.append.copy.34
dynarray.append.copy.34:
  %t96 = load ptr, ptr %t86
  %t97 = alloca i64
  store i64 0, ptr %t97
  br label %dynarray.append.loop.35
dynarray.append.loop.35:
  %t98 = load i64, ptr %t97
  %t99 = icmp ult i64 %t98, %t78
  br i1 %t99, label %dynarray.append.body.36, label %dynarray.append.done.37
dynarray.append.body.36:
  %t100 = add i64 %t90, %t98
  %t101 = getelementptr inbounds { ptr, i64 }, ptr %t77, i64 %t98
  %t102 = getelementptr inbounds { ptr, i64 }, ptr %t96, i64 %t100
  %t103 = load { ptr, i64 }, ptr %t101
  store { ptr, i64 } %t103, ptr %t102
  %t104 = add i64 %t98, 1
  store i64 %t104, ptr %t97
  br label %dynarray.append.loop.35
dynarray.append.done.37:
  store i64 %t92, ptr %t87
  %t105 = load ptr, ptr %local.3
  %t106 = alloca i64
  %t107 = icmp eq ptr %t105, null
  br i1 %t107, label %dynarray.field.empty.38, label %dynarray.field.load.39
dynarray.field.empty.38:
  store i64 0, ptr %t106
  br label %dynarray.field.done.40
dynarray.field.load.39:
  %t108 = getelementptr inbounds { ptr, i64, i64 }, ptr %t105, i64 0, i32 1
  %t109 = load i64, ptr %t108
  store i64 %t109, ptr %t106
  br label %dynarray.field.done.40
dynarray.field.done.40:
  %t110 = load i64, ptr %t106
  %t111 = icmp ne i64 %t110, 5
  %t112 = icmp eq i1 %t111, 1
  br i1 %t112, label %on.body.42, label %on.end.41
on.body.42:
  ret i32 7
on.end.41:
  %t113 = load ptr, ptr %local.3
  %t114 = getelementptr inbounds { ptr, i64, i64 }, ptr %t113, i64 0, i32 0
  %t115 = load ptr, ptr %t114
  %t116 = getelementptr inbounds { ptr, i64, i64 }, ptr %t113, i64 0, i32 1
  %t117 = load i64, ptr %t116
  %t118 = sub i64 %t117, 1
  %t119 = getelementptr inbounds { ptr, i64 }, ptr %t115, i64 %t118
  %t120 = load { ptr, i64 }, ptr %t119
  store i64 %t118, ptr %t116
  %t122 = alloca { ptr, i64 }
  store { ptr, i64 } %t120, ptr %t122
  %t123 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.7, i64 2 }, ptr %t123
  %t121 = call i1 @string_eq(ptr %t122, ptr %t123)
  %t124 = xor i1 %t121, 1
  %t125 = icmp eq i1 %t124, 1
  br i1 %t125, label %on.body.44, label %on.end.43
on.body.44:
  ret i32 8
on.end.43:
  %t126 = load ptr, ptr %local.3
  %t127 = alloca i64
  %t128 = icmp eq ptr %t126, null
  br i1 %t128, label %dynarray.field.empty.45, label %dynarray.field.load.46
dynarray.field.empty.45:
  store i64 0, ptr %t127
  br label %dynarray.field.done.47
dynarray.field.load.46:
  %t129 = getelementptr inbounds { ptr, i64, i64 }, ptr %t126, i64 0, i32 1
  %t130 = load i64, ptr %t129
  store i64 %t130, ptr %t127
  br label %dynarray.field.done.47
dynarray.field.done.47:
  %t131 = load i64, ptr %t127
  %t132 = icmp ne i64 %t131, 4
  %t133 = icmp eq i1 %t132, 1
  br i1 %t133, label %on.body.49, label %on.end.48
on.body.49:
  ret i32 9
on.end.48:
  %t134 = load ptr, ptr %local.3
  %t135 = icmp eq ptr %t134, null
  br i1 %t135, label %dynarray.alloc.50, label %dynarray.ready.51
dynarray.alloc.50:
  %t136 = call ptr @malloc(i64 24)
  %t137 = getelementptr inbounds { ptr, i64, i64 }, ptr %t136, i64 0, i32 0
  %t138 = getelementptr inbounds { ptr, i64, i64 }, ptr %t136, i64 0, i32 1
  %t139 = getelementptr inbounds { ptr, i64, i64 }, ptr %t136, i64 0, i32 2
  store ptr null, ptr %t137
  store i64 0, ptr %t138
  store i64 0, ptr %t139
  store ptr %t136, ptr %local.3
  br label %dynarray.ready.51
dynarray.ready.51:
  %t140 = load ptr, ptr %local.3
  %t141 = getelementptr inbounds { ptr, i64, i64 }, ptr %t140, i64 0, i32 0
  %t142 = getelementptr inbounds { ptr, i64, i64 }, ptr %t140, i64 0, i32 1
  %t143 = getelementptr inbounds { ptr, i64, i64 }, ptr %t140, i64 0, i32 2
  %t144 = load ptr, ptr %t141
  %t145 = load i64, ptr %t142
  %t146 = load i64, ptr %t143
  %t147 = add i64 %t145, 1
  %t148 = icmp ugt i64 %t147, %t146
  br i1 %t148, label %dynarray.grow.52, label %dynarray.store.53
dynarray.grow.52:
  %t149 = icmp eq i64 %t146, 0
  %t150 = mul i64 %t146, 2
  %t151 = select i1 %t149, i64 1, i64 %t150
  %t152 = mul i64 %t151, 16
  %t153 = call ptr @realloc(ptr %t144, i64 %t152)
  store ptr %t153, ptr %t141
  store i64 %t151, ptr %t143
  br label %dynarray.store.53
dynarray.store.53:
  %t154 = load ptr, ptr %t141
  %t155 = getelementptr inbounds { ptr, i64 }, ptr %t154, i64 %t145
  store { ptr, i64 } %t120, ptr %t155
  store i64 %t147, ptr %t142
  %t156 = load ptr, ptr %local.3
  %t159 = alloca ptr
  %t160 = alloca i64
  %t161 = icmp eq ptr %t156, null
  br i1 %t161, label %dynarray.slice.empty.54, label %dynarray.slice.load.55
dynarray.slice.empty.54:
  store ptr null, ptr %t159
  store i64 0, ptr %t160
  br label %dynarray.slice.ready.56
dynarray.slice.load.55:
  %t162 = getelementptr inbounds { ptr, i64, i64 }, ptr %t156, i64 0, i32 0
  %t163 = load ptr, ptr %t162
  %t164 = getelementptr inbounds { ptr, i64, i64 }, ptr %t156, i64 0, i32 1
  %t165 = load i64, ptr %t164
  store ptr %t163, ptr %t159
  store i64 %t165, ptr %t160
  br label %dynarray.slice.ready.56
dynarray.slice.ready.56:
  %t166 = load i64, ptr %t160
  %t167 = load ptr, ptr %t159
  %t168 = getelementptr inbounds { ptr, i64 }, ptr %t167, i64 0
  %t169 = insertvalue { ptr, i64 } poison, ptr %t168, 0
  %t170 = insertvalue { ptr, i64 } %t169, i64 %t166, 1
  %t171 = extractvalue { ptr, i64 } %t170, 1
  %t172 = icmp ne i64 %t171, 5
  %t173 = icmp eq i1 %t172, 1
  br i1 %t173, label %on.body.58, label %on.end.57
on.body.58:
  ret i32 10
on.end.57:
  %t174 = call i64 @string_builder_mark()
  %t175 = extractvalue { ptr, i64 } %t170, 0
  %t176 = getelementptr inbounds { ptr, i64 }, ptr %t175, i32 0
  %t177 = load { ptr, i64 }, ptr %t176
  %t178 = alloca { ptr, i64 }
  %t179 = alloca { ptr, i64 }
  store { ptr, i64 } %t177, ptr %t179
  call void @to_string$string(ptr %t178, ptr %t179)
  call void @string_builder_append_string(ptr %t178)
  %t180 = alloca { ptr, i64 }
  %t181 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.8, i64 1 }, ptr %t181
  call void @to_string$string(ptr %t180, ptr %t181)
  call void @string_builder_append_string(ptr %t180)
  %t182 = extractvalue { ptr, i64 } %t170, 0
  %t183 = getelementptr inbounds { ptr, i64 }, ptr %t182, i32 1
  %t184 = load { ptr, i64 }, ptr %t183
  %t185 = alloca { ptr, i64 }
  %t186 = alloca { ptr, i64 }
  store { ptr, i64 } %t184, ptr %t186
  call void @to_string$string(ptr %t185, ptr %t186)
  call void @string_builder_append_string(ptr %t185)
  %t187 = alloca { ptr, i64 }
  %t188 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.9, i64 1 }, ptr %t188
  call void @to_string$string(ptr %t187, ptr %t188)
  call void @string_builder_append_string(ptr %t187)
  %t189 = extractvalue { ptr, i64 } %t170, 0
  %t190 = getelementptr inbounds { ptr, i64 }, ptr %t189, i32 2
  %t191 = load { ptr, i64 }, ptr %t190
  %t192 = alloca { ptr, i64 }
  %t193 = alloca { ptr, i64 }
  store { ptr, i64 } %t191, ptr %t193
  call void @to_string$string(ptr %t192, ptr %t193)
  call void @string_builder_append_string(ptr %t192)
  %t194 = alloca { ptr, i64 }
  %t195 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.10, i64 1 }, ptr %t195
  call void @to_string$string(ptr %t194, ptr %t195)
  call void @string_builder_append_string(ptr %t194)
  %t196 = extractvalue { ptr, i64 } %t170, 0
  %t197 = getelementptr inbounds { ptr, i64 }, ptr %t196, i32 3
  %t198 = load { ptr, i64 }, ptr %t197
  %t199 = alloca { ptr, i64 }
  %t200 = alloca { ptr, i64 }
  store { ptr, i64 } %t198, ptr %t200
  call void @to_string$string(ptr %t199, ptr %t200)
  call void @string_builder_append_string(ptr %t199)
  %t201 = alloca { ptr, i64 }
  %t202 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.11, i64 1 }, ptr %t202
  call void @to_string$string(ptr %t201, ptr %t202)
  call void @string_builder_append_string(ptr %t201)
  %t203 = extractvalue { ptr, i64 } %t170, 0
  %t204 = getelementptr inbounds { ptr, i64 }, ptr %t203, i32 4
  %t205 = load { ptr, i64 }, ptr %t204
  %t206 = alloca { ptr, i64 }
  %t207 = alloca { ptr, i64 }
  store { ptr, i64 } %t205, ptr %t207
  call void @to_string$string(ptr %t206, ptr %t207)
  call void @string_builder_append_string(ptr %t206)
  %t208 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t208, i64 %t174)
  %t209 = load { ptr, i64 }, ptr %t208
  call void @$prn({ ptr, i64 } %t209)
  %t210 = load ptr, ptr %local.3
  %t211 = icmp eq ptr %t210, null
  br i1 %t211, label %dynarray.alloc.59, label %dynarray.ready.60
dynarray.alloc.59:
  %t212 = call ptr @malloc(i64 24)
  %t213 = getelementptr inbounds { ptr, i64, i64 }, ptr %t212, i64 0, i32 0
  %t214 = getelementptr inbounds { ptr, i64, i64 }, ptr %t212, i64 0, i32 1
  %t215 = getelementptr inbounds { ptr, i64, i64 }, ptr %t212, i64 0, i32 2
  store ptr null, ptr %t213
  store i64 0, ptr %t214
  store i64 0, ptr %t215
  store ptr %t212, ptr %local.3
  br label %dynarray.ready.60
dynarray.ready.60:
  %t216 = load ptr, ptr %local.3
  %t217 = getelementptr inbounds { ptr, i64, i64 }, ptr %t216, i64 0, i32 0
  %t218 = getelementptr inbounds { ptr, i64, i64 }, ptr %t216, i64 0, i32 2
  %t219 = load ptr, ptr %t217
  %t220 = load i64, ptr %t218
  %t221 = icmp ugt i64 10, %t220
  br i1 %t221, label %dynarray.reserve.grow.61, label %dynarray.reserve.done.62
dynarray.reserve.grow.61:
  %t222 = mul i64 10, 16
  %t223 = call ptr @realloc(ptr %t219, i64 %t222)
  store ptr %t223, ptr %t217
  store i64 10, ptr %t218
  br label %dynarray.reserve.done.62
dynarray.reserve.done.62:
  %t224 = load ptr, ptr %local.3
  %t225 = alloca i64
  %t226 = icmp eq ptr %t224, null
  br i1 %t226, label %dynarray.field.empty.63, label %dynarray.field.load.64
dynarray.field.empty.63:
  store i64 0, ptr %t225
  br label %dynarray.field.done.65
dynarray.field.load.64:
  %t227 = getelementptr inbounds { ptr, i64, i64 }, ptr %t224, i64 0, i32 2
  %t228 = load i64, ptr %t227
  store i64 %t228, ptr %t225
  br label %dynarray.field.done.65
dynarray.field.done.65:
  %t229 = load i64, ptr %t225
  %t230 = icmp slt i64 %t229, 10
  %t231 = icmp eq i1 %t230, 1
  br i1 %t231, label %on.body.67, label %on.end.66
on.body.67:
  ret i32 11
on.end.66:
  %t232 = call ptr @fn.0()
  %local.6 = alloca ptr
  store ptr %t232, ptr %local.6
  %t233 = load ptr, ptr %local.6
  %t234 = alloca i64
  %t235 = icmp eq ptr %t233, null
  br i1 %t235, label %dynarray.field.empty.68, label %dynarray.field.load.69
dynarray.field.empty.68:
  store i64 0, ptr %t234
  br label %dynarray.field.done.70
dynarray.field.load.69:
  %t236 = getelementptr inbounds { ptr, i64, i64 }, ptr %t233, i64 0, i32 1
  %t237 = load i64, ptr %t236
  store i64 %t237, ptr %t234
  br label %dynarray.field.done.70
dynarray.field.done.70:
  %t238 = load i64, ptr %t234
  %t239 = icmp ne i64 %t238, 2
  %t240 = icmp eq i1 %t239, 1
  br i1 %t240, label %on.body.72, label %on.end.71
on.body.72:
  ret i32 12
on.end.71:
  %t241 = call i64 @string_builder_mark()
  %t242 = load ptr, ptr %local.6
  %t243 = getelementptr inbounds { ptr, i64, i64 }, ptr %t242, i64 0, i32 0
  %t244 = load ptr, ptr %t243
  %t245 = getelementptr inbounds { ptr, i64 }, ptr %t244, i32 0
  %t246 = load { ptr, i64 }, ptr %t245
  %t247 = alloca { ptr, i64 }
  %t248 = alloca { ptr, i64 }
  store { ptr, i64 } %t246, ptr %t248
  call void @to_string$string(ptr %t247, ptr %t248)
  call void @string_builder_append_string(ptr %t247)
  %t249 = alloca { ptr, i64 }
  %t250 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.12, i64 1 }, ptr %t250
  call void @to_string$string(ptr %t249, ptr %t250)
  call void @string_builder_append_string(ptr %t249)
  %t251 = load ptr, ptr %local.6
  %t252 = getelementptr inbounds { ptr, i64, i64 }, ptr %t251, i64 0, i32 0
  %t253 = load ptr, ptr %t252
  %t254 = getelementptr inbounds { ptr, i64 }, ptr %t253, i32 1
  %t255 = load { ptr, i64 }, ptr %t254
  %t256 = alloca { ptr, i64 }
  %t257 = alloca { ptr, i64 }
  store { ptr, i64 } %t255, ptr %t257
  call void @to_string$string(ptr %t256, ptr %t257)
  call void @string_builder_append_string(ptr %t256)
  %t258 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t258, i64 %t241)
  %t259 = load { ptr, i64 }, ptr %t258
  call void @$prn({ ptr, i64 } %t259)
  %t260 = load ptr, ptr %local.6
  %t261 = icmp eq ptr %t260, null
  br i1 %t261, label %dynarray.free.done.74, label %dynarray.free.73
dynarray.free.73:
  %t262 = getelementptr inbounds { ptr, i64, i64 }, ptr %t260, i64 0, i32 0
  %t263 = load ptr, ptr %t262
  call void @free(ptr %t263)
  call void @free(ptr %t260)
  store ptr null, ptr %local.6
  br label %dynarray.free.done.74
dynarray.free.done.74:
  %local.7 = alloca ptr
  store ptr null, ptr %local.7
  %t264 = load ptr, ptr %local.7
  %t265 = icmp eq ptr %t264, null
  br i1 %t265, label %dynarray.alloc.75, label %dynarray.ready.76
dynarray.alloc.75:
  %t266 = call ptr @malloc(i64 24)
  %t267 = getelementptr inbounds { ptr, i64, i64 }, ptr %t266, i64 0, i32 0
  %t268 = getelementptr inbounds { ptr, i64, i64 }, ptr %t266, i64 0, i32 1
  %t269 = getelementptr inbounds { ptr, i64, i64 }, ptr %t266, i64 0, i32 2
  store ptr null, ptr %t267
  store i64 0, ptr %t268
  store i64 0, ptr %t269
  store ptr %t266, ptr %local.7
  br label %dynarray.ready.76
dynarray.ready.76:
  %t270 = load ptr, ptr %local.7
  %t271 = getelementptr inbounds { ptr, i64, i64 }, ptr %t270, i64 0, i32 0
  %t272 = getelementptr inbounds { ptr, i64, i64 }, ptr %t270, i64 0, i32 1
  %t273 = getelementptr inbounds { ptr, i64, i64 }, ptr %t270, i64 0, i32 2
  %t274 = load ptr, ptr %t271
  %t275 = load i64, ptr %t272
  %t276 = load i64, ptr %t273
  %t277 = add i64 %t275, 1
  %t278 = icmp ugt i64 %t277, %t276
  br i1 %t278, label %dynarray.grow.77, label %dynarray.store.78
dynarray.grow.77:
  %t279 = icmp eq i64 %t276, 0
  %t280 = mul i64 %t276, 2
  %t281 = select i1 %t279, i64 1, i64 %t280
  %t282 = mul i64 %t281, 4
  %t283 = call ptr @realloc(ptr %t274, i64 %t282)
  store ptr %t283, ptr %t271
  store i64 %t281, ptr %t273
  br label %dynarray.store.78
dynarray.store.78:
  %t284 = load ptr, ptr %t271
  %t285 = getelementptr inbounds i32, ptr %t284, i64 %t275
  store i32 1, ptr %t285
  store i64 %t277, ptr %t272
  %t286 = load ptr, ptr %local.7
  %t287 = icmp eq ptr %t286, null
  br i1 %t287, label %dynarray.alloc.79, label %dynarray.ready.80
dynarray.alloc.79:
  %t288 = call ptr @malloc(i64 24)
  %t289 = getelementptr inbounds { ptr, i64, i64 }, ptr %t288, i64 0, i32 0
  %t290 = getelementptr inbounds { ptr, i64, i64 }, ptr %t288, i64 0, i32 1
  %t291 = getelementptr inbounds { ptr, i64, i64 }, ptr %t288, i64 0, i32 2
  store ptr null, ptr %t289
  store i64 0, ptr %t290
  store i64 0, ptr %t291
  store ptr %t288, ptr %local.7
  br label %dynarray.ready.80
dynarray.ready.80:
  %t292 = load ptr, ptr %local.7
  %t293 = getelementptr inbounds { ptr, i64, i64 }, ptr %t292, i64 0, i32 0
  %t294 = getelementptr inbounds { ptr, i64, i64 }, ptr %t292, i64 0, i32 1
  %t295 = getelementptr inbounds { ptr, i64, i64 }, ptr %t292, i64 0, i32 2
  %t296 = load ptr, ptr %t293
  %t297 = load i64, ptr %t294
  %t298 = load i64, ptr %t295
  %t299 = add i64 %t297, 1
  %t300 = icmp ugt i64 %t299, %t298
  br i1 %t300, label %dynarray.grow.81, label %dynarray.store.82
dynarray.grow.81:
  %t301 = icmp eq i64 %t298, 0
  %t302 = mul i64 %t298, 2
  %t303 = select i1 %t301, i64 1, i64 %t302
  %t304 = mul i64 %t303, 4
  %t305 = call ptr @realloc(ptr %t296, i64 %t304)
  store ptr %t305, ptr %t293
  store i64 %t303, ptr %t295
  br label %dynarray.store.82
dynarray.store.82:
  %t306 = load ptr, ptr %t293
  %t307 = getelementptr inbounds i32, ptr %t306, i64 %t297
  store i32 2, ptr %t307
  store i64 %t299, ptr %t294
  %t308 = load ptr, ptr %local.7
  %t309 = icmp eq ptr %t308, null
  br i1 %t309, label %dynarray.alloc.83, label %dynarray.ready.84
dynarray.alloc.83:
  %t310 = call ptr @malloc(i64 24)
  %t311 = getelementptr inbounds { ptr, i64, i64 }, ptr %t310, i64 0, i32 0
  %t312 = getelementptr inbounds { ptr, i64, i64 }, ptr %t310, i64 0, i32 1
  %t313 = getelementptr inbounds { ptr, i64, i64 }, ptr %t310, i64 0, i32 2
  store ptr null, ptr %t311
  store i64 0, ptr %t312
  store i64 0, ptr %t313
  store ptr %t310, ptr %local.7
  br label %dynarray.ready.84
dynarray.ready.84:
  %t314 = load ptr, ptr %local.7
  %t315 = getelementptr inbounds { ptr, i64, i64 }, ptr %t314, i64 0, i32 0
  %t316 = getelementptr inbounds { ptr, i64, i64 }, ptr %t314, i64 0, i32 1
  %t317 = getelementptr inbounds { ptr, i64, i64 }, ptr %t314, i64 0, i32 2
  %t318 = load ptr, ptr %t315
  %t319 = load i64, ptr %t316
  %t320 = load i64, ptr %t317
  %t321 = add i64 %t319, 1
  %t322 = icmp ugt i64 %t321, %t320
  br i1 %t322, label %dynarray.grow.85, label %dynarray.store.86
dynarray.grow.85:
  %t323 = icmp eq i64 %t320, 0
  %t324 = mul i64 %t320, 2
  %t325 = select i1 %t323, i64 1, i64 %t324
  %t326 = mul i64 %t325, 4
  %t327 = call ptr @realloc(ptr %t318, i64 %t326)
  store ptr %t327, ptr %t315
  store i64 %t325, ptr %t317
  br label %dynarray.store.86
dynarray.store.86:
  %t328 = load ptr, ptr %t315
  %t329 = getelementptr inbounds i32, ptr %t328, i64 %t319
  store i32 3, ptr %t329
  store i64 %t321, ptr %t316
  %t330 = load ptr, ptr %local.7
  %t331 = icmp eq ptr %t330, null
  br i1 %t331, label %dynarray.alloc.87, label %dynarray.ready.88
dynarray.alloc.87:
  %t332 = call ptr @malloc(i64 24)
  %t333 = getelementptr inbounds { ptr, i64, i64 }, ptr %t332, i64 0, i32 0
  %t334 = getelementptr inbounds { ptr, i64, i64 }, ptr %t332, i64 0, i32 1
  %t335 = getelementptr inbounds { ptr, i64, i64 }, ptr %t332, i64 0, i32 2
  store ptr null, ptr %t333
  store i64 0, ptr %t334
  store i64 0, ptr %t335
  store ptr %t332, ptr %local.7
  br label %dynarray.ready.88
dynarray.ready.88:
  %t336 = load ptr, ptr %local.7
  %t337 = getelementptr inbounds { ptr, i64, i64 }, ptr %t336, i64 0, i32 0
  %t338 = getelementptr inbounds { ptr, i64, i64 }, ptr %t336, i64 0, i32 1
  %t339 = getelementptr inbounds { ptr, i64, i64 }, ptr %t336, i64 0, i32 2
  %t340 = load ptr, ptr %t337
  %t341 = load i64, ptr %t338
  %t342 = load i64, ptr %t339
  %t343 = add i64 %t341, 1
  %t344 = icmp ugt i64 %t343, %t342
  br i1 %t344, label %dynarray.grow.89, label %dynarray.store.90
dynarray.grow.89:
  %t345 = icmp eq i64 %t342, 0
  %t346 = mul i64 %t342, 2
  %t347 = select i1 %t345, i64 1, i64 %t346
  %t348 = mul i64 %t347, 4
  %t349 = call ptr @realloc(ptr %t340, i64 %t348)
  store ptr %t349, ptr %t337
  store i64 %t347, ptr %t339
  br label %dynarray.store.90
dynarray.store.90:
  %t350 = load ptr, ptr %t337
  %t351 = getelementptr inbounds i32, ptr %t350, i64 %t341
  store i32 4, ptr %t351
  store i64 %t343, ptr %t338
  %t352 = load ptr, ptr %local.7
  %t353 = icmp eq ptr %t352, null
  br i1 %t353, label %dynarray.alloc.91, label %dynarray.ready.92
dynarray.alloc.91:
  %t354 = call ptr @malloc(i64 24)
  %t355 = getelementptr inbounds { ptr, i64, i64 }, ptr %t354, i64 0, i32 0
  %t356 = getelementptr inbounds { ptr, i64, i64 }, ptr %t354, i64 0, i32 1
  %t357 = getelementptr inbounds { ptr, i64, i64 }, ptr %t354, i64 0, i32 2
  store ptr null, ptr %t355
  store i64 0, ptr %t356
  store i64 0, ptr %t357
  store ptr %t354, ptr %local.7
  br label %dynarray.ready.92
dynarray.ready.92:
  %t358 = load ptr, ptr %local.7
  %t359 = getelementptr inbounds { ptr, i64, i64 }, ptr %t358, i64 0, i32 0
  %t360 = getelementptr inbounds { ptr, i64, i64 }, ptr %t358, i64 0, i32 1
  %t361 = getelementptr inbounds { ptr, i64, i64 }, ptr %t358, i64 0, i32 2
  %t362 = load ptr, ptr %t359
  %t363 = load i64, ptr %t360
  %t364 = load i64, ptr %t361
  %t365 = add i64 %t363, 1
  %t366 = icmp ugt i64 %t365, %t364
  br i1 %t366, label %dynarray.grow.93, label %dynarray.store.94
dynarray.grow.93:
  %t367 = icmp eq i64 %t364, 0
  %t368 = mul i64 %t364, 2
  %t369 = select i1 %t367, i64 1, i64 %t368
  %t370 = mul i64 %t369, 4
  %t371 = call ptr @realloc(ptr %t362, i64 %t370)
  store ptr %t371, ptr %t359
  store i64 %t369, ptr %t361
  br label %dynarray.store.94
dynarray.store.94:
  %t372 = load ptr, ptr %t359
  %t373 = getelementptr inbounds i32, ptr %t372, i64 %t363
  store i32 5, ptr %t373
  store i64 %t365, ptr %t360
  %t374 = load ptr, ptr %local.7
  %t375 = getelementptr inbounds { ptr, i64, i64 }, ptr %t374, i64 0, i32 0
  %t376 = load ptr, ptr %t375
  %t377 = getelementptr inbounds { ptr, i64, i64 }, ptr %t374, i64 0, i32 1
  %t378 = load i64, ptr %t377
  %t379 = sub i64 %t378, 1
  %t380 = alloca i64
  store i64 1, ptr %t380
  br label %dynarray.delete.loop.95
dynarray.delete.loop.95:
  %t381 = load i64, ptr %t380
  %t382 = icmp ult i64 %t381, %t379
  br i1 %t382, label %dynarray.delete.body.96, label %dynarray.delete.done.97
dynarray.delete.body.96:
  %t383 = add i64 %t381, 1
  %t384 = getelementptr inbounds i32, ptr %t376, i64 %t383
  %t385 = getelementptr inbounds i32, ptr %t376, i64 %t381
  %t386 = load i32, ptr %t384
  store i32 %t386, ptr %t385
  store i64 %t383, ptr %t380
  br label %dynarray.delete.loop.95
dynarray.delete.done.97:
  store i64 %t379, ptr %t377
  %t387 = load ptr, ptr %local.7
  %t388 = alloca i64
  %t389 = icmp eq ptr %t387, null
  br i1 %t389, label %dynarray.field.empty.98, label %dynarray.field.load.99
dynarray.field.empty.98:
  store i64 0, ptr %t388
  br label %dynarray.field.done.100
dynarray.field.load.99:
  %t390 = getelementptr inbounds { ptr, i64, i64 }, ptr %t387, i64 0, i32 1
  %t391 = load i64, ptr %t390
  store i64 %t391, ptr %t388
  br label %dynarray.field.done.100
dynarray.field.done.100:
  %t392 = load i64, ptr %t388
  %t393 = icmp ne i64 %t392, 4
  %t394 = icmp eq i1 %t393, 1
  br i1 %t394, label %on.body.102, label %on.end.101
on.body.102:
  ret i32 13
on.end.101:
  %t395 = load ptr, ptr %local.7
  %t396 = getelementptr inbounds { ptr, i64, i64 }, ptr %t395, i64 0, i32 0
  %t397 = load ptr, ptr %t396
  %t398 = getelementptr inbounds i32, ptr %t397, i32 0
  %t399 = load i32, ptr %t398
  %t400 = icmp ne i32 %t399, 1
  %t401 = icmp eq i1 %t400, 1
  br i1 %t401, label %on.body.104, label %on.end.103
on.body.104:
  ret i32 14
on.end.103:
  %t402 = load ptr, ptr %local.7
  %t403 = getelementptr inbounds { ptr, i64, i64 }, ptr %t402, i64 0, i32 0
  %t404 = load ptr, ptr %t403
  %t405 = getelementptr inbounds i32, ptr %t404, i32 1
  %t406 = load i32, ptr %t405
  %t407 = icmp ne i32 %t406, 3
  %t408 = icmp eq i1 %t407, 1
  br i1 %t408, label %on.body.106, label %on.end.105
on.body.106:
  ret i32 15
on.end.105:
  %t409 = load ptr, ptr %local.7
  %t410 = getelementptr inbounds { ptr, i64, i64 }, ptr %t409, i64 0, i32 0
  %t411 = load ptr, ptr %t410
  %t412 = getelementptr inbounds i32, ptr %t411, i32 2
  %t413 = load i32, ptr %t412
  %t414 = icmp ne i32 %t413, 4
  %t415 = icmp eq i1 %t414, 1
  br i1 %t415, label %on.body.108, label %on.end.107
on.body.108:
  ret i32 16
on.end.107:
  %t416 = load ptr, ptr %local.7
  %t417 = getelementptr inbounds { ptr, i64, i64 }, ptr %t416, i64 0, i32 0
  %t418 = load ptr, ptr %t417
  %t419 = getelementptr inbounds i32, ptr %t418, i32 3
  %t420 = load i32, ptr %t419
  %t421 = icmp ne i32 %t420, 5
  %t422 = icmp eq i1 %t421, 1
  br i1 %t422, label %on.body.110, label %on.end.109
on.body.110:
  ret i32 17
on.end.109:
  %t423 = load ptr, ptr %local.7
  %t424 = getelementptr inbounds { ptr, i64, i64 }, ptr %t423, i64 0, i32 0
  %t425 = load ptr, ptr %t424
  %t426 = getelementptr inbounds { ptr, i64, i64 }, ptr %t423, i64 0, i32 1
  %t427 = load i64, ptr %t426
  %t428 = sub i64 %t427, 1
  %t429 = getelementptr inbounds i32, ptr %t425, i64 %t428
  %t430 = getelementptr inbounds i32, ptr %t425, i64 1
  %t431 = load i32, ptr %t429
  store i32 %t431, ptr %t430
  store i64 %t428, ptr %t426
  %t432 = load ptr, ptr %local.7
  %t433 = alloca i64
  %t434 = icmp eq ptr %t432, null
  br i1 %t434, label %dynarray.field.empty.111, label %dynarray.field.load.112
dynarray.field.empty.111:
  store i64 0, ptr %t433
  br label %dynarray.field.done.113
dynarray.field.load.112:
  %t435 = getelementptr inbounds { ptr, i64, i64 }, ptr %t432, i64 0, i32 1
  %t436 = load i64, ptr %t435
  store i64 %t436, ptr %t433
  br label %dynarray.field.done.113
dynarray.field.done.113:
  %t437 = load i64, ptr %t433
  %t438 = icmp ne i64 %t437, 3
  %t439 = icmp eq i1 %t438, 1
  br i1 %t439, label %on.body.115, label %on.end.114
on.body.115:
  ret i32 18
on.end.114:
  %t440 = load ptr, ptr %local.7
  %t441 = getelementptr inbounds { ptr, i64, i64 }, ptr %t440, i64 0, i32 0
  %t442 = load ptr, ptr %t441
  %t443 = getelementptr inbounds i32, ptr %t442, i32 0
  %t444 = load i32, ptr %t443
  %t445 = icmp ne i32 %t444, 1
  %t446 = icmp eq i1 %t445, 1
  br i1 %t446, label %on.body.117, label %on.end.116
on.body.117:
  ret i32 19
on.end.116:
  %t447 = load ptr, ptr %local.7
  %t448 = getelementptr inbounds { ptr, i64, i64 }, ptr %t447, i64 0, i32 0
  %t449 = load ptr, ptr %t448
  %t450 = getelementptr inbounds i32, ptr %t449, i32 1
  %t451 = load i32, ptr %t450
  %t452 = icmp ne i32 %t451, 5
  %t453 = icmp eq i1 %t452, 1
  br i1 %t453, label %on.body.119, label %on.end.118
on.body.119:
  ret i32 20
on.end.118:
  %t454 = load ptr, ptr %local.7
  %t455 = getelementptr inbounds { ptr, i64, i64 }, ptr %t454, i64 0, i32 0
  %t456 = load ptr, ptr %t455
  %t457 = getelementptr inbounds i32, ptr %t456, i32 2
  %t458 = load i32, ptr %t457
  %t459 = icmp ne i32 %t458, 4
  %t460 = icmp eq i1 %t459, 1
  br i1 %t460, label %on.body.121, label %on.end.120
on.body.121:
  ret i32 21
on.end.120:
  %t461 = load ptr, ptr %local.7
  %t462 = icmp eq ptr %t461, null
  br i1 %t462, label %dynarray.free.done.123, label %dynarray.free.122
dynarray.free.122:
  %t463 = getelementptr inbounds { ptr, i64, i64 }, ptr %t461, i64 0, i32 0
  %t464 = load ptr, ptr %t463
  call void @free(ptr %t464)
  call void @free(ptr %t461)
  store ptr null, ptr %local.7
  br label %dynarray.free.done.123
dynarray.free.done.123:
  %t465 = load ptr, ptr %local.3
  %t466 = icmp eq ptr %t465, null
  br i1 %t466, label %dynarray.clear.done.125, label %dynarray.clear.124
dynarray.clear.124:
  %t467 = getelementptr inbounds { ptr, i64, i64 }, ptr %t465, i64 0, i32 1
  store i64 0, ptr %t467
  br label %dynarray.clear.done.125
dynarray.clear.done.125:
  %t468 = load ptr, ptr %local.3
  %t469 = alloca i64
  %t470 = icmp eq ptr %t468, null
  br i1 %t470, label %dynarray.field.empty.126, label %dynarray.field.load.127
dynarray.field.empty.126:
  store i64 0, ptr %t469
  br label %dynarray.field.done.128
dynarray.field.load.127:
  %t471 = getelementptr inbounds { ptr, i64, i64 }, ptr %t468, i64 0, i32 1
  %t472 = load i64, ptr %t471
  store i64 %t472, ptr %t469
  br label %dynarray.field.done.128
dynarray.field.done.128:
  %t473 = load i64, ptr %t469
  %t474 = icmp ne i64 %t473, 0
  %t475 = icmp eq i1 %t474, 1
  br i1 %t475, label %on.body.130, label %on.end.129
on.body.130:
  ret i32 22
on.end.129:
  %t476 = load ptr, ptr %local.3
  %t477 = icmp eq ptr %t476, null
  br i1 %t477, label %dynarray.free.done.132, label %dynarray.free.131
dynarray.free.131:
  %t478 = getelementptr inbounds { ptr, i64, i64 }, ptr %t476, i64 0, i32 0
  %t479 = load ptr, ptr %t478
  call void @free(ptr %t479)
  call void @free(ptr %t476)
  store ptr null, ptr %local.3
  br label %dynarray.free.done.132
dynarray.free.done.132:
  %t480 = load ptr, ptr %local.3
  %t481 = icmp ne ptr %t480, null
  %t482 = icmp eq i1 %t481, 1
  br i1 %t482, label %on.body.134, label %on.end.133
on.body.134:
  ret i32 23
on.end.133:
  ret i32 0
}

@$make_words = internal alias ptr (), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
