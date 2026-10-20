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
