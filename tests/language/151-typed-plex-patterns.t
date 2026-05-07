use std.io

Event :: enum {
    KeyDown(KeyEvent)
    KeyUp(KeyEvent)
}

KeyEvent :: plex {
    keycode   Key
    modifiers i32
    char      u32
}

Key :: enum {
    Escape = 41
    Other  = 42
}

score :: fn (event: Event) -> i32 {
    return on event {
        KeyDown(KeyEvent { keycode: Escape }) => 1
        KeyDown(KeyEvent { keycode: Other }) => 2
        else => 0
    }
}

main :: fn () -> i32 {
    escape := Event.KeyDown(KeyEvent { keycode: Escape, modifiers: 0, char: 0 })
    other := Event.KeyDown(KeyEvent { keycode: Other, modifiers: 1, char: 2 })
    up := Event.KeyUp(KeyEvent { keycode: Escape, modifiers: 0, char: 0 })
    prn($"typed plex patterns {score(escape)} {score(other)} {score(up)}")
    return score(escape)
}
¬
1
¬
typed plex patterns 1 2 0

¬

¬

