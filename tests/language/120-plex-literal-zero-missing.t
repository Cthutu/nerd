Point :: plex {
    x i32
    y i32
    z i32
}

main :: fn () -> i32 {
    p := Point { x: 7, ... }
    return p.x + p.y + p.z
}
¬
7
¬

¬
hir 0
bind Point = type.0
bind main = fn.0
type type.0 = Point
func fn.0() -> i32 {
  let p: Point = Point plex(x: i32 7, ...)
  return i32 add(i32 add(i32 field(Point local.0(p), x), i32 field(Point local.0(p), y)), i32 field(Point local.0(p), z))
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %t0 = insertvalue { i32, i32, i32 } poison, i32 7, 0
  %t1 = insertvalue { i32, i32, i32 } %t0, i32 0, 1
  %t2 = insertvalue { i32, i32, i32 } %t1, i32 0, 2
  %t3 = extractvalue { i32, i32, i32 } %t2, 0
  %t4 = extractvalue { i32, i32, i32 } %t2, 1
  %t5 = add i32 %t3, %t4
  %t6 = extractvalue { i32, i32, i32 } %t2, 2
  %t7 = add i32 %t5, %t6
  ret i32 %t7
}

@$main = alias i32 (), ptr @fn.0
¬

