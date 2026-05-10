Location :: plex {
    description string
    tag         string
}

locs: []Location = [
    { description : "d", tag : "a" },
]

main :: fn () -> i32 {
    noun := "a"
    for i, loc in locs {
        on noun == loc.tag => return i.as(i32)
    }
    return 1
}
¬
0
¬
¬
delete
¬
--llvm-backend
