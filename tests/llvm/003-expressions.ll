add :: fn(a: i32, b: i32) -> i32 {
    return a + b
}

main :: fn() -> i32 {
    return add(20, 22) * 2
}
¬
define i32 @$add(i32 %a, i32 %b) {
  %t0 = add i32 %a, %b
  ret i32 %t0
}
define i32 @$main() {
  %t0 = call i32 @$add(i32 20, i32 22)
  %t1 = mul i32 %t0, 2
  ret i32 %t1
}
