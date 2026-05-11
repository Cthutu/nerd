pub counter := 3

main :: fn () -> i32 {
    counter += 4
    return counter
}
¬
7
¬

¬
hir 0
module module.0(145-public-mutable-globals.input)
bind counter = value.0
bind main = fn.0
global value.0: i32 = untyped integer 3
func fn.0() -> i32 {
  assign i32 bind.0(counter) = i32 add(i32 bind.0(counter), i32 4)
  return i32 bind.0(counter)
}
export bind.0(counter)
¬
; nerd llvm-ir 0
; generated from HIR

@$counter = global i32 0

define void @m0.init() {
  store i32 3, ptr @$counter
  ret void
}

define internal i32 @fn.0() {
  %t0 = load i32, ptr @$counter
  %t1 = add i32 %t0, 4
  store i32 %t1, ptr @$counter
  %t2 = load i32, ptr @$counter
  ret i32 %t2
}

@$main = alias i32 (), ptr @fn.0
