-- test-platform: linux

arena :: use std.arena

main :: fn () -> i32 {
    a := arena.arena_new(16)
    result := arena.arena_capacity(^a).as(i32) - 16
    block := arena.arena_alloc(^a, 4)
    result += block.count.as(i32) - 4
    arena.arena_free(^a)
    return result
}
¬
0
¬

¬

¬
