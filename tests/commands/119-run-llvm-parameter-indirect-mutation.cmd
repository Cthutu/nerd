State :: plex {
    player_loc i32
}

move :: fn (state: ^State) {
    state.player_loc = 7
}

write_ptr :: fn (value: ^i32) {
    value^ = 11
}

write_slice :: fn (values: []i32) {
    values[0] = 13
}

main :: fn () -> i32 {
    state := State { player_loc: 0 }
    move(^state)

    value := 0
    write_ptr(^value)

    values: [1]i32 = [0]
    write_slice(values[..])

    return state.player_loc + value + values[0]
}
¬
31
¬
¬
delete
