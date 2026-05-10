main :: fn() -> i32 {
    assert 1 < 2, "ok"
    value := 1
    defer value = 2
    return value
}
¬
define i32 @fn.0() {
  %t0 = icmp slt i32 1, 2
  ret i32 1
}
@$main = alias i32 (), ptr @fn.0
