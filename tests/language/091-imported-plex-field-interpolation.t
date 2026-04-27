boxmod :: mod test.imported_plex

consume :: fn (text: string) {
    on text.count == 0 => return
    return
}

main :: fn () {
    box := boxmod.make_box(7)
    consume($"Value: {box.value}")
}
¬
0
¬

¬

¬
