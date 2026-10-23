main :: fn (args: []string) -> ?i32 {
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
