Mob :: plex { id u32 name string }
World :: plex { mobs []Mob }
OwnedWorld :: plex { mobs [2]Mob }
sum :: fn (items: []Mob) -> u32 {
    total: u32
    for item in items { total += item.id }
    return total
}
read_world :: fn (world: World) => sum(world.mobs)
read_nested :: fn (items: [1][1]Mob) => items[0][0].id
read_bytes :: fn (items: []u8) => items[0] + items[1]
read_id :: fn (id: u32) => id
make_owned :: fn () -> OwnedWorld {
    mobs := [{ id: 20, name: "Bear" }, { id: 22, name: "Boar" }]
    return { mobs }
}
main :: fn () {
    id := 20
    mobs := [{ id, name: "Bear" }, { id: 22, name: "Boar" }]
    world: World = { mobs }
    assert read_world(world) == 42
    assert mobs.count == 2 && mobs.size == 2 * Mob.size
    assert read_id(id) == 20

    direct := [{ id: 20, name: "Bear" }, { id: 22, name: "Boar" }]
    assert sum(direct) == 42
    alias := [{ id: 42, name: "Gnoll" }]
    copied := alias
    assert sum(copied) == 42
    assert alias.size == Mob.size && copied.size == Mob.size

    tuple := ([{ id: 42, name: "Kobold" }], "tuple")
    assert sum(tuple.0) == 42
    assert tuple.1 == "tuple"
    nested := [[{ id: 42, name: "Murloc" }]]
    assert read_nested(nested) == 42
    empty := []
    assert sum(empty) == 0 && empty.count == 0
    bytes := [20, 22]
    assert read_bytes(bytes) == 42 && bytes.size == 2

    typed := [{ id: 42, name: "Spider" }]
    assert read_world(World { mobs: typed }) == 42
    destination: World
    assigned := [{ id: 42, name: "Wolf" }]
    destination = { mobs: assigned }
    assert read_world(destination) == 42

    owned := make_owned()
    assert sum(owned.mobs) == 42
    prn("contextual array inference passed")
}
¬
0
¬
contextual array inference passed

¬
hir 0
¬
