Point :: plex {
    x i32
    y i32
}

main :: fn () -> i32 {
    p :: Point { ... }
    return p.x + p.y
}
¬
0
¬

¬
hir 0
bind Point = type.0
bind main = fn.0
type type.0 = Point
func fn.0() -> i32 {
  let p: Point = Point plex(...)
  return i32 add(i32 field(Point local.0(p), x), i32 field(Point local.0(p), y))
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %t0 = insertvalue { i32, i32 } poison, i32 0, 0
  %t1 = insertvalue { i32, i32 } %t0, i32 0, 1
  %t2 = extractvalue { i32, i32 } %t1, 0
  %t3 = extractvalue { i32, i32 } %t1, 1
  %t4 = add i32 %t2, %t3
  ret i32 %t4
}

@$main = alias i32 (), ptr @fn.0
¬

