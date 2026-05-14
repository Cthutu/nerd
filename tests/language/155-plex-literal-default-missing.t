State :: plex {
    visible bool
    name    string
    next    ^State
    count   i32
}

main :: fn () -> i32 {
    state := State { count: 7 ... }

    on state.visible => return 1
    on state.name != "" => return 2
    on state.next != nil => return 3
    return state.count
}
¬
7
¬

¬

¬
