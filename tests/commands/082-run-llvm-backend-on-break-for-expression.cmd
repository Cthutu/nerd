Info :: plex {
    handle u64
}

System :: plex {
    infos [..]Info
}

find :: fn (system: ^System, handle: u64) -> ^Info {
    return for info in system.infos {
        on info.handle == handle => break info
    } else {
        break nil
    }
}

main :: fn () -> i32 {
    infos: [2..]Info
    infos.push({ handle: 7 })
    infos.push({ handle: 9 })

    system := System { infos }
    found  := find(^system, 9)
    on found == nil => return 1
    on found.handle != 9 => return 2

    missing := find(^system, 3)
    on missing != nil => return 3

    return 0
}
¬
0
¬
¬
delete
¬
--llvm-backend
