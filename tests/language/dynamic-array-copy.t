Mob :: plex { id u32 name string }
Location :: plex { mobs [..]i32 }
World :: plex { mobs [..]Mob locations [..]Location }
Holder :: union { items [..]i32 bits u64 }
make_world :: fn () -> World {
    mobs := [{ id: 1 name: "Bear" }, { id: 2 name: "Boar" }]
    locations := [Location { mobs: [1, 2] }]
    return { mobs locations }
}
copy_slice :: fn (items: []i32) -> [..]i32 { return items }
copy_local :: fn () -> [..]i32 {
    items := [10, 20, 30]
    return items
}
consume :: fn (items: [..]i32) -> i32 {
    defer items.free()
    items[0] = items[0] + 100
    return items[0]
}
source_calls: i32
get_view :: fn (items: []i32) -> []i32 {
    source_calls += 1
    return items
}
copy_generic :: fn [T] (items: [..]T) -> [..]T { return items }
copy_optional :: fn (items: []i32) -> ?[..]i32 { return items }
copy_runtime :: fn (count: usize) -> [..]i32 {
    items: [count]i32
    items[0] = 42
    view := items[..]
    copy: [..]i32 = view
    return copy
}
main :: fn () {
    source := [1, 2, 3, 4]
    fixed_copy: [..]i32 = source
    defer fixed_copy.free()
    assert source.size == 4 * i32.size
    assert fixed_copy.count == 4 && fixed_copy.capacity >= 4
    source[0] = 9
    assert fixed_copy[0] == 1
    fixed_copy[1] = 8
    assert source[1] == 2

    view := source[1..3]
    slice_copy: [..]i32 = view
    defer slice_copy.free()
    assert slice_copy.count == 2 && slice_copy[0] == 2 && slice_copy[1] == 3
    assert consume(view) == 102 && source[1] == 2
    assert consume(source) == 109 && source[0] == 9

    once: [..]i32 = get_view(view)
    defer once.free()
    assert source_calls == 1 && once.count == 2
    returned := copy_slice(view)
    defer returned.free()
    assert returned[0] == 2
    local := copy_local()
    defer local.free()
    assert local[0] == 10 && local[2] == 30
    generic := copy_generic(source)
    defer generic.free()
    assert generic[0] == 9
    generic_slice := copy_generic(view)
    defer generic_slice.free()
    assert generic_slice.count == 2
    runtime := copy_runtime(3)
    defer runtime.free()
    assert runtime.count == 3 && runtime[0] == 42
    holder_union := Holder { items: view }
    defer holder_union.items.free()
    assert holder_union.items.count == 2 && holder_union.items[0] == 2
    optional := copy_optional(view)
    optional_copy := optional else { assert no return }
    defer optional_copy.free()
    assert optional_copy.count == 2 && optional_copy[1] == 3

    destination: [..]i32
    destination = view
    assert destination.count == 2
    destination.free()
    destination = source
    defer destination.free()
    assert destination.count == 4 && destination[0] == 9
    holder: Location
    holder.mobs = view
    defer holder.mobs.free()
    assert holder.mobs[1] == 3

    empty_fixed: [0]i32
    empty: [..]i32 = empty_fixed
    defer empty.free()
    empty_view: []i32
    nil_copy: [..]i32 = empty_view
    defer nil_copy.free()
    assert empty.count == 0 && nil_copy.count == 0
    empty.push(42)
    nil_copy.push(7)
    assert empty[0] == 42 && nil_copy[0] == 7

    world := make_world()
    defer {
        world.locations[0].mobs.free()
        world.locations.free()
        world.mobs.free()
    }
    assert world.mobs[1].name == "Boar"
    assert world.locations[0].mobs[1] == 2

    -- Copy elements, without cloning the resources inside them.
    nested_source := [Location { mobs: [1] }]
    nested: [..]Location = nested_source[..]
    defer {
        nested.free()
        nested_source[0].mobs.free()
    }
    nested[0].mobs[0] = 42
    assert nested_source[0].mobs[0] == 42
    alias := fixed_copy
    alias[0] = 77
    assert fixed_copy[0] == 77
    independent: [..]i32 = fixed_copy[..]
    defer independent.free()
    independent[0] = 88
    assert fixed_copy[0] == 77
    prn("dynamic array copies passed")
}
¬
0
¬
dynamic array copies passed

¬
hir 0
¬
