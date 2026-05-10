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
¬
0
¬
yes
yes
yes
yes

¬
delete
¬
