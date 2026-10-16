main :: fn () {
    assert current_temp_arena() == temp_arena
    mark := current_temp_arena().mark()
    prn($"current arena {42}")
    current_temp_arena().restore(mark)
}
¬
0
¬
current arena 42
¬
delete
¬
--llvm
