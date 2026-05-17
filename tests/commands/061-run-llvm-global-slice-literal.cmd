Location :: plex {
    description string
}

locs: []Location = [
    { description : "field" },
]

main :: fn () {
    player_loc: usize = 1
    for i, loc in locs {
        on {
            i == player_loc => prn("same")
            else => prn($"You go to {loc.description}.")
        }
    }
}
¬
0
¬
You go to field.

¬
delete
¬
