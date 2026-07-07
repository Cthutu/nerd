main :: fn () -> i32 {
    Point :: plex {
        x i32
        y i32
    }

    Value :: union {
        i i32
        f f32
    }

    Choice :: enum { Left(i32) Right(i32) }

    point: Point = Point { x: 10, y: 20 }
    value: Value = Value { i: 7 }
    choice: Choice = Right(5)

    extra := on choice {
        Left(n) => n
        Right(n) => n
    }

    return point.x + point.y + value.i + extra
}
¬
42
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  let Point: plex { i32 x, i32 y } = plex { i32 x, i32 y } <unsupported>
  let Value: union { i32 i, f32 f } = union { i32 i, f32 f } <unsupported>
  let Choice: enum { Left(i32), Right(i32) } = enum { Left(i32), Right(i32) } <unsupported>
  let point: plex { i32 x, i32 y } = plex { i32 x, i32 y } plex(x: i32 10, y: i32 20)
  let value: union { i32 i, f32 f } = union { i32 i, f32 f } plex(i: i32 7)
  let choice: enum { Left(i32), Right(i32) } = enum { Left(i32), Right(i32) } call Right(i32 5)
  let extra: i32 = i32 on enum { Left(i32), Right(i32) } local.5(choice) {
    enum_variant(Left, as n) => {
      expr i32 local.6(n)
    }
    enum_variant(Right, as n) => {
      expr i32 local.7(n)
    }
  }
  return i32 add(i32 add(i32 add(i32 field(plex { i32 x, i32 y } local.3(point), x), i32 field(plex { i32 x, i32 y } local.3(point), y)), i32 field(union { i32 i, f32 f } local.4(value), i)), i32 local.8(extra))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [45 x i8] c"tests/language/144-local-type-declarations.t\00"

define internal i32 @fn.0() {
  %local.2 = alloca { i64, i32 }
  %local.5 = alloca { i64, i32 }
  store { i64, i32 } zeroinitializer, ptr %local.2
  %t0 = insertvalue { i32, i32 } poison, i32 10, 0
  %t1 = insertvalue { i32, i32 } %t0, i32 20, 1
  %t2 = insertvalue { i64, i32 } poison, i64 1, 0
  %t3 = insertvalue { i64, i32 } %t2, i32 5, 1
  store { i64, i32 } %t3, ptr %local.5
  %t4 = load { i64, i32 }, ptr %local.5
  %t5 = extractvalue { i64, i32 } %t4, 0
  %t6 = icmp eq i64 %t5, 0
  %t7 = extractvalue { i64, i32 } %t4, 1
  %t8 = and i1 %t6, 1
  br i1 %t8, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t9 = extractvalue { i64, i32 } %t4, 0
  %t10 = icmp eq i64 %t9, 1
  %t11 = extractvalue { i64, i32 } %t4, 1
  %t12 = and i1 %t10, 1
  br i1 %t12, label %on.body.4, label %on.next.5
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  unreachable
on.end.0:
  %t13 = phi i32 [%t7, %on.value.3], [%t11, %on.value.6]
  %t14 = extractvalue { i32, i32 } %t1, 0
  %t15 = extractvalue { i32, i32 } %t1, 1
  %t16 = add i32 %t14, %t15
  %t17 = add i32 %t16, 7
  %t18 = add i32 %t17, %t13
  ret i32 %t18
}

@$main = alias i32 (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)
