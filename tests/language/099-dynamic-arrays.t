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
  %t121 = call i1 @string_eq({ ptr, i64 } %t120, { ptr, i64 } { ptr @.str.m0.7, i64 2 })
  %t122 = xor i1 %t121, 1
  %t123 = icmp eq i1 %t122, 1
  br i1 %t123, label %on.body.44, label %on.end.43
on.body.44:
  ret i32 8
on.end.43:
  %t124 = load ptr, ptr %local.3
  %t125 = alloca i64
  %t126 = icmp eq ptr %t124, null
  br i1 %t126, label %dynarray.field.empty.45, label %dynarray.field.load.46
dynarray.field.empty.45:
  store i64 0, ptr %t125
  br label %dynarray.field.done.47
dynarray.field.load.46:
  %t127 = getelementptr inbounds { ptr, i64, i64 }, ptr %t124, i64 0, i32 1
  %t128 = load i64, ptr %t127
  store i64 %t128, ptr %t125
  br label %dynarray.field.done.47
dynarray.field.done.47:
  %t129 = load i64, ptr %t125
  %t130 = icmp ne i64 %t129, 4
  %t131 = icmp eq i1 %t130, 1
  br i1 %t131, label %on.body.49, label %on.end.48
on.body.49:
  ret i32 9
on.end.48:
  %t132 = load ptr, ptr %local.3
  %t133 = icmp eq ptr %t132, null
  br i1 %t133, label %dynarray.alloc.50, label %dynarray.ready.51
dynarray.alloc.50:
  %t134 = call ptr @malloc(i64 24)
  %t135 = getelementptr inbounds { ptr, i64, i64 }, ptr %t134, i64 0, i32 0
  %t136 = getelementptr inbounds { ptr, i64, i64 }, ptr %t134, i64 0, i32 1
  %t137 = getelementptr inbounds { ptr, i64, i64 }, ptr %t134, i64 0, i32 2
  store ptr null, ptr %t135
  store i64 0, ptr %t136
  store i64 0, ptr %t137
  store ptr %t134, ptr %local.3
  br label %dynarray.ready.51
dynarray.ready.51:
  %t138 = load ptr, ptr %local.3
  %t139 = getelementptr inbounds { ptr, i64, i64 }, ptr %t138, i64 0, i32 0
  %t140 = getelementptr inbounds { ptr, i64, i64 }, ptr %t138, i64 0, i32 1
  %t141 = getelementptr inbounds { ptr, i64, i64 }, ptr %t138, i64 0, i32 2
  %t142 = load ptr, ptr %t139
  %t143 = load i64, ptr %t140
  %t144 = load i64, ptr %t141
  %t145 = add i64 %t143, 1
  %t146 = icmp ugt i64 %t145, %t144
  br i1 %t146, label %dynarray.grow.52, label %dynarray.store.53
dynarray.grow.52:
  %t147 = icmp eq i64 %t144, 0
  %t148 = mul i64 %t144, 2
  %t149 = select i1 %t147, i64 1, i64 %t148
  %t150 = mul i64 %t149, 16
  %t151 = call ptr @realloc(ptr %t142, i64 %t150)
  store ptr %t151, ptr %t139
  store i64 %t149, ptr %t141
  br label %dynarray.store.53
dynarray.store.53:
  %t152 = load ptr, ptr %t139
  %t153 = getelementptr inbounds { ptr, i64 }, ptr %t152, i64 %t143
  store { ptr, i64 } %t120, ptr %t153
  store i64 %t145, ptr %t140
  %t154 = load ptr, ptr %local.3
  %t157 = alloca ptr
  %t158 = alloca i64
  %t159 = icmp eq ptr %t154, null
  br i1 %t159, label %dynarray.slice.empty.54, label %dynarray.slice.load.55
dynarray.slice.empty.54:
  store ptr null, ptr %t157
  store i64 0, ptr %t158
  br label %dynarray.slice.ready.56
dynarray.slice.load.55:
  %t160 = getelementptr inbounds { ptr, i64, i64 }, ptr %t154, i64 0, i32 0
  %t161 = load ptr, ptr %t160
  %t162 = getelementptr inbounds { ptr, i64, i64 }, ptr %t154, i64 0, i32 1
  %t163 = load i64, ptr %t162
  store ptr %t161, ptr %t157
  store i64 %t163, ptr %t158
  br label %dynarray.slice.ready.56
dynarray.slice.ready.56:
  %t164 = load i64, ptr %t158
  %t165 = load ptr, ptr %t157
  %t166 = getelementptr inbounds { ptr, i64 }, ptr %t165, i64 0
  %t167 = insertvalue { ptr, i64 } poison, ptr %t166, 0
  %t168 = insertvalue { ptr, i64 } %t167, i64 %t164, 1
  %t169 = extractvalue { ptr, i64 } %t168, 1
  %t170 = icmp ne i64 %t169, 5
  %t171 = icmp eq i1 %t170, 1
  br i1 %t171, label %on.body.58, label %on.end.57
on.body.58:
  ret i32 10
on.end.57:
  %t172 = call i64 @string_builder_mark()
  %t173 = extractvalue { ptr, i64 } %t168, 0
  %t174 = getelementptr inbounds { ptr, i64 }, ptr %t173, i32 0
  %t175 = load { ptr, i64 }, ptr %t174
  %t176 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t175)
  call void @string_builder_append_string({ ptr, i64 } %t176)
  %t177 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.8, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t177)
  %t178 = extractvalue { ptr, i64 } %t168, 0
  %t179 = getelementptr inbounds { ptr, i64 }, ptr %t178, i32 1
  %t180 = load { ptr, i64 }, ptr %t179
  %t181 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t180)
  call void @string_builder_append_string({ ptr, i64 } %t181)
  %t182 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.9, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t182)
  %t183 = extractvalue { ptr, i64 } %t168, 0
  %t184 = getelementptr inbounds { ptr, i64 }, ptr %t183, i32 2
  %t185 = load { ptr, i64 }, ptr %t184
  %t186 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t185)
  call void @string_builder_append_string({ ptr, i64 } %t186)
  %t187 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.10, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t187)
  %t188 = extractvalue { ptr, i64 } %t168, 0
  %t189 = getelementptr inbounds { ptr, i64 }, ptr %t188, i32 3
  %t190 = load { ptr, i64 }, ptr %t189
  %t191 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t190)
  call void @string_builder_append_string({ ptr, i64 } %t191)
  %t192 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.11, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t192)
  %t193 = extractvalue { ptr, i64 } %t168, 0
  %t194 = getelementptr inbounds { ptr, i64 }, ptr %t193, i32 4
  %t195 = load { ptr, i64 }, ptr %t194
  %t196 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t195)
  call void @string_builder_append_string({ ptr, i64 } %t196)
  %t197 = call { ptr, i64 } @string_builder_finish(i64 %t172)
  call void @$prn({ ptr, i64 } %t197)
  %t198 = load ptr, ptr %local.3
  %t199 = icmp eq ptr %t198, null
  br i1 %t199, label %dynarray.alloc.59, label %dynarray.ready.60
dynarray.alloc.59:
  %t200 = call ptr @malloc(i64 24)
  %t201 = getelementptr inbounds { ptr, i64, i64 }, ptr %t200, i64 0, i32 0
  %t202 = getelementptr inbounds { ptr, i64, i64 }, ptr %t200, i64 0, i32 1
  %t203 = getelementptr inbounds { ptr, i64, i64 }, ptr %t200, i64 0, i32 2
  store ptr null, ptr %t201
  store i64 0, ptr %t202
  store i64 0, ptr %t203
  store ptr %t200, ptr %local.3
  br label %dynarray.ready.60
dynarray.ready.60:
  %t204 = load ptr, ptr %local.3
  %t205 = getelementptr inbounds { ptr, i64, i64 }, ptr %t204, i64 0, i32 0
  %t206 = getelementptr inbounds { ptr, i64, i64 }, ptr %t204, i64 0, i32 2
  %t207 = load ptr, ptr %t205
  %t208 = load i64, ptr %t206
  %t209 = icmp ugt i64 10, %t208
  br i1 %t209, label %dynarray.reserve.grow.61, label %dynarray.reserve.done.62
dynarray.reserve.grow.61:
  %t210 = mul i64 10, 16
  %t211 = call ptr @realloc(ptr %t207, i64 %t210)
  store ptr %t211, ptr %t205
  store i64 10, ptr %t206
  br label %dynarray.reserve.done.62
dynarray.reserve.done.62:
  %t212 = load ptr, ptr %local.3
  %t213 = alloca i64
  %t214 = icmp eq ptr %t212, null
  br i1 %t214, label %dynarray.field.empty.63, label %dynarray.field.load.64
dynarray.field.empty.63:
  store i64 0, ptr %t213
  br label %dynarray.field.done.65
dynarray.field.load.64:
  %t215 = getelementptr inbounds { ptr, i64, i64 }, ptr %t212, i64 0, i32 2
  %t216 = load i64, ptr %t215
  store i64 %t216, ptr %t213
  br label %dynarray.field.done.65
dynarray.field.done.65:
  %t217 = load i64, ptr %t213
  %t218 = icmp slt i64 %t217, 10
  %t219 = icmp eq i1 %t218, 1
  br i1 %t219, label %on.body.67, label %on.end.66
on.body.67:
  ret i32 11
on.end.66:
  %t220 = call ptr @fn.0()
  %local.6 = alloca ptr
  store ptr %t220, ptr %local.6
  %t221 = load ptr, ptr %local.6
  %t222 = alloca i64
  %t223 = icmp eq ptr %t221, null
  br i1 %t223, label %dynarray.field.empty.68, label %dynarray.field.load.69
dynarray.field.empty.68:
  store i64 0, ptr %t222
  br label %dynarray.field.done.70
dynarray.field.load.69:
  %t224 = getelementptr inbounds { ptr, i64, i64 }, ptr %t221, i64 0, i32 1
  %t225 = load i64, ptr %t224
  store i64 %t225, ptr %t222
  br label %dynarray.field.done.70
dynarray.field.done.70:
  %t226 = load i64, ptr %t222
  %t227 = icmp ne i64 %t226, 2
  %t228 = icmp eq i1 %t227, 1
  br i1 %t228, label %on.body.72, label %on.end.71
on.body.72:
  ret i32 12
on.end.71:
  %t229 = call i64 @string_builder_mark()
  %t230 = load ptr, ptr %local.6
  %t231 = getelementptr inbounds { ptr, i64, i64 }, ptr %t230, i64 0, i32 0
  %t232 = load ptr, ptr %t231
  %t233 = getelementptr inbounds { ptr, i64 }, ptr %t232, i32 0
  %t234 = load { ptr, i64 }, ptr %t233
  %t235 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t234)
  call void @string_builder_append_string({ ptr, i64 } %t235)
  %t236 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.12, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t236)
  %t237 = load ptr, ptr %local.6
  %t238 = getelementptr inbounds { ptr, i64, i64 }, ptr %t237, i64 0, i32 0
  %t239 = load ptr, ptr %t238
  %t240 = getelementptr inbounds { ptr, i64 }, ptr %t239, i32 1
  %t241 = load { ptr, i64 }, ptr %t240
  %t242 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t241)
  call void @string_builder_append_string({ ptr, i64 } %t242)
  %t243 = call { ptr, i64 } @string_builder_finish(i64 %t229)
  call void @$prn({ ptr, i64 } %t243)
  %t244 = load ptr, ptr %local.6
  %t245 = icmp eq ptr %t244, null
  br i1 %t245, label %dynarray.free.done.74, label %dynarray.free.73
dynarray.free.73:
  %t246 = getelementptr inbounds { ptr, i64, i64 }, ptr %t244, i64 0, i32 0
  %t247 = load ptr, ptr %t246
  call void @free(ptr %t247)
  call void @free(ptr %t244)
  store ptr null, ptr %local.6
  br label %dynarray.free.done.74
dynarray.free.done.74:
  %local.7 = alloca ptr
  store ptr null, ptr %local.7
  %t248 = load ptr, ptr %local.7
  %t249 = icmp eq ptr %t248, null
  br i1 %t249, label %dynarray.alloc.75, label %dynarray.ready.76
dynarray.alloc.75:
  %t250 = call ptr @malloc(i64 24)
  %t251 = getelementptr inbounds { ptr, i64, i64 }, ptr %t250, i64 0, i32 0
  %t252 = getelementptr inbounds { ptr, i64, i64 }, ptr %t250, i64 0, i32 1
  %t253 = getelementptr inbounds { ptr, i64, i64 }, ptr %t250, i64 0, i32 2
  store ptr null, ptr %t251
  store i64 0, ptr %t252
  store i64 0, ptr %t253
  store ptr %t250, ptr %local.7
  br label %dynarray.ready.76
dynarray.ready.76:
  %t254 = load ptr, ptr %local.7
  %t255 = getelementptr inbounds { ptr, i64, i64 }, ptr %t254, i64 0, i32 0
  %t256 = getelementptr inbounds { ptr, i64, i64 }, ptr %t254, i64 0, i32 1
  %t257 = getelementptr inbounds { ptr, i64, i64 }, ptr %t254, i64 0, i32 2
  %t258 = load ptr, ptr %t255
  %t259 = load i64, ptr %t256
  %t260 = load i64, ptr %t257
  %t261 = add i64 %t259, 1
  %t262 = icmp ugt i64 %t261, %t260
  br i1 %t262, label %dynarray.grow.77, label %dynarray.store.78
dynarray.grow.77:
  %t263 = icmp eq i64 %t260, 0
  %t264 = mul i64 %t260, 2
  %t265 = select i1 %t263, i64 1, i64 %t264
  %t266 = mul i64 %t265, 4
  %t267 = call ptr @realloc(ptr %t258, i64 %t266)
  store ptr %t267, ptr %t255
  store i64 %t265, ptr %t257
  br label %dynarray.store.78
dynarray.store.78:
  %t268 = load ptr, ptr %t255
  %t269 = getelementptr inbounds i32, ptr %t268, i64 %t259
  store i32 1, ptr %t269
  store i64 %t261, ptr %t256
  %t270 = load ptr, ptr %local.7
  %t271 = icmp eq ptr %t270, null
  br i1 %t271, label %dynarray.alloc.79, label %dynarray.ready.80
dynarray.alloc.79:
  %t272 = call ptr @malloc(i64 24)
  %t273 = getelementptr inbounds { ptr, i64, i64 }, ptr %t272, i64 0, i32 0
  %t274 = getelementptr inbounds { ptr, i64, i64 }, ptr %t272, i64 0, i32 1
  %t275 = getelementptr inbounds { ptr, i64, i64 }, ptr %t272, i64 0, i32 2
  store ptr null, ptr %t273
  store i64 0, ptr %t274
  store i64 0, ptr %t275
  store ptr %t272, ptr %local.7
  br label %dynarray.ready.80
dynarray.ready.80:
  %t276 = load ptr, ptr %local.7
  %t277 = getelementptr inbounds { ptr, i64, i64 }, ptr %t276, i64 0, i32 0
  %t278 = getelementptr inbounds { ptr, i64, i64 }, ptr %t276, i64 0, i32 1
  %t279 = getelementptr inbounds { ptr, i64, i64 }, ptr %t276, i64 0, i32 2
  %t280 = load ptr, ptr %t277
  %t281 = load i64, ptr %t278
  %t282 = load i64, ptr %t279
  %t283 = add i64 %t281, 1
  %t284 = icmp ugt i64 %t283, %t282
  br i1 %t284, label %dynarray.grow.81, label %dynarray.store.82
dynarray.grow.81:
  %t285 = icmp eq i64 %t282, 0
  %t286 = mul i64 %t282, 2
  %t287 = select i1 %t285, i64 1, i64 %t286
  %t288 = mul i64 %t287, 4
  %t289 = call ptr @realloc(ptr %t280, i64 %t288)
  store ptr %t289, ptr %t277
  store i64 %t287, ptr %t279
  br label %dynarray.store.82
dynarray.store.82:
  %t290 = load ptr, ptr %t277
  %t291 = getelementptr inbounds i32, ptr %t290, i64 %t281
  store i32 2, ptr %t291
  store i64 %t283, ptr %t278
  %t292 = load ptr, ptr %local.7
  %t293 = icmp eq ptr %t292, null
  br i1 %t293, label %dynarray.alloc.83, label %dynarray.ready.84
dynarray.alloc.83:
  %t294 = call ptr @malloc(i64 24)
  %t295 = getelementptr inbounds { ptr, i64, i64 }, ptr %t294, i64 0, i32 0
  %t296 = getelementptr inbounds { ptr, i64, i64 }, ptr %t294, i64 0, i32 1
  %t297 = getelementptr inbounds { ptr, i64, i64 }, ptr %t294, i64 0, i32 2
  store ptr null, ptr %t295
  store i64 0, ptr %t296
  store i64 0, ptr %t297
  store ptr %t294, ptr %local.7
  br label %dynarray.ready.84
dynarray.ready.84:
  %t298 = load ptr, ptr %local.7
  %t299 = getelementptr inbounds { ptr, i64, i64 }, ptr %t298, i64 0, i32 0
  %t300 = getelementptr inbounds { ptr, i64, i64 }, ptr %t298, i64 0, i32 1
  %t301 = getelementptr inbounds { ptr, i64, i64 }, ptr %t298, i64 0, i32 2
  %t302 = load ptr, ptr %t299
  %t303 = load i64, ptr %t300
  %t304 = load i64, ptr %t301
  %t305 = add i64 %t303, 1
  %t306 = icmp ugt i64 %t305, %t304
  br i1 %t306, label %dynarray.grow.85, label %dynarray.store.86
dynarray.grow.85:
  %t307 = icmp eq i64 %t304, 0
  %t308 = mul i64 %t304, 2
  %t309 = select i1 %t307, i64 1, i64 %t308
  %t310 = mul i64 %t309, 4
  %t311 = call ptr @realloc(ptr %t302, i64 %t310)
  store ptr %t311, ptr %t299
  store i64 %t309, ptr %t301
  br label %dynarray.store.86
dynarray.store.86:
  %t312 = load ptr, ptr %t299
  %t313 = getelementptr inbounds i32, ptr %t312, i64 %t303
  store i32 3, ptr %t313
  store i64 %t305, ptr %t300
  %t314 = load ptr, ptr %local.7
  %t315 = icmp eq ptr %t314, null
  br i1 %t315, label %dynarray.alloc.87, label %dynarray.ready.88
dynarray.alloc.87:
  %t316 = call ptr @malloc(i64 24)
  %t317 = getelementptr inbounds { ptr, i64, i64 }, ptr %t316, i64 0, i32 0
  %t318 = getelementptr inbounds { ptr, i64, i64 }, ptr %t316, i64 0, i32 1
  %t319 = getelementptr inbounds { ptr, i64, i64 }, ptr %t316, i64 0, i32 2
  store ptr null, ptr %t317
  store i64 0, ptr %t318
  store i64 0, ptr %t319
  store ptr %t316, ptr %local.7
  br label %dynarray.ready.88
dynarray.ready.88:
  %t320 = load ptr, ptr %local.7
  %t321 = getelementptr inbounds { ptr, i64, i64 }, ptr %t320, i64 0, i32 0
  %t322 = getelementptr inbounds { ptr, i64, i64 }, ptr %t320, i64 0, i32 1
  %t323 = getelementptr inbounds { ptr, i64, i64 }, ptr %t320, i64 0, i32 2
  %t324 = load ptr, ptr %t321
  %t325 = load i64, ptr %t322
  %t326 = load i64, ptr %t323
  %t327 = add i64 %t325, 1
  %t328 = icmp ugt i64 %t327, %t326
  br i1 %t328, label %dynarray.grow.89, label %dynarray.store.90
dynarray.grow.89:
  %t329 = icmp eq i64 %t326, 0
  %t330 = mul i64 %t326, 2
  %t331 = select i1 %t329, i64 1, i64 %t330
  %t332 = mul i64 %t331, 4
  %t333 = call ptr @realloc(ptr %t324, i64 %t332)
  store ptr %t333, ptr %t321
  store i64 %t331, ptr %t323
  br label %dynarray.store.90
dynarray.store.90:
  %t334 = load ptr, ptr %t321
  %t335 = getelementptr inbounds i32, ptr %t334, i64 %t325
  store i32 4, ptr %t335
  store i64 %t327, ptr %t322
  %t336 = load ptr, ptr %local.7
  %t337 = icmp eq ptr %t336, null
  br i1 %t337, label %dynarray.alloc.91, label %dynarray.ready.92
dynarray.alloc.91:
  %t338 = call ptr @malloc(i64 24)
  %t339 = getelementptr inbounds { ptr, i64, i64 }, ptr %t338, i64 0, i32 0
  %t340 = getelementptr inbounds { ptr, i64, i64 }, ptr %t338, i64 0, i32 1
  %t341 = getelementptr inbounds { ptr, i64, i64 }, ptr %t338, i64 0, i32 2
  store ptr null, ptr %t339
  store i64 0, ptr %t340
  store i64 0, ptr %t341
  store ptr %t338, ptr %local.7
  br label %dynarray.ready.92
dynarray.ready.92:
  %t342 = load ptr, ptr %local.7
  %t343 = getelementptr inbounds { ptr, i64, i64 }, ptr %t342, i64 0, i32 0
  %t344 = getelementptr inbounds { ptr, i64, i64 }, ptr %t342, i64 0, i32 1
  %t345 = getelementptr inbounds { ptr, i64, i64 }, ptr %t342, i64 0, i32 2
  %t346 = load ptr, ptr %t343
  %t347 = load i64, ptr %t344
  %t348 = load i64, ptr %t345
  %t349 = add i64 %t347, 1
  %t350 = icmp ugt i64 %t349, %t348
  br i1 %t350, label %dynarray.grow.93, label %dynarray.store.94
dynarray.grow.93:
  %t351 = icmp eq i64 %t348, 0
  %t352 = mul i64 %t348, 2
  %t353 = select i1 %t351, i64 1, i64 %t352
  %t354 = mul i64 %t353, 4
  %t355 = call ptr @realloc(ptr %t346, i64 %t354)
  store ptr %t355, ptr %t343
  store i64 %t353, ptr %t345
  br label %dynarray.store.94
dynarray.store.94:
  %t356 = load ptr, ptr %t343
  %t357 = getelementptr inbounds i32, ptr %t356, i64 %t347
  store i32 5, ptr %t357
  store i64 %t349, ptr %t344
  %t358 = load ptr, ptr %local.7
  %t359 = getelementptr inbounds { ptr, i64, i64 }, ptr %t358, i64 0, i32 0
  %t360 = load ptr, ptr %t359
  %t361 = getelementptr inbounds { ptr, i64, i64 }, ptr %t358, i64 0, i32 1
  %t362 = load i64, ptr %t361
  %t363 = sub i64 %t362, 1
  %t364 = alloca i64
  store i64 1, ptr %t364
  br label %dynarray.delete.loop.95
dynarray.delete.loop.95:
  %t365 = load i64, ptr %t364
  %t366 = icmp ult i64 %t365, %t363
  br i1 %t366, label %dynarray.delete.body.96, label %dynarray.delete.done.97
dynarray.delete.body.96:
  %t367 = add i64 %t365, 1
  %t368 = getelementptr inbounds i32, ptr %t360, i64 %t367
  %t369 = getelementptr inbounds i32, ptr %t360, i64 %t365
  %t370 = load i32, ptr %t368
  store i32 %t370, ptr %t369
  store i64 %t367, ptr %t364
  br label %dynarray.delete.loop.95
dynarray.delete.done.97:
  store i64 %t363, ptr %t361
  %t371 = load ptr, ptr %local.7
  %t372 = alloca i64
  %t373 = icmp eq ptr %t371, null
  br i1 %t373, label %dynarray.field.empty.98, label %dynarray.field.load.99
dynarray.field.empty.98:
  store i64 0, ptr %t372
  br label %dynarray.field.done.100
dynarray.field.load.99:
  %t374 = getelementptr inbounds { ptr, i64, i64 }, ptr %t371, i64 0, i32 1
  %t375 = load i64, ptr %t374
  store i64 %t375, ptr %t372
  br label %dynarray.field.done.100
dynarray.field.done.100:
  %t376 = load i64, ptr %t372
  %t377 = icmp ne i64 %t376, 4
  %t378 = icmp eq i1 %t377, 1
  br i1 %t378, label %on.body.102, label %on.end.101
on.body.102:
  ret i32 13
on.end.101:
  %t379 = load ptr, ptr %local.7
  %t380 = getelementptr inbounds { ptr, i64, i64 }, ptr %t379, i64 0, i32 0
  %t381 = load ptr, ptr %t380
  %t382 = getelementptr inbounds i32, ptr %t381, i32 0
  %t383 = load i32, ptr %t382
  %t384 = icmp ne i32 %t383, 1
  %t385 = icmp eq i1 %t384, 1
  br i1 %t385, label %on.body.104, label %on.end.103
on.body.104:
  ret i32 14
on.end.103:
  %t386 = load ptr, ptr %local.7
  %t387 = getelementptr inbounds { ptr, i64, i64 }, ptr %t386, i64 0, i32 0
  %t388 = load ptr, ptr %t387
  %t389 = getelementptr inbounds i32, ptr %t388, i32 1
  %t390 = load i32, ptr %t389
  %t391 = icmp ne i32 %t390, 3
  %t392 = icmp eq i1 %t391, 1
  br i1 %t392, label %on.body.106, label %on.end.105
on.body.106:
  ret i32 15
on.end.105:
  %t393 = load ptr, ptr %local.7
  %t394 = getelementptr inbounds { ptr, i64, i64 }, ptr %t393, i64 0, i32 0
  %t395 = load ptr, ptr %t394
  %t396 = getelementptr inbounds i32, ptr %t395, i32 2
  %t397 = load i32, ptr %t396
  %t398 = icmp ne i32 %t397, 4
  %t399 = icmp eq i1 %t398, 1
  br i1 %t399, label %on.body.108, label %on.end.107
on.body.108:
  ret i32 16
on.end.107:
  %t400 = load ptr, ptr %local.7
  %t401 = getelementptr inbounds { ptr, i64, i64 }, ptr %t400, i64 0, i32 0
  %t402 = load ptr, ptr %t401
  %t403 = getelementptr inbounds i32, ptr %t402, i32 3
  %t404 = load i32, ptr %t403
  %t405 = icmp ne i32 %t404, 5
  %t406 = icmp eq i1 %t405, 1
  br i1 %t406, label %on.body.110, label %on.end.109
on.body.110:
  ret i32 17
on.end.109:
  %t407 = load ptr, ptr %local.7
  %t408 = getelementptr inbounds { ptr, i64, i64 }, ptr %t407, i64 0, i32 0
  %t409 = load ptr, ptr %t408
  %t410 = getelementptr inbounds { ptr, i64, i64 }, ptr %t407, i64 0, i32 1
  %t411 = load i64, ptr %t410
  %t412 = sub i64 %t411, 1
  %t413 = getelementptr inbounds i32, ptr %t409, i64 %t412
  %t414 = getelementptr inbounds i32, ptr %t409, i64 1
  %t415 = load i32, ptr %t413
  store i32 %t415, ptr %t414
  store i64 %t412, ptr %t410
  %t416 = load ptr, ptr %local.7
  %t417 = alloca i64
  %t418 = icmp eq ptr %t416, null
  br i1 %t418, label %dynarray.field.empty.111, label %dynarray.field.load.112
dynarray.field.empty.111:
  store i64 0, ptr %t417
  br label %dynarray.field.done.113
dynarray.field.load.112:
  %t419 = getelementptr inbounds { ptr, i64, i64 }, ptr %t416, i64 0, i32 1
  %t420 = load i64, ptr %t419
  store i64 %t420, ptr %t417
  br label %dynarray.field.done.113
dynarray.field.done.113:
  %t421 = load i64, ptr %t417
  %t422 = icmp ne i64 %t421, 3
  %t423 = icmp eq i1 %t422, 1
  br i1 %t423, label %on.body.115, label %on.end.114
on.body.115:
  ret i32 18
on.end.114:
  %t424 = load ptr, ptr %local.7
  %t425 = getelementptr inbounds { ptr, i64, i64 }, ptr %t424, i64 0, i32 0
  %t426 = load ptr, ptr %t425
  %t427 = getelementptr inbounds i32, ptr %t426, i32 0
  %t428 = load i32, ptr %t427
  %t429 = icmp ne i32 %t428, 1
  %t430 = icmp eq i1 %t429, 1
  br i1 %t430, label %on.body.117, label %on.end.116
on.body.117:
  ret i32 19
on.end.116:
  %t431 = load ptr, ptr %local.7
  %t432 = getelementptr inbounds { ptr, i64, i64 }, ptr %t431, i64 0, i32 0
  %t433 = load ptr, ptr %t432
  %t434 = getelementptr inbounds i32, ptr %t433, i32 1
  %t435 = load i32, ptr %t434
  %t436 = icmp ne i32 %t435, 5
  %t437 = icmp eq i1 %t436, 1
  br i1 %t437, label %on.body.119, label %on.end.118
on.body.119:
  ret i32 20
on.end.118:
  %t438 = load ptr, ptr %local.7
  %t439 = getelementptr inbounds { ptr, i64, i64 }, ptr %t438, i64 0, i32 0
  %t440 = load ptr, ptr %t439
  %t441 = getelementptr inbounds i32, ptr %t440, i32 2
  %t442 = load i32, ptr %t441
  %t443 = icmp ne i32 %t442, 4
  %t444 = icmp eq i1 %t443, 1
  br i1 %t444, label %on.body.121, label %on.end.120
on.body.121:
  ret i32 21
on.end.120:
  %t445 = load ptr, ptr %local.7
  %t446 = icmp eq ptr %t445, null
  br i1 %t446, label %dynarray.free.done.123, label %dynarray.free.122
dynarray.free.122:
  %t447 = getelementptr inbounds { ptr, i64, i64 }, ptr %t445, i64 0, i32 0
  %t448 = load ptr, ptr %t447
  call void @free(ptr %t448)
  call void @free(ptr %t445)
  store ptr null, ptr %local.7
  br label %dynarray.free.done.123
dynarray.free.done.123:
  %t449 = load ptr, ptr %local.3
  %t450 = icmp eq ptr %t449, null
  br i1 %t450, label %dynarray.clear.done.125, label %dynarray.clear.124
dynarray.clear.124:
  %t451 = getelementptr inbounds { ptr, i64, i64 }, ptr %t449, i64 0, i32 1
  store i64 0, ptr %t451
  br label %dynarray.clear.done.125
dynarray.clear.done.125:
  %t452 = load ptr, ptr %local.3
  %t453 = alloca i64
  %t454 = icmp eq ptr %t452, null
  br i1 %t454, label %dynarray.field.empty.126, label %dynarray.field.load.127
dynarray.field.empty.126:
  store i64 0, ptr %t453
  br label %dynarray.field.done.128
dynarray.field.load.127:
  %t455 = getelementptr inbounds { ptr, i64, i64 }, ptr %t452, i64 0, i32 1
  %t456 = load i64, ptr %t455
  store i64 %t456, ptr %t453
  br label %dynarray.field.done.128
dynarray.field.done.128:
  %t457 = load i64, ptr %t453
  %t458 = icmp ne i64 %t457, 0
  %t459 = icmp eq i1 %t458, 1
  br i1 %t459, label %on.body.130, label %on.end.129
on.body.130:
  ret i32 22
on.end.129:
  %t460 = load ptr, ptr %local.3
  %t461 = icmp eq ptr %t460, null
  br i1 %t461, label %dynarray.free.done.132, label %dynarray.free.131
dynarray.free.131:
  %t462 = getelementptr inbounds { ptr, i64, i64 }, ptr %t460, i64 0, i32 0
  %t463 = load ptr, ptr %t462
  call void @free(ptr %t463)
  call void @free(ptr %t460)
  store ptr null, ptr %local.3
  br label %dynarray.free.done.132
dynarray.free.done.132:
  %t464 = load ptr, ptr %local.3
  %t465 = icmp ne ptr %t464, null
  %t466 = icmp eq i1 %t465, 1
  br i1 %t466, label %on.body.134, label %on.end.133
on.body.134:
  ret i32 23
on.end.133:
  ret i32 0
}

@$make_words = internal alias ptr (), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
