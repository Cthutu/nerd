arena :: mod std.arena

main :: fn () -> i32 {
    a := arena.arena_new(16)
    result := a.capacity.as(i32) - 16
    arena.arena_free(^a)
    return result
}
¬
0
¬

¬

¬
