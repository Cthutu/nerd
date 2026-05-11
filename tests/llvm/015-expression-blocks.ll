main :: fn() -> i32 {
    result := $ {
        break 7
    }
    return result
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %t0 = alloca i32, align 4
  store i32 0, ptr %t0, align 4
  store i32 7, ptr %t0, align 4
  br label %block.end.0
block.end.0:
  %t1 = load i32, ptr %t0, align 4
  ret i32 %t1
}

@$main = alias i32 (), ptr @fn.0
