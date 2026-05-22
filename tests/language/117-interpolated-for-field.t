-- Interpolates a field read from an indexed for-in item.

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
            i == player_loc => prn("same")
            else => prn($"You go to {loc.description}.")
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
import import.0 prn from module.1(core).decl.13: fn (string) -> void
bind prn = import.0
bind Location = type.0
bind locs = value.0
bind main = fn.0
type type.0 = Location
global value.0: []Location = []Location array(Location plex(description: string "field"))
func fn.0() -> void {
  let player_loc: usize = usize 1
  expr void for in i: usize, loc: ^Location in []Location bind.2(locs) {
    body {
      expr void on condition {
    bool equal(usize local.1(i), usize local.0(player_loc)) => {
      expr void call bind.0(prn)(string "same")
    }
    else => {
      expr void call bind.0(prn)(string interpolate(<unknown> "You go to ", string field(^Location local.2(loc), description), <unknown> "."))
    }
  }
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [44 x i8] c"tests/language/117-interpolated-for-field.t\00"
@.str.m0.0 = private unnamed_addr constant [6 x i8] c"field\00"
@.str.m0.1 = private unnamed_addr constant [5 x i8] c"same\00"
@.str.m0.2 = private unnamed_addr constant [11 x i8] c"You go to \00"
@.str.m0.3 = private unnamed_addr constant [2 x i8] c".\00"
@.slice.const.m0.2 = private unnamed_addr constant [1 x { { ptr, i64 } }] [{ { ptr, i64 } } { { ptr, i64 } { ptr @.str.m0.0, i64 5 } }]

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

declare void @$prn({ ptr, i64 })

@$locs = internal global { ptr, i64 } zeroinitializer

define void @m0.init() {
  %t0 = insertvalue { { ptr, i64 } } poison, { ptr, i64 } { ptr @.str.m0.0, i64 5 }, 0
  store { ptr, i64 } { ptr @.slice.const.m0.2, i64 1 }, ptr @$locs
  ret void
}

define internal void @fn.0() {
  %local.1 = alloca i64
  %local.2 = alloca ptr
  %t10 = alloca { ptr, i64 }
  %t15 = alloca { ptr, i64 }
  %t17 = alloca { ptr, i64 }
  %t0 = load { ptr, i64 }, ptr @$locs
  %t1 = extractvalue { ptr, i64 } %t0, 0
  %t2 = extractvalue { ptr, i64 } %t0, 1
  store i64 0, ptr %local.1
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
  %t9 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 10 }, ptr %t10
  call void @to_string$string(ptr %t9, ptr %t10)
  call void @string_builder_append_string(ptr %t9)
  %t11 = load ptr, ptr %local.2
  %t12 = load { { ptr, i64 } }, ptr %t11
  %t13 = extractvalue { { ptr, i64 } } %t12, 0
  %t14 = alloca { ptr, i64 }
  store { ptr, i64 } %t13, ptr %t15
  call void @to_string$string(ptr %t14, ptr %t15)
  call void @string_builder_append_string(ptr %t14)
  %t16 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 1 }, ptr %t17
  call void @to_string$string(ptr %t16, ptr %t17)
  call void @string_builder_append_string(ptr %t16)
  %t18 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t18, i64 %t8)
  %t19 = load { ptr, i64 }, ptr %t18
  call void @$prn({ ptr, i64 } %t19)
  br label %on.end.3
on.end.3:
  %t20 = load i64, ptr %local.1
  %t21 = add i64 %t20, 1
  store i64 %t21, ptr %local.1
  br label %for.in.cond.0
for.in.end.2:
  ret void
}

@$main = alias void (), ptr @fn.0
