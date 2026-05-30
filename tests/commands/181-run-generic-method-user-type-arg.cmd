Rect :: plex {
    x u32
}

main :: fn () -> i32 {
    rects := temp_arena.alloc_array[Rect](1)
    rects[0].x = 42
    return rects[0].x.as(i32)
}
¬
42
¬

¬
