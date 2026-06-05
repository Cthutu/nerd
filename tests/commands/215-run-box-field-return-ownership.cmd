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
delete
¬
