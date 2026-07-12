choose :: fn (enabled :: bool = yes) -> i32 {
    return on enabled => 7 else 9
}

main :: fn () -> i32 {
    return choose() + choose(no)
}
¬
define internal i32 @fn.0() {
  %t0 = icmp eq i1 1, 1
define internal i32 @fn.1() {
  %t0 = icmp eq i1 0, 1
define internal i32 @fn.2(i1 %enabled) {
  %t0 = icmp eq i1 %enabled, 1
define internal i32 @fn.3() {
  %t0 = call i32 @fn.0()
  %t1 = call i32 @fn.1()
