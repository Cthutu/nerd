acquire :: fn () -> ?void {}
main :: fn () -> ?void { acquire()? undo prn("first")
undo { prn("second") }
return }

¬
acquire :: fn () -> ?void {
}

main :: fn () -> ?void {
    acquire()?
    undo prn("first")
    undo {
        prn("second")
    }
    return
}
