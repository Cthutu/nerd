less :: fn(a: i32, b: i32) -> bool {
    return a < b
}

ready :: fn(a: bool, b: bool) -> bool {
    return a && b || yes
}

main :: fn() -> i32 {
    return 0
}
¬
define i1 @fn.0(i32 %a, i32 %b) {
  %t0 = icmp slt i32 %a, %b
  ret i1 %t0
}
define i1 @fn.1(i1 %a, i1 %b) {
  %t0 = and i1 %a, %b
  %t1 = or i1 %t0, 1
  ret i1 %t1
}
define i32 @fn.2() {
  ret i32 0
}
@$less = alias i1 (i32, i32), ptr @fn.0
@$ready = alias i1 (i1, i1), ptr @fn.1
@$main = alias i32 (), ptr @fn.2
