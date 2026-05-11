-- Binds public declarations directly to types and uses them as aliases.
pub Score :: i32
pub FinalScore :: Score

add :: fn (lhs: Score, rhs: FinalScore) -> Score {
    return lhs + rhs
}

main :: fn () {
    lhs: Score = 20
    rhs: FinalScore = 22
    return add(lhs, rhs)
}
¬
42
¬

¬
hir 0
module module.0(104-public-type-alias-binding.input)
bind Score = type.0
bind FinalScore = type.1
bind add = fn.0
bind main = fn.1
type type.0 = i32
type type.1 = i32
func fn.0(lhs: i32, rhs: i32) -> i32 {
  return i32 add(i32 local.0(lhs), i32 local.1(rhs))
}
func fn.1() -> i32 {
  let lhs: i32 = i32 20
  let rhs: i32 = i32 22
  return i32 call bind.2(add)(i32 local.2(lhs), i32 local.3(rhs))
}
export bind.0(Score)
export bind.1(FinalScore)
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0(i32 %lhs, i32 %rhs) {
  %t0 = add i32 %lhs, %rhs
  ret i32 %t0
}

define i32 @fn.1() {
  %t0 = call i32 @fn.0(i32 20, i32 22)
  ret i32 %t0
}

@$add = internal alias i32 (i32, i32), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
