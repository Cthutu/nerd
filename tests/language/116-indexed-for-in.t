-- Iterates with both a usize index and an element binding.
Location :: plex {
    description string
    tag         string
}

locs: []Location = [
    { description : "d", tag : "a" },
]

main :: fn () -> i32 {
    noun := "a"
    for i, loc in locs {
        on noun == loc.tag => return i.as(i32)
    }
    return 0
}
¬
0
¬

¬
hir 0
bind Location = type.0
bind locs = value.0
bind main = fn.0
type type.0 = Location
global value.0: []Location = []Location array(Location plex(description: string "d", tag: string "a"))
func fn.0() -> i32 {
  let noun: string = string "a"
  expr void for in i: usize, loc: ^Location in []Location bind.1(locs) {
    body {
      expr void on bool equal(string local.0(noun), string field(^Location local.2(loc), tag)) {
    value(bool yes) => {
      return i32 cast(usize local.1(i) as i32)
    }
  }
    }
  }
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [2 x i8] c"d\00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c"a\00"
@.str.m0.2 = private unnamed_addr constant [2 x i8] c"a\00"
@.slice.const.m0.3 = private unnamed_addr constant [1 x { { ptr, i64 }, { ptr, i64 } }] [{ { ptr, i64 }, { ptr, i64 } } { { ptr, i64 } { ptr @.str.m0.0, i64 1 }, { ptr, i64 } { ptr @.str.m0.1, i64 1 } }]

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

@$locs = internal global { ptr, i64 } zeroinitializer

define void @m0.init() {
  %t0 = insertvalue { { ptr, i64 }, { ptr, i64 } } poison, { ptr, i64 } { ptr @.str.m0.0, i64 1 }, 0
  %t1 = insertvalue { { ptr, i64 }, { ptr, i64 } } %t0, { ptr, i64 } { ptr @.str.m0.1, i64 1 }, 1
  store { ptr, i64 } { ptr @.slice.const.m0.3, i64 1 }, ptr @$locs
  ret void
}

define internal i32 @fn.0() {
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
  %t5 = getelementptr inbounds { { ptr, i64 }, { ptr, i64 } }, ptr %t1, i64 %t3
  store ptr %t5, ptr %local.2
  %t6 = load ptr, ptr %local.2
  %t7 = load { { ptr, i64 }, { ptr, i64 } }, ptr %t6
  %t8 = extractvalue { { ptr, i64 }, { ptr, i64 } } %t7, 1
  %t10 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 1 }, ptr %t10
  %t11 = alloca { ptr, i64 }
  store { ptr, i64 } %t8, ptr %t11
  %t9 = call i1 @string_eq(ptr %t10, ptr %t11)
  %t12 = icmp eq i1 %t9, 1
  br i1 %t12, label %on.body.4, label %on.end.3
on.body.4:
  %t13 = load i64, ptr %local.1
  %t14 = trunc i64 %t13 to i32
  ret i32 %t14
on.end.3:
  %t15 = load i64, ptr %local.1
  %t16 = add i64 %t15, 1
  store i64 %t16, ptr %local.1
  br label %for.in.cond.0
for.in.end.2:
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
