main :: fn () -> i32 {
    total := 0
    for value in [0..10] {
        total += value
    }

    for index, value in [2..=4] {
        total += index.as(i32) + value
    }

    return total
}
¬
57
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  let total: i32 = untyped integer 0
  expr void for in value: i32 in i32 range_exclusive(untyped integer 0, untyped integer 10) {
    body {
      assign i32 local.0(total) = i32 add(i32 local.0(total), i32 local.1(value))
    }
  }
  expr void for in index: usize, value: i32 in i32 range_inclusive(untyped integer 2, untyped integer 4) {
    body {
      assign i32 local.0(total) = i32 add(i32 local.0(total), i32 add(i32 cast(usize local.2(index) as i32), i32 local.3(value)))
    }
  }
  return i32 local.0(total)
}
¬

