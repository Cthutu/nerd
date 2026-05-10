-- Interpolates a field read from an indexed for-in item.
io :: use std.io

Location :: plex {
    description string
}

locs: []Location = [
    { description : "field" },
]

main :: fn () {
    player_loc: usize = 1
    for i, loc in locs {
        on {
            i == player_loc => io.prn("same")
            else => io.prn($"You go to {loc.description}.")
        }
    }
}
¬
0
¬
You go to field.

¬
hir 0
module module.0(117-interpolated-for-field.input)
import module.1(std.io)
import import.0 prn from module.1(std.io).decl.11: fn (string) -> void
bind prn = import.0
bind io = module.1
bind Location = type.0
bind locs = value.0
bind main = fn.0
type type.0 = Location
global value.0: []Location = []Location array(Location plex(description: string "field"))
func fn.0() -> void {
  let player_loc: usize = usize 1
  expr void for in i: usize, loc: ^Location in []Location bind.3(locs) {
    body {
      expr void on condition {
    bool equal(usize local.1(i), usize local.0(player_loc)) => {
      expr void call fn (string) -> void field(module bind.1(io), prn)(string "same")
    }
    else => {
      expr void call fn (string) -> void field(module bind.1(io), prn)(string interpolate(<unknown> "You go to ", string field(^Location local.2(loc), description), <unknown> "."))
    }
  }
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [6 x i8] c"field\00"
@.str.m0.1 = private unnamed_addr constant [5 x i8] c"same\00"
@.str.m0.2 = private unnamed_addr constant [11 x i8] c"You go to \00"
@.str.m0.3 = private unnamed_addr constant [2 x i8] c".\00"

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

declare void @$prn({ ptr, i64 })

@.slice.literal.m0.0 = private global [1 x { { ptr, i64 } }] zeroinitializer
@$locs = global { ptr, i64 } zeroinitializer

define void @m0.init() {
  %t0 = insertvalue { { ptr, i64 } } poison, { ptr, i64 } { ptr @.str.m0.0, i64 5 }, 0
  %t1 = insertvalue [1 x { { ptr, i64 } }] poison, { { ptr, i64 } } %t0, 0
  store [1 x { { ptr, i64 } }] %t1, ptr @.slice.literal.m0.0
  %t2 = getelementptr inbounds [1 x { { ptr, i64 } }], ptr @.slice.literal.m0.0, i64 0, i64 0
  %t3 = insertvalue { ptr, i64 } poison, ptr %t2, 0
  %t4 = insertvalue { ptr, i64 } %t3, i64 1, 1
  store { ptr, i64 } %t4, ptr @$locs
  ret void
}

define void @fn.0() {
  %t0 = load { ptr, i64 }, ptr @$locs
  %t1 = extractvalue { ptr, i64 } %t0, 0
  %t2 = extractvalue { ptr, i64 } %t0, 1
  %local.1 = alloca i64
  store i64 0, ptr %local.1
  %local.2 = alloca ptr
  br label %for.in.cond.0
for.in.cond.0:
  %t3 = load i64, ptr %local.1
  %t4 = icmp ult i64 %t3, %t2
  br i1 %t4, label %for.in.body.1, label %for.in.end.2
for.in.body.1:
  %t5 = getelementptr inbounds { { ptr, i64 } }, ptr %t1, i64 %t3
  store ptr %t5, ptr %local.2
  %t6 = load i64, ptr %local.1
  %t7 = icmp eq i64 %t6, 1
  br i1 %t7, label %on.body.4, label %on.next.5
on.body.4:
  call void @$prn({ ptr, i64 } { ptr @.str.m0.1, i64 4 })
  br label %on.end.3
on.next.5:
  br label %on.body.6
on.body.6:
  %t8 = call i64 @string_builder_mark()
  %t9 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 10 })
  call void @string_builder_append_string({ ptr, i64 } %t9)
  %t10 = load ptr, ptr %local.2
  %t11 = load { { ptr, i64 } }, ptr %t10
  %t12 = extractvalue { { ptr, i64 } } %t11, 0
  %t13 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t12)
  call void @string_builder_append_string({ ptr, i64 } %t13)
  %t14 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.3, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t14)
  %t15 = call { ptr, i64 } @string_builder_finish(i64 %t8)
  call void @$prn({ ptr, i64 } %t15)
  br label %on.end.3
on.end.3:
  %t16 = load i64, ptr %local.1
  %t17 = add i64 %t16, 1
  store i64 %t17, ptr %local.1
  br label %for.in.cond.0
for.in.end.2:
  ret void
}

@$main = alias void (), ptr @fn.0
