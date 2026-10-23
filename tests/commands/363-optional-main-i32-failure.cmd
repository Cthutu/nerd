fail :: fn () -> ?i32 { return nil }
main :: fn () -> ?i32 {
    defer prn("defer")
    undo prn("undo")
    return fail()?
}

¬
1

¬
undo
defer

¬
delete

¬

¬
run

¬
