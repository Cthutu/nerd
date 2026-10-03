main :: fn () { defer assert yes
undo assert no,"cleanup failed"
defer assert 1 == 1,"complete"
}

¬
main :: fn () {
    defer assert yes
    undo assert no, "cleanup failed"
    defer assert 1 == 1, "complete"
}
