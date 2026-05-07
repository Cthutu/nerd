Event::enum{KeyDown(KeyEvent)}
KeyEvent::plex{keycode Key modifiers i32 char u32}
Key::enum{Escape=41 Other=42}
main::fn()=>on event{
KeyDown(gfx.FrameKeyEvent{
keycode:Escape
})=>{}
KeyDown(KeyEvent{keycode:Other})=>{}
}
¬
Event :: enum {
    KeyDown(KeyEvent)
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

main :: fn () =>
    on event {
        KeyDown(gfx.FrameKeyEvent { keycode: Escape }) => {
        }
        KeyDown(KeyEvent { keycode: Other }) => {
        }
    }
