State::plex{values [2]usize index usize}
update::fn(state:^State){
state.values[state.index]+=1
state.values[state.index]-=1
}
¬
State :: plex {
    values [2]usize
    index  usize
}

update :: fn (state: ^State) {
    state.values[state.index] += 1
    state.values[state.index] -= 1
}
