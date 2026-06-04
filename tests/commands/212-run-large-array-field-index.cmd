Doc :: plex {
    memory [65536]u8
}

main :: fn () -> i32 {
    doc: Doc
    doc.memory[65535] = 42
    return doc.memory[65535].as(i32)
}
¬
42
¬

¬
delete
¬
