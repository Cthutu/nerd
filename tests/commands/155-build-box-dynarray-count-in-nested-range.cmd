Map :: plex {
    rooms [..]Room
}

Room :: plex {
    x u32
}

gen :: fn (num_rooms: u32) -> box[Map] {
    map := box[Map]()
    room: Room

    for _room in [0 .. num_rooms] {
        for $repeat {
            for _i in [0 .. map.rooms.count] {
                on yes => {
                    again $repeat
                }
            }
            else {
                break $repeat
            }
        }

        map.rooms.push(room)
    }

    return map
}

main :: fn () {
    map := gen(1)
    map.rooms.free()
    map.free()
}
¬
0
¬
