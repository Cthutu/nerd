Key :: enum { A B }
Event :: enum { None KeyDown(Key) Resize(u32, u32) }

events: [..]Event

push_event :: fn (event: Event) {
    events.push(event)
}

poll :: fn () -> Event {
    on events.count == 0 => return Event.None
    event := events[0]
    events.delete(0)
    return event
}

main :: fn () -> i32 {
    push_event(Event.KeyDown(Key.A))
    event := poll()
    return on event {
        Event.KeyDown(A) => 0
        else => 1
    }
}
¬
0
¬

¬
keep
¬

¬
build
