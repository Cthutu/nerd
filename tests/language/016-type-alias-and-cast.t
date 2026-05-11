-- Uses a type alias and narrows a value with an explicit cast.
Price :: u16

main :: fn () {
    amount: Price = 1000
    narrowed := amount.as(u8)
    return narrowed
}
¬
232
¬

¬
hir 0
bind Price = type.0
bind main = fn.0
type type.0 = u16
func fn.0() -> u8 {
  let amount: u16 = u16 1000
  let narrowed: u8 = u8 cast(u16 local.0(amount) as u8)
  return u8 local.1(narrowed)
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i8 @fn.0() {
  %t0 = trunc i16 1000 to i8
  ret i8 %t0
}

@$main = alias i8 (), ptr @fn.0
