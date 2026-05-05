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
void init() {}
#ifndef NERD_TYPE_slice6d447031
#define NERD_TYPE_slice6d447031
typedef struct slice6d447031 {
    uint8_t* data;
    uintptr_t count;
} slice6d447031;
#endif
#ifndef NERD_TYPE_plex22418da5
#define NERD_TYPE_plex22418da5
typedef struct plex22418da5 {
    slice6d447031 $base;
    uintptr_t $cursor;
} plex22418da5;
#endif
int $main() {
    plex22418da5 $0 = arena_new(16);
    plex22418da5 $a = $0;
    struct plex22418da5* $1 = &$a;
    uintptr_t $2 = arena_capacity($1);
    int $3 = (int)$2;
    int $4 = $3 - 16;
    int $result = $4;
    struct plex22418da5* $5 = &$a;
    slice6d447031 $6 = arena_alloc($5, 4);
    slice6d447031 $block = $6;
    uintptr_t $7 = $block.count;
    int $8 = (int)$7;
    int $9 = $8 - 4;
    int $10 = $result + $9;
    $result = $10;
    struct plex22418da5* $11 = &$a;
    int $12 = arena_free($11);
    return $result;
}
