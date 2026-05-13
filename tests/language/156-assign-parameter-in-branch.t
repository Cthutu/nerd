normalise :: fn (seed: u64) -> u64 {
    on seed == 0 => seed = 7
    return seed
}

main :: fn () -> i32 {
    return (normalise(0) + normalise(5)).as(i32)
}
¬
12
¬

¬
hir 0
bind normalise = fn.0
bind main = fn.1
func fn.0(seed: u64) -> u64 {
  expr void on bool equal(u64 local.0(seed), u64 0) {
    value(bool yes) => {
      assign u64 local.0(seed) = u64 7
    }
  }
  return u64 local.0(seed)
}
func fn.1() -> i32 {
  return i32 cast(u64 add(u64 call bind.0(normalise)(u64 0), u64 call bind.0(normalise)(u64 5)) as i32)
}
¬
define internal i64 @fn.0(i64 %seed) {
  %local.0 = alloca i64
  store i64 %seed, ptr %local.0
  %t0 = load i64, ptr %local.0
  %t1 = icmp eq i64 %t0, 0
  %t2 = icmp eq i1 %t1, 1
  br i1 %t2, label %on.body.1, label %on.end.0
on.body.1:
  store i64 7, ptr %local.0
  br label %on.end.0
on.end.0:
  %t3 = load i64, ptr %local.0
  ret i64 %t3
}
