Info :: plex {
    handle u64
}

System :: plex {
    infos [..]Info
}

find :: fn (system: ^System, handle: u64) -> ^Info {
    return for info in system.infos {
        break on info.handle == handle => info
    } else {
        break nil
    }
}

main :: fn () -> i32 {
    labelled := $pick {
        break $pick on 1 == 1 => 5
        break $pick 9
    }
    on labelled != 5 => return 1

    infos: [2..]Info
    infos.push({ handle: 7 })
    infos.push({ handle: 9 })

    system := System { infos }
    found  := find(^system, 9)
    on found == nil => return 2
    on found.handle != 9 => return 3

    missing := find(^system, 3)
    on missing != nil => return 4

    return 0
}
¬
0
¬

¬

¬
