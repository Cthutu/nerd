fail :: fn () -> ?void { return nil }
main :: fn () -> ?void {
    defer prn("always")
    undo prn("undo")
    fail()? undo prn("not registered")
}
¬
1
¬
undo
always

¬
delete
¬
