main :: fn (args: []string) -> ?void {
    defer prn("always")
    undo prn("not on success")
    on args.count == 0 => return nil
    prn("success")
    return
}
¬
0
¬
success
always

¬
delete
¬
