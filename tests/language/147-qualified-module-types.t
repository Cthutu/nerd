boxmod :: use test.imported_plex

Holder :: plex {
    box boxmod.Box
    ptr ^boxmod.Box
}

main :: fn () -> i32 {
    box := boxmod.make_box(7)
    holder := Holder { box, ptr: ^box }
    return (holder.box.value + holder.ptr^.value).as(i32)
}
¬
14
¬

¬

¬
