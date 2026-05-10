use std.io

main :: fn() {
    prn("Hello")
}
¬
; nerd llvm-ir 0
declare void @$prn(ptr)
define void @fn.0() {
  call void @$prn(ptr null)
  ret void
}
@$main = alias void (), ptr @fn.0
