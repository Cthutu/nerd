Event :: enum {
    None
    Resized { width i32 }
    KeyPress { code i32 }
}
handle :: fn (event: Event) -> i32 {
    on event {
        None => {}
        Resized { width: _ } => {}
        KeyPress { code: 1 }, KeyPress { code: 2 } => { return 10 }
    }
    return 20
}
handle_void :: fn (event: Event, count: ^i32) {
    defer count^ += 100
    on event {
        None => {}
        Resized { width: _ } => { count^ += 1 }
        KeyPress { code: 1 }, KeyPress { code: 2 } => { return }
    }
    count^ += 10
}
main :: fn () -> i32 {
    on handle(Event.Resized { width: 5 }) != 20 => return 1
    on handle(Event.None) != 20 => return 2
    on handle(Event.KeyPress { code: 3 }) != 20 => return 3
    on handle(Event.KeyPress { code: 1 }) != 10 => return 4
    on handle(Event.KeyPress { code: 2 }) != 10 => return 5
    count: i32 = 0
    handle_void(Event.Resized { width: 5 }, ^count)
    on count != 111 => return 6
    count = 0
    handle_void(Event.KeyPress { code: 1 }, ^count)
    on count != 100 => return 7
    count = 0
    handle_void(Event.KeyPress { code: 3 }, ^count)
    on count != 110 => return 8
    prn("Enum branches continue correctly")
    return 0
}
¬
0
¬
Enum branches continue correctly

¬
delete
¬
