sum_pair :: fn(pair: (i32, i32)) -> i32 {
    return pair.0 + pair.1
}

main :: fn() -> i32 {
    pair: (i32, i32) = (1, 2)
    return sum_pair(pair)
}
¬
define i32 @fn.0({ i32, i32 } %pair) {
  %t0 = extractvalue { i32, i32 } %pair, 0
  %t1 = extractvalue { i32, i32 } %pair, 1
  %t2 = add i32 %t0, %t1
  ret i32 %t2
}
define i32 @fn.1() {
  %t0 = insertvalue { i32, i32 } poison, i32 1, 0
  %t1 = insertvalue { i32, i32 } %t0, i32 2, 1
  %t2 = call i32 @fn.0({ i32, i32 } %t1)
  ret i32 %t2
}
@$sum_pair = alias i32 ({ i32, i32 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
