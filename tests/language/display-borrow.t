-- Custom Display takes precedence over structural formatting and borrows storage.
World :: plex { locations []Location visits i32 }
Location :: plex { name string }
calls: i32
created: i32
impl Display for World {
    show :: fn (self: ^Self) -> string {
        calls += 1
        self.visits += 1
        return $"<World with {self.locations.count} locations>"
    }
}
impl World {
    show :: fn (_self: ^Self) => "inherent"
}
make_world :: fn () -> World {
    created += 1
    return { locations: [{ name: "Elwynn Forest" }], visits: 0 }
}
Owned :: plex { storage arena }
impl Display for Owned {
    show :: fn (self: ^Self) -> string {
        value := self.storage.alloc[i32]()
        value^ = 42
        return $"owned={value^}"
    }
}
Cell :: plex [T] { value T }
impl [T] Display for Cell[T] where T: Display {
    show :: fn (self: ^Self) -> string {
        return self.value.show()
    }
}
main :: fn () {
    world := make_world()
    assert $"{world}" == "<World with 1 locations>"
    assert world.visits == 1 && calls == 1
    assert world.show() == "inherent"
    assert Display.show(world) == "<World with 1 locations>"
    assert world.visits == 2 && calls == 2
    assert $"{make_world()}" == "<World with 1 locations>"
    assert created == 2 && calls == 3
    owner: Owned
    defer owner.storage.done()
    assert $"{owner}" == "owned=42"
    cell := Cell[i32] { value: 42 }
    assert $"{cell}" == "42"
    prn("borrowed display passed")
}
¬
0
¬
borrowed display passed

¬
hir 0
¬

=== case: explicit-primitive-display ===
impl Display for i32 {
    show :: fn (_self: ^Self) -> string { return "integer" }
}
main :: fn () {
    value := 42
    assert $"{value}" == "integer"
    assert value.show() == "integer"
    assert $"{value:04d}" == "0042"
    prn("primitive display passed")
}
¬
0
¬
primitive display passed

¬
hir 0
¬
