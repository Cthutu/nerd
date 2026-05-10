-- Uses shorthand plex fields where `name` means `name: name`.
Point :: plex {
    x i32
    y i32
}

main :: fn () -> i32 {
    x := 7
    y := 11
    point := Point { x, y }
    other := point with { x }
    return point.x + point.y + other.x
}
¬
25
¬

¬
hir 0
bind Point = type.0
bind main = fn.0
type type.0 = Point
func fn.0() -> i32 {
  let x: i32 = untyped integer 7
  let y: i32 = untyped integer 11
  let point: Point = Point plex(x: i32 local.0(x), y: i32 local.1(y))
  let other: Point = Point plex_update(Point local.2(point), x: i32 local.0(x))
  return i32 add(i32 add(i32 field(Point local.2(point), x), i32 field(Point local.2(point), y)), i32 field(Point local.3(other), x))
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0() {
  %t0 = insertvalue { i32, i32 } poison, i32 7, 0
  %t1 = insertvalue { i32, i32 } %t0, i32 11, 1
  %t2 = insertvalue { i32, i32 } %t1, i32 7, 0
  %t3 = extractvalue { i32, i32 } %t1, 0
  %t4 = extractvalue { i32, i32 } %t1, 1
  %t5 = add i32 %t3, %t4
  %t6 = extractvalue { i32, i32 } %t2, 0
  %t7 = add i32 %t5, %t6
  ret i32 %t7
}

@$main = alias i32 (), ptr @fn.0
