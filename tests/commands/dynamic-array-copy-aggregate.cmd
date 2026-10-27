World :: plex { values [..]i32 }
Holder :: union { values [..]i32 bits u64 }
make :: fn () -> World {
    values := [1, 2]
    return { values }
}
make_holder :: fn () -> Holder {
    values := [3]
    return Holder { values }
}
main :: fn () {
    _world := make()
    _holder := make_holder()
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
  heap      2    36  dynamic-array-copy-aggregate.input.n:9
  heap      1    40  dynamic-array-copy-aggregate.input.n:5
nrt: total 2 heap leaks, 76 bytes; 0 arena leaks, 0 bytes committed

