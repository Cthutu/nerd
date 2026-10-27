Problem :: plex {}
impl Display for Problem {
    show :: fn (_self: ^Self) -> string { prn("unexpected display")
 return "unreachable" }
}
main :: fn (args: []string) -> i32\Problem {
    assert args.count >= 1
    defer prn("defer")
    undo prn("not on success")
    return 23
}

¬
23

¬
defer

¬
delete

¬

¬
run

¬
