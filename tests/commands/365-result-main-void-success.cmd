Problem :: plex {}
impl Display for Problem {
    show :: fn (_self: Self) -> string { prn("unexpected display")
 return "unreachable" }
}
main :: fn () -> void\Problem {
    defer prn("defer")
    undo prn("not on success")
    return
}

¬
0

¬
defer

¬
delete

¬

¬
run

¬
