main :: fn () {
    prn_wrap("Now is the time for all good people to come to the aid of party "
    "and make merry with the likes of us.")
}

State :: plex {
    -- Database
    locations []string

    -- Game state
    player_loc usize
}

init :: fn () -> State {
    return {
        locations:[
            "I am in a Hall.  The Kitchen is to the East, the Bedroom to the West and the Lounge to the South.  Steps lead Down to the Cellar.",
            "I am in the Cellar.  Steps lead Up to the Hall.".
        ]
        player_loc: 0
    }
}
¬
main :: fn () {
    prn_wrap("Now is the time for all good people to come to the aid of party "
        "and make merry with the likes of us.")
}

State :: plex {
    -- Database
    locations []string

    -- Game state
    player_loc usize
}

init :: fn () -> State {
    return {
        locations:[
            "I am in a Hall.  The Kitchen is to the East, the Bedroom to the "
                "West and the Lounge to the South.  Steps lead Down to the "
                "Cellar.",
            "I am in the Cellar.  Steps lead Up to the Hall.".
        ]
        player_loc: 0
    }
}
