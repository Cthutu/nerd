Info :: plex {
    handle u64
}

System :: plex {
    infos [..]Info
}

main :: fn () -> i32 {
    infos: [..]Info
    system := System { infos }
    _ := system
    return 0
}
¬
0
¬

¬
delete
¬
--llvm-backend
