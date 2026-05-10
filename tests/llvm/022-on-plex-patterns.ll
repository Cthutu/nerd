Point :: plex {
    x i32
    y i32
}

score :: fn(point: Point) -> i32 {
    return on point {
        { x: 0, y: as y } => y
        { x: as x, y: 9 } => x * 10
        else => 0
    }
}

main :: fn() -> i32 {
    point := Point { x: 4, y: 9 }
    return score(point)
}
¬
define i32 @fn.0({ i32, i32 } %point) {
  %t0 = extractvalue { i32, i32 } %point, 0
  %t1 = icmp eq i32 %t0, 0
  %t2 = extractvalue { i32, i32 } %point, 1
  %t3 = and i1 %t1, 1
  br i1 %t3, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.end.0
on.next.2:
  %t4 = extractvalue { i32, i32 } %point, 0
  %t5 = extractvalue { i32, i32 } %point, 1
  %t6 = icmp eq i32 %t5, 9
  %t7 = and i1 1, %t6
  br i1 %t7, label %on.body.3, label %on.next.4
on.body.3:
  %t8 = mul i32 %t4, 10
  br label %on.end.0
on.next.4:
  br label %on.body.5
on.body.5:
  br label %on.end.0
on.end.0:
  %t9 = phi i32 [%t2, %on.body.1], [%t8, %on.body.3], [0, %on.body.5]
  ret i32 %t9
}
define i32 @fn.1() {
  %t0 = insertvalue { i32, i32 } poison, i32 4, 0
  %t1 = insertvalue { i32, i32 } %t0, i32 9, 1
  %t2 = call i32 @fn.0({ i32, i32 } %t1)
  ret i32 %t2
}
@$score = alias i32 ({ i32, i32 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
