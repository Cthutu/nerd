Envelope :: enum { Empty Owned(arena) }
make_envelope :: fn () -> Envelope { return Envelope.Owned(arena(4096)) }
Holder :: plex { storage arena }
make_arena :: fn () -> arena { return arena(4096) }
fill :: fn (storage: ^arena) -> ^i32 {
    value := storage.alloc[i32]()
    value^ = 42
    return value
}
borrow :: fn [T](value: ^T) -> ^T { return value }
main :: fn () {
    empty := Envelope.Empty
    on empty { Empty => {} Owned(_) => {} }
    absent: ?arena
    assert absent == nil
    assert nil == absent
    assert !(absent != nil)
    storage := make_arena()
    shared := ^storage
    shared_again := shared
    first := fill(shared_again)
    second := fill(borrow[arena](^storage))
    assert first != second
    assert first^ == 42
    assert second^ == 42
    assert storage.mark() >= 8
    storage.done()

    storage = arena(4096)
    assert storage.mark() == 0
    storage.done()

    lazy: arena
    value := fill(^lazy)
    assert value^ == 42
    lazy.done()

    holder := Holder { storage: arena(4096) }
    assert fill(^holder.storage)^ == 42
    holder.storage.done()

    arenas := [arena(4096)]
    view := arenas[..]
    view_copy := view
    assert fill(^view_copy[0])^ == 42
    for entry in view_copy { entry.reset() }
    assert arenas[0].mark() == 0
    arenas[0].done()

    pair := (arena(4096), 42)
    assert fill(^pair.0)^ == pair.1
    pair.0.done()

    temp := temp_arena
    temp_copy := temp
    mark := temp.mark()
    assert fill(temp_copy)^ == 42
    temp.restore(mark)
    prn("arena borrowing works")
}
¬
0
¬
arena borrowing works
¬
delete
