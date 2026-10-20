=== case: 243-implicit-box-borrows ===
use test.generics

borrow :: fn (owner: ^box[i32]) -> []i32 { return owner^ }
read_slice :: fn (values: []i32) -> i32 { return values[0] + values[1] }
first :: fn [T] (values: []T) -> T { return values[0] }
pointee :: fn [T] (value: ^T) -> T { return value^ }
Reader :: plex { }
impl Reader {
    read :: fn [T] (self: ^Self, values: []T) -> T { return values[0] }
    pointer :: fn [T] (self: ^Self, value: ^T) -> T { return value^ }
}
Pair :: [2]i32
Chunk :: plex {
    byte u8
    word u64
}
Holder :: plex {
    values []i32
    pointer ^i32
}
main :: fn () {
    owner := box[i32](3)
    view: []i32 = owner
    assert view.count == 3 && view.data == owner.data
    view[0] = 20
    view[1] = 22
    view[2] = 9
    pointer: ^i32 = owner
    assert pointer^ == 20 && owner.count == 3
    assert read_slice(owner) == 42
    assert first(owner) == 20 && pointee(owner) == 20
    assert pick(owner, 0) == 22
    reader: Reader
    assert reader.read(owner) == 20 && reader.pointer(owner) == 20
    holder := Holder { values: owner pointer: owner }
    assert holder.values.data == pointer && holder.pointer == pointer
    returned := borrow(^owner)
    returned[0] = 21
    assert view[0] == 21 && pointee(owner) == 21
    assigned: []i32
    assigned = owner
    assert assigned.count == 3 && assigned[2] == 9
    reassigned: ^i32
    reassigned = owner
    assert reassigned^ == 21 && owner != nil
    wrapped: ?[]i32 = owner
    on wrapped {
        value => { assert value.count == 3 }
        else => { assert no }
    }
    singleton := box[u8]()
    single: []u8 = singleton
    assert single.count == 1
    single[0] = 42
    assert first(singleton) == 42
    zero := box[i32](0)
    zero_view: []i32 = zero
    assert zero_view.count == 0
    chunks := box[Chunk](2)
    chunk_view: []Chunk = chunks
    assert chunk_view.count == 2
    chunk_view[1].word = 17
    assert chunks.data.as([]Chunk, chunks.count)[1].word == 17
    arrays := box[Pair](2)
    array_view: [][2]i32 = arrays
    assert array_view.count == 2
    absent: box[i32] = nil
    empty: []i32 = absent
    null_pointer: ^i32 = absent
    assert empty.count == 0 && empty.data == nil && null_pointer == nil
    assert reader.read(owner) == 21 && owner != nil
    moved := owner
    assert owner == nil && moved != nil
    assert view[1] == 22 && assigned[0] == 21
    moved.free()
    -- The borrowed views now dangle and must no longer be accessed.
    assert owner == nil && moved == nil
    prn("Implicit box borrows passed")
}
¬
0
¬
Implicit box borrows passed
¬

¬

=== case: 153-run-box-allocation ===
Map :: plex {
    x i32
}

read_map :: fn (map: ^Map) -> i32 {
    return map.x
}

main :: fn () -> i32 {
    map: box[Map] = nil
    on map != nil => return 1

    map = box[Map]()
    map.x = 41
    on read_map(map) != 41 => return 2

    map.free()
    on map != nil => return 3

    return 42
}
¬
42
¬

¬

¬

=== case: 155-build-box-dynarray-count-in-nested-range ===
Map :: plex {
    rooms [..]Room
}

Room :: plex {
    x u32
}

gen :: fn (num_rooms: u32) -> box[Map] {
    map := box[Map]()
    room: Room

    for _room in [0 .. num_rooms] {
        for $repeat {
            for _i in [0 .. map.rooms.count] {
                on yes => {
                    again $repeat
                }
            }
            else {
                break $repeat
            }
        }

        map.rooms.push(room)
    }

    return map
}

main :: fn () {
    map := gen(1)
    map.rooms.free()
    map.free()
}
¬
0
¬

¬

¬

=== case: 156-run-box-implicit-bool ===
Item :: plex {
    value i32
}

accept_bool :: fn (value: bool) -> i32 {
    return on value {
        yes => 1
        else => 0
    }
}

main :: fn () -> i32 {
    item: box[Item] = nil

    on item => return 1

    item = box[Item]()
    item.value = 7

    on !item => return 2

    ok: bool = item
    on !ok => return 3

    on item && ok => {
        on accept_bool(item) != 1 => return 4
    }
    else {
        return 5
    }

    assert item

    item.free()
    on item => return 6

    return 42
}
¬
42
¬

¬

¬

=== case: 157-run-box-move-and-cleanup ===
Map :: plex {
    value i32
}

global_map: box[Map]

take :: fn (map: box[Map]) -> i32 {
    on !map => return 1
    return map.value
}

make :: fn () -> box[Map] {
    map := box[Map]()
    map.value = 42
    return map
}

main :: fn () -> i32 {
    first := make()
    on !first => return 2

    result := take(first)
    on result != 42 => return 3
    on first => return 4

    second := make()
    third := second
    on second => return 5
    on !third => return 6

    third.free()
    on third => return 7

    global_map = make()
    on !global_map => return 8
    on take(global_map) != 42 => return 9
    on global_map => return 10

    return 42
}
¬
42
¬

¬

¬

=== case: 214-run-counted-box-allocation ===
take :: fn (bytes: box[u8]) -> i32 {
    on bytes.count != 4 => return 1
    view := bytes.data.as([]u8, bytes.count)
    return view[0].as(i32) + view[1].as(i32) + view[2].as(i32) + view[3].as(i32)
}

main :: fn () -> i32 {
    bytes := box[u8](4)
    on bytes.count != 4 => return 2
    view := bytes.data.as([]u8, bytes.count)

    view[0] = 10
    view[1] = 11
    view[2] = 12
    view[3] = 9

    result := take(bytes)
    on bytes => return 3

    return result
}
¬
42
¬

¬

¬

=== case: 215-run-box-field-return-ownership ===
Holder :: plex {
    image box[u8]
}

make :: fn () -> Holder {
    image := box[u8](4)
    view := image.data.as([]u8, image.count)
    view[0] = 10
    view[1] = 11
    view[2] = 12
    view[3] = 9

    return Holder { image }
}

sum :: fn (holder: Holder) -> i32 {
    on holder.image.count != 4 => return 1
    view := holder.image.data.as([]u8, holder.image.count)
    return view[0].as(i32) + view[1].as(i32) + view[2].as(i32) + view[3].as(i32)
}

main :: fn () -> i32 {
    holder := make()
    return sum(holder)
}
¬
42
¬

¬

¬

=== case: global-box-transfer ===
owned: box[i32]
take_global :: fn () -> box[i32] { return owned }
consume :: fn (value: box[i32]) -> i32 { return value.data^ }
main :: fn () {
    owned = box[i32]()
    owned.data^ = 42
    moved := take_global()
    assert !owned && moved.data^ == 42
    owned = moved
    assert !moved
    assert consume(owned) == 42
    assert !owned
    prn("Global box transfers passed")
}
¬
0
¬
Global box transfers passed
¬

¬
