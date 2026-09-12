use std.memory

main :: fn () {
    _ := alloc(13)
}
¬
0
¬
¬
delete
¬
--llvm
¬
run
¬
nrt: memory leaks detected
  type  index bytes  location
  heap      1    13  254-run-core-done-reports-leaks.input.n:4
nrt: total 1 heap leaks, 13 bytes; 0 arena leaks, 0 bytes committed
