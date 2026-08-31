-- Compatible pointers can be ordered within one allocation.
main :: fn () -> i32 {
    values: [2]i32 = [10, 20]
    first          := ^values[0]
    second         := ^values[1]

    assert first < second
    assert first <= second
    assert second > first
    assert second >= first
    return 0
}
¬
0
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  assert bool less(^i32 local.1(first), ^i32 local.2(second))
  assert bool less_equal(^i32 local.1(first), ^i32 local.2(second))
  assert bool greater(^i32 local.2(second), ^i32 local.1(first))
  assert bool greater_equal(^i32 local.2(second), ^i32 local.1(first))
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

  %t4 = icmp ult ptr %t2, %t3
  %t6 = icmp ule ptr %t2, %t3
  %t8 = icmp ugt ptr %t3, %t2
  %t10 = icmp uge ptr %t3, %t2
