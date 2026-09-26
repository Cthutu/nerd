Key :: enum { Q Escape Other }
Event :: enum { Press { key Key } None }

matches :: fn (event: Event) -> bool {
    return on event {
        Press { key: Q }, Press { key: Escape } => yes
        else => no
    }
}

main :: fn () {
    assert matches(Event.Press { key: Q })
    assert matches(Event.Press { key: Escape })
    assert !matches(Event.Press { key: Other })
    assert !matches(Event.None)
}
¬
0
¬
