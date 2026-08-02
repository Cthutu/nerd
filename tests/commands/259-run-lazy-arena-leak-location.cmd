main :: fn () {
    byte_arena: arena
    _bytes := byte_arena.alloc_bytes(13)
    array_arena: arena
    _array := array_arena.alloc_array[i32](3)
    value_arena: arena
    _value := value_arena.alloc[i32]()
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
  arena     3     4  259-run-lazy-arena-leak-location.input.n:7 (4096 bytes committed)
  arena     2    12  259-run-lazy-arena-leak-location.input.n:5 (4096 bytes committed)
  arena     1    13  259-run-lazy-arena-leak-location.input.n:3 (4096 bytes committed)
nrt: total 0 heap leaks, 0 bytes; 3 arena leaks, 12288 bytes committed
