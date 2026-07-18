use test.folder_part_global

main :: fn () {
    part_alloc_leak()
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
  heap      1    13  __REPO__/tests/mods/test/folder_part_global/state.n:4
nrt: total 1 heap leaks, 13 bytes; 0 arena leaks, 0 bytes committed
