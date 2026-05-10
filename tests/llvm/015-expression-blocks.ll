main :: fn() -> i32 {
    result := $ {
        break 7
    }
    return result
}
¬
define i32 @fn.0() {
  ret i32 7
}
@$main = alias i32 (), ptr @fn.0
