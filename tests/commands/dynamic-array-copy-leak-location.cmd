main :: fn () {
    source := [1, 2]
    _first: [..]i32 = source
    view := source[1..]
    _second: [..]i32 = view
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
  heap      2    36  dynamic-array-copy-leak-location.input.n:5
  heap      1    40  dynamic-array-copy-leak-location.input.n:3
nrt: total 2 heap leaks, 76 bytes; 0 arena leaks, 0 bytes committed

