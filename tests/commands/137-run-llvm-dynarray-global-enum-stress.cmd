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
    for i: u32 = 0; i < 100000; i += 1 {
        push_event(Event.Resize(i, i + 1))
        event := poll()
        on event {
            Resize(width, height) => {
                on width != i => return 1
                on height != i + 1 => return 2
            }
            else => return 3
        }
    }

    on events.count != 0 => return 4
    return 0
}
¬
0
¬

¬
delete
¬
--llvm
