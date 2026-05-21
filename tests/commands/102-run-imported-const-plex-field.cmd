use test.imported_const_plex

main :: fn () -> i32 {
    flags :: Flags {
        value: A | B
        ...
    }
    return flags.value.as(i32)
}
¬
3
¬

¬
delete
