use std.io

RoomType :: enum { NONE HALL KITCHEN }

Room :: plex {
    exits [2]RoomType
}

rooms : [2]Room : [
    { exits: [NONE, HALL] },
    { exits: [KITCHEN, NONE] },
]

main :: fn () -> i32 {
    prn($"{rooms[0].exits[0] == NONE}")
    prn($"{rooms[0].exits[1] == HALL}")
    prn($"{rooms[1].exits[0] == KITCHEN}")
    prn($"{rooms[1].exits[1] == NONE}")
    return 0
}
¬0¬yes
yes
yes
yes

¬¬
#ifndef NERD_TYPE_enumdce9ae15
#define NERD_TYPE_enumdce9ae15
typedef struct enumdce9ae15 {
    uint8_t tag;
    union { uint8_t unit; } data;
} enumdce9ae15;
#endif
#ifndef NERD_TYPE_array6b80695e
#define NERD_TYPE_array6b80695e
typedef struct array6b80695e {
    enumdce9ae15 items[2];
} array6b80695e;
#endif
#ifndef NERD_TYPE_plex790cd9ef
#define NERD_TYPE_plex790cd9ef
typedef struct plex790cd9ef {
    array6b80695e $exits;
} plex790cd9ef;
#endif
#ifndef NERD_TYPE_array1ce52410
#define NERD_TYPE_array1ce52410
typedef struct array1ce52410 {
    plex790cd9ef items[2];
} array1ce52410;
#endif
array1ce52410 $rooms;
int $main() {
    string_builder_reset();
    size_t $0 = string_builder_mark();
    #ifndef NDEBUG
    if ((long long)0 < 0 || (size_t)0 >= 2) { eprn("fatal: array index out of bounds"); abort(); }
    #endif
    plex790cd9ef $2 = $rooms.items[0];
    array6b80695e $3 = $2.$exits;
    #ifndef NDEBUG
    if ((long long)0 < 0 || (size_t)0 >= 2) { eprn("fatal: array index out of bounds"); abort(); }
    #endif
    enumdce9ae15 $4 = $3.items[0];
    enumdce9ae15 $5 = (enumdce9ae15){.tag = 0};
    bool $6 = $4.tag == $5.tag;
    string_builder_append_string(to_string$bool($6));
    string $1 = string_builder_finish($0);
    prn($1);
    string_builder_reset();
    size_t $7 = string_builder_mark();
    #ifndef NDEBUG
    if ((long long)0 < 0 || (size_t)0 >= 2) { eprn("fatal: array index out of bounds"); abort(); }
    #endif
    plex790cd9ef $9 = $rooms.items[0];
    array6b80695e $10 = $9.$exits;
    #ifndef NDEBUG
    if ((long long)1 < 0 || (size_t)1 >= 2) { eprn("fatal: array index out of bounds"); abort(); }
    #endif
    enumdce9ae15 $11 = $10.items[1];
    enumdce9ae15 $12 = (enumdce9ae15){.tag = 1};
    bool $13 = $11.tag == $12.tag;
    string_builder_append_string(to_string$bool($13));
    string $8 = string_builder_finish($7);
    prn($8);
    string_builder_reset();
    size_t $14 = string_builder_mark();
    #ifndef NDEBUG
    if ((long long)1 < 0 || (size_t)1 >= 2) { eprn("fatal: array index out of bounds"); abort(); }
    #endif
    plex790cd9ef $16 = $rooms.items[1];
    array6b80695e $17 = $16.$exits;
    #ifndef NDEBUG
    if ((long long)0 < 0 || (size_t)0 >= 2) { eprn("fatal: array index out of bounds"); abort(); }
    #endif
    enumdce9ae15 $18 = $17.items[0];
    enumdce9ae15 $19 = (enumdce9ae15){.tag = 2};
    bool $20 = $18.tag == $19.tag;
    string_builder_append_string(to_string$bool($20));
    string $15 = string_builder_finish($14);
    prn($15);
    string_builder_reset();
    size_t $21 = string_builder_mark();
    #ifndef NDEBUG
    if ((long long)1 < 0 || (size_t)1 >= 2) { eprn("fatal: array index out of bounds"); abort(); }
    #endif
    plex790cd9ef $23 = $rooms.items[1];
    array6b80695e $24 = $23.$exits;
    #ifndef NDEBUG
    if ((long long)1 < 0 || (size_t)1 >= 2) { eprn("fatal: array index out of bounds"); abort(); }
    #endif
    enumdce9ae15 $25 = $24.items[1];
    enumdce9ae15 $26 = (enumdce9ae15){.tag = 0};
    bool $27 = $25.tag == $26.tag;
    string_builder_append_string(to_string$bool($27));
    string $22 = string_builder_finish($21);
    prn($22);
    string_builder_reset();
    return 0;
}
void init() {
    enumdce9ae15 $1 = (enumdce9ae15){.tag = 0};
    enumdce9ae15 $2 = (enumdce9ae15){.tag = 1};
    array6b80695e $3 = (array6b80695e){.items = {$1, $2}};
    plex790cd9ef $0 = (plex790cd9ef){.$exits = $3};
    enumdce9ae15 $5 = (enumdce9ae15){.tag = 2};
    enumdce9ae15 $6 = (enumdce9ae15){.tag = 0};
    array6b80695e $7 = (array6b80695e){.items = {$5, $6}};
    plex790cd9ef $4 = (plex790cd9ef){.$exits = $7};
    array1ce52410 $8 = (array1ce52410){.items = {$0, $4}};
    $rooms = $8;
}
