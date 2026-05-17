use std.io

Direction :: enum {
    NORTH,
    EAST,
    SOUTH = 10,
    WEST,
    COUNT
}

Token :: enum {
    EOF = 0,
    IDENT,
    NUMBER = 10,
    STRING,
}

labels : [Direction.COUNT]string = [
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"
]

describe_direction :: fn (direction: Direction) -> string {
    return on direction {
        NORTH => "north"
        EAST => "east"
        SOUTH => "south"
        WEST => "west"
        COUNT => "count"
    }
}

describe_token :: fn (token: Token) -> string {
    return on token {
        EOF => "eof"
        IDENT => "ident"
        NUMBER => "number"
        STRING => "string"
    }
}

main :: fn () -> i32 {
    south : Direction = Direction.SOUTH
    string_token : Token = Token.STRING

    prn(labels[0])
    prn(labels[11])
    prn(describe_direction(south))
    prn(describe_token(string_token))

    ok := on south {
        SOUTH => yes
        else => no
    } && on string_token {
        STRING => yes
        else => no
    }

    return on ok => 0 else 1
}
¬
0
¬
0
11
south
string

¬
hir 0
module module.0(077-enum-discriminants.input)
import module.1(std.io)
import import.0 prn from module.2(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.5: fn (string) -> string
bind prn = import.0
bind input = import.1
bind Direction = type.0
bind Token = type.1
bind labels = value.0
bind describe_direction = fn.0
bind describe_token = fn.1
bind main = fn.2
type type.0 = Direction
type type.1 = Token
global value.0: [12]string = [12]string array(string "0", string "1", string "2", string "3", string "4", string "5", string "6", string "7", string "8", string "9", string "10", string "11")
func fn.0(direction: Direction) -> string {
  return string on Direction local.0(direction) {
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
    value(Direction COUNT) => {
      expr string "count"
    }
  }
}
func fn.1(token: Token) -> string {
  return string on Token local.1(token) {
    value(Token EOF) => {
      expr string "eof"
    }
    value(Token IDENT) => {
      expr string "ident"
    }
    value(Token NUMBER) => {
      expr string "number"
    }
    value(Token STRING) => {
      expr string "string"
    }
  }
}
func fn.2() -> i32 {
  let south: Direction = Direction field(Direction bind.2(Direction), SOUTH)
  let string_token: Token = Token field(Token bind.3(Token), STRING)
  expr void call bind.0(prn)(string index([12]string bind.4(labels), untyped integer 0))
  expr void call bind.0(prn)(string index([12]string bind.4(labels), untyped integer 11))
  expr void call bind.0(prn)(string call bind.5(describe_direction)(Direction local.2(south)))
  expr void call bind.0(prn)(string call bind.6(describe_token)(Token local.3(string_token)))
  let ok: bool = bool logical_and(bool on Direction local.2(south) {
    value(Direction SOUTH) => {
      expr bool yes
    }
    else => {
      expr bool no
    }
  }, bool on Token local.3(string_token) {
    value(Token STRING) => {
      expr bool yes
    }
    else => {
      expr bool no
    }
  })
  return i32 on bool local.4(ok) {
    value(bool yes) => {
      expr i32 0
    }
    else => {
      expr i32 1
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [2 x i8] c"0\00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c"1\00"
@.str.m0.2 = private unnamed_addr constant [2 x i8] c"2\00"
@.str.m0.3 = private unnamed_addr constant [2 x i8] c"3\00"
@.str.m0.4 = private unnamed_addr constant [2 x i8] c"4\00"
@.str.m0.5 = private unnamed_addr constant [2 x i8] c"5\00"
@.str.m0.6 = private unnamed_addr constant [2 x i8] c"6\00"
@.str.m0.7 = private unnamed_addr constant [2 x i8] c"7\00"
@.str.m0.8 = private unnamed_addr constant [2 x i8] c"8\00"
@.str.m0.9 = private unnamed_addr constant [2 x i8] c"9\00"
@.str.m0.10 = private unnamed_addr constant [3 x i8] c"10\00"
@.str.m0.11 = private unnamed_addr constant [3 x i8] c"11\00"
@.str.m0.12 = private unnamed_addr constant [6 x i8] c"north\00"
@.str.m0.13 = private unnamed_addr constant [5 x i8] c"east\00"
@.str.m0.14 = private unnamed_addr constant [6 x i8] c"south\00"
@.str.m0.15 = private unnamed_addr constant [5 x i8] c"west\00"
@.str.m0.16 = private unnamed_addr constant [6 x i8] c"count\00"
@.str.m0.17 = private unnamed_addr constant [4 x i8] c"eof\00"
@.str.m0.18 = private unnamed_addr constant [6 x i8] c"ident\00"
@.str.m0.19 = private unnamed_addr constant [7 x i8] c"number\00"
@.str.m0.20 = private unnamed_addr constant [7 x i8] c"string\00"

declare void @$prn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

@$labels = internal global [12 x { ptr, i64 }] zeroinitializer

define void @m0.init() {
  %t0 = insertvalue [12 x { ptr, i64 }] poison, { ptr, i64 } { ptr @.str.m0.0, i64 1 }, 0
  %t1 = insertvalue [12 x { ptr, i64 }] %t0, { ptr, i64 } { ptr @.str.m0.1, i64 1 }, 1
  %t2 = insertvalue [12 x { ptr, i64 }] %t1, { ptr, i64 } { ptr @.str.m0.2, i64 1 }, 2
  %t3 = insertvalue [12 x { ptr, i64 }] %t2, { ptr, i64 } { ptr @.str.m0.3, i64 1 }, 3
  %t4 = insertvalue [12 x { ptr, i64 }] %t3, { ptr, i64 } { ptr @.str.m0.4, i64 1 }, 4
  %t5 = insertvalue [12 x { ptr, i64 }] %t4, { ptr, i64 } { ptr @.str.m0.5, i64 1 }, 5
  %t6 = insertvalue [12 x { ptr, i64 }] %t5, { ptr, i64 } { ptr @.str.m0.6, i64 1 }, 6
  %t7 = insertvalue [12 x { ptr, i64 }] %t6, { ptr, i64 } { ptr @.str.m0.7, i64 1 }, 7
  %t8 = insertvalue [12 x { ptr, i64 }] %t7, { ptr, i64 } { ptr @.str.m0.8, i64 1 }, 8
  %t9 = insertvalue [12 x { ptr, i64 }] %t8, { ptr, i64 } { ptr @.str.m0.9, i64 1 }, 9
  %t10 = insertvalue [12 x { ptr, i64 }] %t9, { ptr, i64 } { ptr @.str.m0.10, i64 2 }, 10
  %t11 = insertvalue [12 x { ptr, i64 }] %t10, { ptr, i64 } { ptr @.str.m0.11, i64 2 }, 11
  store [12 x { ptr, i64 }] %t11, ptr @$labels
  ret void
}

define internal { ptr, i64 } @fn.0({ i64, i64 } %direction) {
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
  %t10 = insertvalue { i64, i64 } poison, i64 10, 0
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
  %t15 = insertvalue { i64, i64 } poison, i64 11, 0
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
  %t20 = insertvalue { i64, i64 } poison, i64 12, 0
  %t21 = insertvalue { i64, i64 } %t20, i64 0, 1
  %t22 = extractvalue { i64, i64 } %direction, 0
  %t23 = extractvalue { i64, i64 } %t21, 0
  %t24 = icmp eq i64 %t22, %t23
  br i1 %t24, label %on.body.13, label %on.next.14
on.body.13:
  br label %on.value.15
on.value.15:
  br label %on.end.0
on.next.14:
  unreachable
on.end.0:
  %t25 = phi { ptr, i64 } [{ ptr @.str.m0.12, i64 5 }, %on.value.3], [{ ptr @.str.m0.13, i64 4 }, %on.value.6], [{ ptr @.str.m0.14, i64 5 }, %on.value.9], [{ ptr @.str.m0.15, i64 4 }, %on.value.12], [{ ptr @.str.m0.16, i64 5 }, %on.value.15]
  ret { ptr, i64 } %t25
}

define internal { ptr, i64 } @fn.1({ i64, i64 } %token) {
  %t0 = insertvalue { i64, i64 } poison, i64 0, 0
  %t1 = insertvalue { i64, i64 } %t0, i64 0, 1
  %t2 = extractvalue { i64, i64 } %token, 0
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
  %t7 = extractvalue { i64, i64 } %token, 0
  %t8 = extractvalue { i64, i64 } %t6, 0
  %t9 = icmp eq i64 %t7, %t8
  br i1 %t9, label %on.body.4, label %on.next.5
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  %t10 = insertvalue { i64, i64 } poison, i64 10, 0
  %t11 = insertvalue { i64, i64 } %t10, i64 0, 1
  %t12 = extractvalue { i64, i64 } %token, 0
  %t13 = extractvalue { i64, i64 } %t11, 0
  %t14 = icmp eq i64 %t12, %t13
  br i1 %t14, label %on.body.7, label %on.next.8
on.body.7:
  br label %on.value.9
on.value.9:
  br label %on.end.0
on.next.8:
  %t15 = insertvalue { i64, i64 } poison, i64 11, 0
  %t16 = insertvalue { i64, i64 } %t15, i64 0, 1
  %t17 = extractvalue { i64, i64 } %token, 0
  %t18 = extractvalue { i64, i64 } %t16, 0
  %t19 = icmp eq i64 %t17, %t18
  br i1 %t19, label %on.body.10, label %on.next.11
on.body.10:
  br label %on.value.12
on.value.12:
  br label %on.end.0
on.next.11:
  unreachable
on.end.0:
  %t20 = phi { ptr, i64 } [{ ptr @.str.m0.17, i64 3 }, %on.value.3], [{ ptr @.str.m0.18, i64 5 }, %on.value.6], [{ ptr @.str.m0.19, i64 6 }, %on.value.9], [{ ptr @.str.m0.20, i64 6 }, %on.value.12]
  ret { ptr, i64 } %t20
}

define internal i32 @fn.2() {
  %t0 = insertvalue { i64, i64 } poison, i64 10, 0
  %t1 = insertvalue { i64, i64 } %t0, i64 0, 1
  %t2 = insertvalue { i64, i64 } poison, i64 11, 0
  %t3 = insertvalue { i64, i64 } %t2, i64 0, 1
  %t4 = load [12 x { ptr, i64 }], ptr @$labels
  %t5 = extractvalue [12 x { ptr, i64 }] %t4, 0
  call void @$prn({ ptr, i64 } %t5)
  %t6 = load [12 x { ptr, i64 }], ptr @$labels
  %t7 = extractvalue [12 x { ptr, i64 }] %t6, 11
  call void @$prn({ ptr, i64 } %t7)
  %t8 = call { ptr, i64 } @fn.0({ i64, i64 } %t1)
  call void @$prn({ ptr, i64 } %t8)
  %t9 = call { ptr, i64 } @fn.1({ i64, i64 } %t3)
  call void @$prn({ ptr, i64 } %t9)
  %t10 = insertvalue { i64, i64 } poison, i64 10, 0
  %t11 = insertvalue { i64, i64 } %t10, i64 0, 1
  %t12 = extractvalue { i64, i64 } %t1, 0
  %t13 = extractvalue { i64, i64 } %t11, 0
  %t14 = icmp eq i64 %t12, %t13
  br i1 %t14, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  br label %on.body.4
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.end.0:
  %t15 = phi i1 [1, %on.value.3], [0, %on.value.6]
  %t16 = insertvalue { i64, i64 } poison, i64 11, 0
  %t17 = insertvalue { i64, i64 } %t16, i64 0, 1
  %t18 = extractvalue { i64, i64 } %t3, 0
  %t19 = extractvalue { i64, i64 } %t17, 0
  %t20 = icmp eq i64 %t18, %t19
  br i1 %t20, label %on.body.8, label %on.next.9
on.body.8:
  br label %on.value.10
on.value.10:
  br label %on.end.7
on.next.9:
  br label %on.body.11
on.body.11:
  br label %on.value.13
on.value.13:
  br label %on.end.7
on.end.7:
  %t21 = phi i1 [1, %on.value.10], [0, %on.value.13]
  %t22 = and i1 %t15, %t21
  %t23 = icmp eq i1 %t22, 1
  br i1 %t23, label %on.body.15, label %on.next.16
on.body.15:
  br label %on.value.17
on.value.17:
  br label %on.end.14
on.next.16:
  br label %on.body.18
on.body.18:
  br label %on.value.20
on.value.20:
  br label %on.end.14
on.end.14:
  %t24 = phi i32 [0, %on.value.17], [1, %on.value.20]
  ret i32 %t24
}

@$describe_direction = internal alias { ptr, i64 } ({ i64, i64 }), ptr @fn.0
@$describe_token = internal alias { ptr, i64 } ({ i64, i64 }), ptr @fn.1
@$main = alias i32 (), ptr @fn.2