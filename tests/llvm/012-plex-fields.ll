Point :: plex {
    x i32
    y i32
}

sum_point :: fn(point: Point) -> i32 {
    return point.x + point.y
}

main :: fn() -> i32 {
    point := Point { x: 1, y: 2 }
    moved := point with { y: 3 }
    return sum_point(moved)
}
¬
define i32 @fn.0({ i32, i32 } %point) {
  %t0 = extractvalue { i32, i32 } %point, 0
  %t1 = extractvalue { i32, i32 } %point, 1
  %t2 = add i32 %t0, %t1
  ret i32 %t2
}
define i32 @fn.1() {
  %t0 = insertvalue { i32, i32 } { i32 1, i32 2 }, i32 3, 1
  %t1 = call i32 @fn.0({ i32, i32 } %t0)
  ret i32 %t1
}
@$sum_point = alias i32 ({ i32, i32 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
