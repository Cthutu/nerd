Key :: enum { Q Escape }
Event :: enum { Press { key Key } }
main :: fn () {
    event := Event.Press { key: Q }
    on event {
        Press { key: Q }, Press { key: ESC } => {}
    }
}
¬
Key :: enum {
    Q
    Escape
}

Event :: enum {
    Press {
        key Key
    }
}

main :: fn () {
    event := Event.Press { key: Q }
    on event {
        Press { key: Q },
        Press { key: ESC } => {
        }
    }
}
