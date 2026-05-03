boxmod :: use test.imported_plex

consume :: fn (text: string) {
    on text.count == 0 => return
    return
}

main :: fn () {
    box := boxmod.make_box(7)
    consume($"Value: {box.value}")
}
¬
0
¬

¬

¬
void init() {}
#ifndef NERD_TYPE_plex27dcca0a
#define NERD_TYPE_plex27dcca0a
typedef struct plex27dcca0a {
    uintptr_t $value;
} plex27dcca0a;
#endif
int $consume(string $text) {
    uintptr_t $0 = $text.count;
    bool $1 = $0 == 0;
    if (!$1) goto L2;
    return 0;
    L2: ;
    return 0;
}
int $main() {
    string_builder_reset();
    plex27dcca0a $0 = make_box(7);
    plex27dcca0a $box = $0;
    size_t $1 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"Value: ", .count = 7}));
    uintptr_t $3 = $box.$value;
    string_builder_append_string(to_string$usize($3));
    string $2 = string_builder_finish($1);
    int $4 = $consume($2);
    string_builder_reset();
    return 0;
}
