State :: plex {
    exits [][](string, usize)
    player_loc usize
}

init :: fn () -> State {
    return {
        exits      : [
            [
                ("E", 1),
                ("W", 4),
            ],
            [("S", 2)],
        ]
        player_loc : 0
    }
}

main :: fn () -> i32 {
    state := init()

    on state.exits.count != 2 => return 1
    on state.exits[state.player_loc].count != 2 => return 2
    on state.exits[state.player_loc][0].0 != "E" => return 3
    on state.exits[state.player_loc][1].1 != 4 => return 4

    total := 0
    for _, exit in state.exits[state.player_loc] {
        total += exit.1.as(i32)
    }

    on total != 5 => return 5
    return 0
}
¬
0
¬

¬
delete
¬
