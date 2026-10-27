Mob :: plex { name string }
Event :: enum { Found(Mob) Missing }
score :: fn (mob: Mob) -> i32 {
    return on mob {
        Mob { name: "gnoll" } => 1
        else => 0
    }
}
describe :: fn (event: Event) -> string {
    return on event {
        Event.Found(Mob { name }) => name
        Event.Missing => "missing"
    }
}
main :: fn () {
    Local :: plex { value i32 }
    local := Local { value: 42 }
    on local { Local { value } => { assert value == 42 } }
    mob := Mob { name: "gnoll" }
    assert score(mob) == 1
    assert score(Mob { name: "bear" }) == 0
    assert describe(Event.Found(mob)) == "gnoll"
    assert describe(Event.Missing) == "missing"
    prn("braced typed plex patterns passed")
}
¬
0
¬
braced typed plex patterns passed

¬
hir 0
¬
