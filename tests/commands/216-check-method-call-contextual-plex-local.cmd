Thing :: plex {
    value i32
}

System :: plex {
    count i32
}

impl System {
    apply :: fn (system: ^Self, thing: ^Thing) {
        system.count += thing.value
    }
}

main :: fn () {
    system: System = { count: 0 }
    thing := { value: 42 }
    system.apply(^thing)
}
¬
0
¬

¬
delete
¬

¬
check
¬
