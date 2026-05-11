use std.io

Direction :: enum {
    NORTH
    EAST
    SOUTH
    WEST
    NO_DIRECTION
}

direction_from_word :: fn (word: string) -> Direction {
    return on word {
        "north", "n" => NORTH
        "east",  "e" => EAST
        "south", "s" => SOUTH
        "west",  "w" => WEST
        else => NO_DIRECTION
    }
}

default_direction :: fn () -> Direction {
    return on yes => NORTH else NO_DIRECTION
}

direction_name :: fn (direction: Direction) -> string {
    return on direction {
        NORTH => "north"
        EAST => "east"
        SOUTH => "south"
        WEST => "west"
        else => "none"
    }
}

main :: fn () -> i32 {
    prn(direction_name(direction_from_word("n")))
    prn(direction_name(default_direction()))
    return 0
}
¬
0
¬
north
north

¬
hir 0
module module.0(083-explicit-return-enum-context.input)
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
bind Direction = type.0
bind direction_from_word = fn.0
bind default_direction = fn.1
bind direction_name = fn.2
bind main = fn.3
type type.0 = Direction
func fn.0(word: string) -> Direction {
  return Direction on string local.0(word) {
    value(string "north"), value(string "n") => {
      expr Direction NORTH
    }
    value(string "east"), value(string "e") => {
      expr Direction EAST
    }
    value(string "south"), value(string "s") => {
      expr Direction SOUTH
    }
    value(string "west"), value(string "w") => {
      expr Direction WEST
    }
    else => {
      expr Direction NO_DIRECTION
    }
  }
}
func fn.1() -> Direction {
  return Direction on bool yes {
    value(bool yes) => {
      expr Direction NORTH
    }
    else => {
      expr Direction NO_DIRECTION
    }
  }
}
func fn.2(direction: Direction) -> string {
  return string on Direction local.1(direction) {
    value(Direction NORTH) => {
      expr string "north"
    }
    value(Direction EAST) => {
      expr string "east"
    }
    value(Direction SOUTH) => {
      expr string "south"
    }
    value(Direction WEST) => {
      expr string "west"
    }
    else => {
      expr string "none"
    }
  }
}
func fn.3() -> i32 {
  expr void call bind.2(prn)(string call bind.8(direction_name)(Direction call bind.6(direction_from_word)(string "n")))
  expr void call bind.2(prn)(string call bind.8(direction_name)(Direction call bind.7(default_direction)()))
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [6 x i8] c"north\00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c"n\00"
@.str.m0.2 = private unnamed_addr constant [5 x i8] c"east\00"
@.str.m0.3 = private unnamed_addr constant [2 x i8] c"e\00"
@.str.m0.4 = private unnamed_addr constant [6 x i8] c"south\00"
@.str.m0.5 = private unnamed_addr constant [2 x i8] c"s\00"
@.str.m0.6 = private unnamed_addr constant [5 x i8] c"west\00"
@.str.m0.7 = private unnamed_addr constant [2 x i8] c"w\00"
@.str.m0.8 = private unnamed_addr constant [6 x i8] c"north\00"
@.str.m0.9 = private unnamed_addr constant [5 x i8] c"east\00"
@.str.m0.10 = private unnamed_addr constant [6 x i8] c"south\00"
@.str.m0.11 = private unnamed_addr constant [5 x i8] c"west\00"
@.str.m0.12 = private unnamed_addr constant [5 x i8] c"none\00"
@.str.m0.13 = private unnamed_addr constant [2 x i8] c"n\00"

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

define internal { i64, i64 } @fn.0({ ptr, i64 } %word) {
  %t0 = alloca { ptr, i64 }
  store { ptr, i64 } %word, ptr %t0
  %t1 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 5 }, ptr %t1
  %t2 = call i1 @string_eq(ptr %t0, ptr %t1)
  %t3 = alloca { ptr, i64 }
  store { ptr, i64 } %word, ptr %t3
  %t4 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 1 }, ptr %t4
  %t5 = call i1 @string_eq(ptr %t3, ptr %t4)
  %t6 = or i1 %t2, %t5
  br i1 %t6, label %on.body.1, label %on.next.2
on.body.1:
  %t7 = insertvalue { i64, i64 } poison, i64 0, 0
  %t8 = insertvalue { i64, i64 } %t7, i64 0, 1
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t9 = alloca { ptr, i64 }
  store { ptr, i64 } %word, ptr %t9
  %t10 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 4 }, ptr %t10
  %t11 = call i1 @string_eq(ptr %t9, ptr %t10)
  %t12 = alloca { ptr, i64 }
  store { ptr, i64 } %word, ptr %t12
  %t13 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 1 }, ptr %t13
  %t14 = call i1 @string_eq(ptr %t12, ptr %t13)
  %t15 = or i1 %t11, %t14
  br i1 %t15, label %on.body.4, label %on.next.5
on.body.4:
  %t16 = insertvalue { i64, i64 } poison, i64 1, 0
  %t17 = insertvalue { i64, i64 } %t16, i64 0, 1
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  %t18 = alloca { ptr, i64 }
  store { ptr, i64 } %word, ptr %t18
  %t19 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 5 }, ptr %t19
  %t20 = call i1 @string_eq(ptr %t18, ptr %t19)
  %t21 = alloca { ptr, i64 }
  store { ptr, i64 } %word, ptr %t21
  %t22 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.5, i64 1 }, ptr %t22
  %t23 = call i1 @string_eq(ptr %t21, ptr %t22)
  %t24 = or i1 %t20, %t23
  br i1 %t24, label %on.body.7, label %on.next.8
on.body.7:
  %t25 = insertvalue { i64, i64 } poison, i64 2, 0
  %t26 = insertvalue { i64, i64 } %t25, i64 0, 1
  br label %on.value.9
on.value.9:
  br label %on.end.0
on.next.8:
  %t27 = alloca { ptr, i64 }
  store { ptr, i64 } %word, ptr %t27
  %t28 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.6, i64 4 }, ptr %t28
  %t29 = call i1 @string_eq(ptr %t27, ptr %t28)
  %t30 = alloca { ptr, i64 }
  store { ptr, i64 } %word, ptr %t30
  %t31 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.7, i64 1 }, ptr %t31
  %t32 = call i1 @string_eq(ptr %t30, ptr %t31)
  %t33 = or i1 %t29, %t32
  br i1 %t33, label %on.body.10, label %on.next.11
on.body.10:
  %t34 = insertvalue { i64, i64 } poison, i64 3, 0
  %t35 = insertvalue { i64, i64 } %t34, i64 0, 1
  br label %on.value.12
on.value.12:
  br label %on.end.0
on.next.11:
  br label %on.body.13
on.body.13:
  %t36 = insertvalue { i64, i64 } poison, i64 4, 0
  %t37 = insertvalue { i64, i64 } %t36, i64 0, 1
  br label %on.value.15
on.value.15:
  br label %on.end.0
on.end.0:
  %t38 = phi { i64, i64 } [%t8, %on.value.3], [%t17, %on.value.6], [%t26, %on.value.9], [%t35, %on.value.12], [%t37, %on.value.15]
  ret { i64, i64 } %t38
}

define internal { i64, i64 } @fn.1() {
  %t0 = icmp eq i1 1, 1
  br i1 %t0, label %on.body.1, label %on.next.2
on.body.1:
  %t1 = insertvalue { i64, i64 } poison, i64 0, 0
  %t2 = insertvalue { i64, i64 } %t1, i64 0, 1
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  br label %on.body.4
on.body.4:
  %t3 = insertvalue { i64, i64 } poison, i64 4, 0
  %t4 = insertvalue { i64, i64 } %t3, i64 0, 1
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.end.0:
  %t5 = phi { i64, i64 } [%t2, %on.value.3], [%t4, %on.value.6]
  ret { i64, i64 } %t5
}

define internal { ptr, i64 } @fn.2({ i64, i64 } %direction) {
  %t0 = insertvalue { i64, i64 } poison, i64 0, 0
  %t1 = insertvalue { i64, i64 } %t0, i64 0, 1
  %t2 = extractvalue { i64, i64 } %direction, 0
  %t3 = extractvalue { i64, i64 } %t1, 0
  %t4 = icmp eq i64 %t2, %t3
  br i1 %t4, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t5 = insertvalue { i64, i64 } poison, i64 1, 0
  %t6 = insertvalue { i64, i64 } %t5, i64 0, 1
  %t7 = extractvalue { i64, i64 } %direction, 0
  %t8 = extractvalue { i64, i64 } %t6, 0
  %t9 = icmp eq i64 %t7, %t8
  br i1 %t9, label %on.body.4, label %on.next.5
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  %t10 = insertvalue { i64, i64 } poison, i64 2, 0
  %t11 = insertvalue { i64, i64 } %t10, i64 0, 1
  %t12 = extractvalue { i64, i64 } %direction, 0
  %t13 = extractvalue { i64, i64 } %t11, 0
  %t14 = icmp eq i64 %t12, %t13
  br i1 %t14, label %on.body.7, label %on.next.8
on.body.7:
  br label %on.value.9
on.value.9:
  br label %on.end.0
on.next.8:
  %t15 = insertvalue { i64, i64 } poison, i64 3, 0
  %t16 = insertvalue { i64, i64 } %t15, i64 0, 1
  %t17 = extractvalue { i64, i64 } %direction, 0
  %t18 = extractvalue { i64, i64 } %t16, 0
  %t19 = icmp eq i64 %t17, %t18
  br i1 %t19, label %on.body.10, label %on.next.11
on.body.10:
  br label %on.value.12
on.value.12:
  br label %on.end.0
on.next.11:
  br label %on.body.13
on.body.13:
  br label %on.value.15
on.value.15:
  br label %on.end.0
on.end.0:
  %t20 = phi { ptr, i64 } [{ ptr @.str.m0.8, i64 5 }, %on.value.3], [{ ptr @.str.m0.9, i64 4 }, %on.value.6], [{ ptr @.str.m0.10, i64 5 }, %on.value.9], [{ ptr @.str.m0.11, i64 4 }, %on.value.12], [{ ptr @.str.m0.12, i64 4 }, %on.value.15]
  ret { ptr, i64 } %t20
}

define internal i32 @fn.3() {
  %t0 = call { i64, i64 } @fn.0({ ptr, i64 } { ptr @.str.m0.13, i64 1 })
  %t1 = call { ptr, i64 } @fn.2({ i64, i64 } %t0)
  call void @$prn({ ptr, i64 } %t1)
  %t2 = call { i64, i64 } @fn.1()
  %t3 = call { ptr, i64 } @fn.2({ i64, i64 } %t2)
  call void @$prn({ ptr, i64 } %t3)
  ret i32 0
}

@$direction_from_word = internal alias { i64, i64 } ({ ptr, i64 }), ptr @fn.0
@$default_direction = internal alias { i64, i64 } (), ptr @fn.1
@$direction_name = internal alias { ptr, i64 } ({ i64, i64 }), ptr @fn.2
@$main = alias i32 (), ptr @fn.3
